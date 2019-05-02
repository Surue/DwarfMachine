/*
MIT License

Copyright (c) 2019 Nicolas Schneider

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <stdexcept>
#include <chrono>
#include <algorithm>

#include <graphics/graphic_manager.h>
#include <engine/Input.h>
#include <graphics/window.h>

#include <SPIRV/GlslangToSpv.h>

namespace dm
{
GraphicManager::GraphicManager(Engine& engine) : m_Engine(engine)
{
	glslang::InitializeProcess();

	m_Window = std::make_unique<Window>(m_Engine);
	InitWindow();

	m_Instance = std::make_unique<Instance>(m_Window->GetWindow());
	m_PhysicalDevice = std::make_unique<PhysicalDevice>(m_Instance.get());
	m_Surface = std::make_unique<Surface>(m_Instance.get(), m_PhysicalDevice.get(), m_Window.get());
	m_LogicalDevice = std::make_unique<LogicalDevice>(m_Instance.get(), m_PhysicalDevice.get(), m_Surface.get());

	m_CurrentFrame = 0;

	CreatePipelineCache();

}

RenderStage* GraphicManager::GetRenderStage(const uint32_t& index) const
{
	if(m_RenderStages.empty() || m_RenderStages.size() < index)
	{
		return nullptr;
	}

	return m_RenderStages.at(index).get();
}

void GraphicManager::CreatePipelineCache()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(*m_LogicalDevice, &pipelineCacheCreateInfo, nullptr, &m_PipelineCache);
}

void GraphicManager::InitWindow()
{
	m_Window->Init();
}

void GraphicManager::FrameBufferResizeCallback(SDL_Window* window, int width, int height)
{
	//TODO Faire en sorte que quand on resize une window cette fonction soit appelée 
	const auto app = reinterpret_cast<GraphicManager*>(SDL_GetWindowData(window, "Vulkan App"));
	//app->m_FrameBufferResized = true; 
}

void GraphicManager::Init()
{
	
}

void GraphicManager::Destroy()
{

}

GraphicManager::~GraphicManager()
{
	auto graphicsQueue = m_LogicalDevice->GetGraphicsQueue();

	vkQueueWaitIdle(graphicsQueue);

	glslang::FinalizeProcess();
		
	vkDestroyPipelineCache(*m_LogicalDevice, m_PipelineCache, nullptr);

	for (size_t i = 0; i < m_InFlightFences.size(); i++)
	{
		vkDestroyFence(*m_LogicalDevice, m_InFlightFences[i], nullptr);
		vkDestroySemaphore(*m_LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(*m_LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
	}
}

void GraphicManager::Update()
{
	if (m_RenderManager == nullptr)
	{
		return;
	}

	if (!m_RenderManager->m_Started)
	{
		m_RenderManager->Start();
		m_RenderManager->m_Started = true;
	}

	m_RenderManager->Update();

	auto &stages = m_RenderManager->GetRendererContainer().GetStages();

	std::optional<uint32_t> renderpass;
	uint32_t subpass = 0;

	VkResult acquireResult = m_Swapchain->AcquireNextImage(m_ImageAvailableSemaphores[m_CurrentFrame]);

	if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return;
	}

	if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
	{
		return;
	}

	for (auto &[key, renderPipelines] : stages)
	{
		if (renderpass != key.first)
		{
			// Ends the previous renderpass.
			if (renderpass)
			{
				EndRenderpass(*GetRenderStage(*renderpass));
			}

			renderpass = key.first;
			subpass = 0;

			// Starts the next renderpass.
			auto renderStage = GetRenderStage(*renderpass);
			renderStage->Update();
			auto startResult = StartRenderpass(*renderStage);

			if (!startResult)
			{
				return;
			}
		}

		auto renderStage = GetRenderStage(*renderpass);

		// Changes the subpass.
		if (subpass != key.second)
		{
			uint32_t difference = key.second - subpass;

			if (subpass == static_cast<uint32_t>(renderStage->GetSubpasses().size() - 1))
			{
				difference -= 1;
			}

			for (uint32_t d = 0; d < difference; d++)
			{
				vkCmdNextSubpass(*m_CommandBuffers[m_Swapchain->GetActiveImageIndex()], VK_SUBPASS_CONTENTS_INLINE);
			}

			subpass = key.second;
		}

		// Renders subpass render pipeline.
		for (auto &renderPipeline : renderPipelines)
		{
			if (!renderPipeline->IsEnabled())
			{
				continue;
			}

			renderPipeline->Draw(*m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]);
		}
	}

	// Ends the last renderpass.
	if (renderpass)
	{
		auto renderStage = GetRenderStage(*renderpass);

		if (renderStage != nullptr)
		{
			EndRenderpass(*renderStage);
		}
	}
}

void GraphicManager::SetMainCamera(Camera* camera)
{
	m_MainCamera = camera;
}

Window* GraphicManager::GetWindow() const
{
	return m_Window.get();
}

void GraphicManager::SetRenderStages(std::vector<std::unique_ptr<RenderStage>> renderStages)
{
	VkExtent2D displayExtent = { static_cast<uint32_t>(m_Window->GetSize().x), static_cast<uint32_t>(m_Window->GetSize().y) };

	m_RenderStages = std::move(renderStages);
	m_Swapchain = std::make_unique<Swapchain>(displayExtent);

	if(m_InFlightFences.size() != m_Swapchain->GetImageCount())
	{
		for(size_t i = 0; i < m_InFlightFences.size(); i++)
		{
			vkDestroyFence(*m_LogicalDevice, m_InFlightFences[i], nullptr);
			vkDestroySemaphore(*m_LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(*m_LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
		}

		m_ImageAvailableSemaphores.resize(m_Swapchain->GetImageCount());
		m_RenderFinishedSemaphores.resize(m_Swapchain->GetImageCount());
		m_InFlightFences.resize(m_Swapchain->GetImageCount());
		m_CommandBuffers.resize(m_Swapchain->GetImageCount());

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for(size_t i = 0; i < m_InFlightFences.size(); i++)
		{
			if (vkCreateSemaphore(m_LogicalDevice->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_LogicalDevice->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_LogicalDevice->GetLogicalDevice(), &fenceCreateInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame");
			}

			m_CommandBuffers[i] = std::make_unique<CommandBuffer>(false);
		}
	}

	for (const auto &renderStage : m_RenderStages)
	{
		renderStage->Rebuild(*m_Swapchain);
	}

	RecreateAttachmentsMap();
}

const std::shared_ptr<CommandPool> &GraphicManager::GetCommandPool(const std::thread::id &threadId)
{
	auto it = m_CommandPools.find(threadId);

	if (it != m_CommandPools.end())
	{
		return it->second;
	}

	m_CommandPools.emplace(threadId, std::make_shared<CommandPool>(threadId));
	return m_CommandPools.find(threadId)->second; // TODO: Cleanup.
}

void GraphicManager::UpdateMainCamera() const
{
	const auto windowSize = m_Engine.GetSettings().windowSize;
	m_MainCamera->proj = Matrix4::PerspectiveMatrix(45 * (3.14f / 180), windowSize.x / static_cast<float>(windowSize.y), 0.1f, 100.0f);
}

void GraphicManager::RecreatePass(RenderStage& renderStage)
{
	auto graphicQueue = m_LogicalDevice->GetGraphicsQueue();

	VkExtent2D displayExtent = { m_Window->GetSize().x, m_Window->GetSize().y };

	if(renderStage.HasSwapchain() && m_Swapchain->IsSameExtent(displayExtent))
	{
		m_Swapchain = std::make_unique<Swapchain>(displayExtent);
	}

	renderStage.Rebuild(*m_Swapchain);
	RecreateAttachmentsMap();
}

void GraphicManager::RecreateAttachmentsMap()
{
	m_Attachments.clear();

	for(const auto &renderStage : m_RenderStages)
	{
		m_Attachments.insert(renderStage->m_Descriptors.begin(), renderStage->m_Descriptors.end());
	}
}

bool GraphicManager::StartRenderpass(RenderStage& renderStage)
{
	if(renderStage.IsOutOfDate())
	{
		RecreatePass(renderStage);
		return false;
	}

	if(!m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]->IsRunning())
	{
		vkWaitForFences(*m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
		m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]->Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
	}

	VkRect2D renderArea = {};
	renderArea.offset = { 0 ,0 };
	renderArea.extent = { static_cast<uint32_t>(renderStage.GetSize().x), static_cast<uint32_t>(renderStage.GetSize().y) };

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(renderArea.extent.width);
	viewport.height = static_cast<float>(renderArea.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(*m_CommandBuffers[m_Swapchain->GetActiveImageIndex()], 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = renderArea.extent;
	vkCmdSetScissor(*m_CommandBuffers[m_Swapchain->GetActiveImageIndex()], 0, 1, &scissor);

	auto clearValues = renderStage.GetClearValues();

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderStage.GetRenderPass()->GetRenderPass();
	renderPassBeginInfo.framebuffer = renderStage.GetActiveFramebuffer(m_Swapchain->GetActiveImageIndex());
	renderPassBeginInfo.renderArea = renderArea;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(
		*m_CommandBuffers[m_Swapchain->GetActiveImageIndex()], 
		&renderPassBeginInfo, 
		VK_SUBPASS_CONTENTS_INLINE);
}

void GraphicManager::EndRenderpass(RenderStage& renderStage)
{
	auto presentQueue = m_LogicalDevice->GetPresentQueue();

	vkCmdEndRenderPass(*m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]);

	if (!renderStage.HasSwapchain())
	{
		return;
	}

	m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]->End();
	m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]->Submit(m_ImageAvailableSemaphores[m_CurrentFrame], m_RenderFinishedSemaphores[m_CurrentFrame], m_InFlightFences[m_CurrentFrame]);
	VkResult presentResult = m_Swapchain->QueuePresent(presentQueue, m_RenderFinishedSemaphores[m_CurrentFrame]);

	if (!(presentResult == VK_SUCCESS || presentResult == VK_SUBOPTIMAL_KHR))
	{
		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreatePass(renderStage);
			return;
		}
		std::cout << presentResult << "\n";
	}

	m_CurrentFrame = (m_CurrentFrame + 1) % m_Swapchain->GetImageCount();
}
}
