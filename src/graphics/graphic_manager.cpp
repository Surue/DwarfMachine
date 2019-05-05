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
std::string GraphicManager::StringifyResultVk(const VkResult &result)
{
	switch (result)
	{
	case VK_SUCCESS:
		return "Success";
	case VK_NOT_READY:
		return "A fence or query has not yet completed";
	case VK_TIMEOUT:
		return "A wait operation has not completed in the specified time";
	case VK_EVENT_SET:
		return "An event is signaled";
	case VK_EVENT_RESET:
		return "An event is unsignaled";
	case VK_INCOMPLETE:
		return "A return array was too small for the result";
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		return "A host memory allocation has failed";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		return "A device memory allocation has failed";
	case VK_ERROR_INITIALIZATION_FAILED:
		return "Initialization of an object could not be completed for implementation-specific reasons";
	case VK_ERROR_DEVICE_LOST:
		return "The logical or physical device has been lost";
	case VK_ERROR_MEMORY_MAP_FAILED:
		return "Mapping of a memory object has failed";
	case VK_ERROR_LAYER_NOT_PRESENT:
		return "A requested layer is not present or could not be loaded";
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		return "A requested extension is not supported";
	case VK_ERROR_FEATURE_NOT_PRESENT:
		return "A requested feature is not supported";
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
	case VK_ERROR_TOO_MANY_OBJECTS:
		return "Too many objects of the type have already been created";
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		return "A requested format is not supported on this device";
	case VK_ERROR_SURFACE_LOST_KHR:
		return "A surface is no longer available";
		//case VK_ERROR_OUT_OF_POOL_MEMORY:
		//return "A allocation failed due to having no more space in the descriptor pool";
	case VK_SUBOPTIMAL_KHR:
		return "A swapchain no longer matches the surface properties exactly, but can still be used";
	case VK_ERROR_OUT_OF_DATE_KHR:
		return "A surface has changed in such a way that it is no longer compatible with the swapchain";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		return "The display used by a swapchain does not use the same presentable image layout";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
	case VK_ERROR_VALIDATION_FAILED_EXT:
		return "A validation layer found an error";
	default:
		return "Unknown Vulkan error";
	}
}

void GraphicManager::CheckVk(const VkResult &result)
{
	if (result >= 0)
	{
		return;
	}

	if constexpr (!ENABLE_VALIDATION_LAYERS)
	{
		return;
	}

	std::string failure = StringifyResultVk(result);

	std::cout << "Vulkan error: " <<  failure.c_str() << ", " << result;
	throw std::runtime_error("Vulkan error: " + failure);
}

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
	//TODO Faire en sorte que quand on resize une window cette fonction soit appel�e 
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
	std::cout << "=============== GraphicManager ===============\n";
	auto graphicsQueue = m_LogicalDevice->GetGraphicsQueue();

	CheckVk(vkQueueWaitIdle(graphicsQueue));

	glslang::FinalizeProcess();

	vkDestroyPipelineCache(*m_LogicalDevice, m_PipelineCache, nullptr);

	for (size_t i = 0; i < m_InFlightFences.size(); i++)
	{
		vkDestroyFence(*m_LogicalDevice, m_InFlightFences[i], nullptr);
		vkDestroySemaphore(*m_LogicalDevice, m_RenderCompletesSemaphore[i], nullptr);
		vkDestroySemaphore(*m_LogicalDevice, m_PresentCompletesSemaphore[i], nullptr);
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

	const auto acquireResult = m_Swapchain->AcquireNextImage(m_PresentCompletesSemaphore[m_CurrentFrame]);

	if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		VkExtent2D displayExtent = { m_Window->GetSize().x, m_Window->GetSize().y };
		m_Swapchain = std::make_unique<Swapchain>(displayExtent);
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

void GraphicManager::SetManager(RenderManager* managerRender)
{
	std::cout << "Set RenderManager\n";
	vkDeviceWaitIdle(*m_LogicalDevice);
	m_RenderManager.reset(managerRender);
}

void GraphicManager::SetRenderStages(std::vector<std::unique_ptr<RenderStage>> renderStages)
{
	VkExtent2D displayExtent = { m_Window->GetSize().x, m_Window->GetSize().y };
	m_RenderStages = std::move(renderStages);
	std::cout << "SetRenderStages\n";
	m_Swapchain = std::make_unique<Swapchain>(displayExtent);

	if (m_InFlightFences.size() != m_Swapchain->GetImageCount())
	{
		for (size_t i = 0; i < m_InFlightFences.size(); i++)
		{
			vkDestroyFence(*m_LogicalDevice, m_InFlightFences[i], nullptr);
			vkDestroySemaphore(*m_LogicalDevice, m_RenderCompletesSemaphore[i], nullptr);
			vkDestroySemaphore(*m_LogicalDevice, m_PresentCompletesSemaphore[i], nullptr);
		}

		m_PresentCompletesSemaphore.resize(m_Swapchain->GetImageCount());
		m_RenderCompletesSemaphore.resize(m_Swapchain->GetImageCount());
		m_InFlightFences.resize(m_Swapchain->GetImageCount());
		m_CommandBuffers.resize(m_Swapchain->GetImageCount());

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < m_InFlightFences.size(); i++)
		{
			CheckVk(vkCreateSemaphore(*m_LogicalDevice, &semaphoreCreateInfo, nullptr, &m_PresentCompletesSemaphore[i]));

			CheckVk(vkCreateSemaphore(*m_LogicalDevice, &semaphoreCreateInfo, nullptr, &m_RenderCompletesSemaphore[i]));

			CheckVk(vkCreateFence(*m_LogicalDevice, &fenceCreateInfo, nullptr, &m_InFlightFences[i]));

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
	return m_CommandPools.find(threadId)->second; 
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

	CheckVk(vkQueueWaitIdle(graphicQueue));

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
	if (renderStage.IsOutOfDate())
	{
		RecreatePass(renderStage);
		return false;
	}

	if (!m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]->IsRunning())
	{
		CheckVk(vkWaitForFences(*m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max()));
		m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]->Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
	}

	VkRect2D renderArea = {};
	renderArea.offset = { 0, 0 };
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
	scissor.offset = { 0, 0 };
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
	vkCmdBeginRenderPass(*m_CommandBuffers[m_Swapchain->GetActiveImageIndex()], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	return true;
}

void GraphicManager::EndRenderpass(RenderStage& renderStage)
{
	const auto presentQueue = m_LogicalDevice->GetPresentQueue();

	vkCmdEndRenderPass(*m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]);

	if (!renderStage.HasSwapchain())
	{
		return;
	}

	m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]->End();
	m_CommandBuffers[m_Swapchain->GetActiveImageIndex()]->Submit(m_PresentCompletesSemaphore[m_CurrentFrame], m_RenderCompletesSemaphore[m_CurrentFrame], m_InFlightFences[m_CurrentFrame]);
	const auto presentResult = m_Swapchain->QueuePresent(presentQueue, m_RenderCompletesSemaphore[m_CurrentFrame]);

	if (!(presentResult == VK_SUCCESS || presentResult == VK_SUBOPTIMAL_KHR))
	{
		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreatePass(renderStage);
			return;
		}
		CheckVk(presentResult);
	}

	m_CurrentFrame = (m_CurrentFrame + 1) % m_Swapchain->GetImageCount();
}
}
