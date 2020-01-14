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

#include <editor/renderer_imgui.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <graphics/graphic_manager.h>
#include <graphics/mesh_vertex.h>
#include <graphics/window.h>

namespace dm
{
static void check_vk_result(VkResult err)
{
	if (err == 0) return;
	printf("VkResult %d\n", err);
	if (err < 0)
		abort();
}

RendererImGui::RendererImGui(const Pipeline::Stage &pipelineStage) :
	RenderPipeline(pipelineStage)
{
	auto surface = GraphicManager::Get()->GetSurface();
	auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();
	auto window = GraphicManager::Get()->GetWindow();

	ImGui::CreateContext();
	// Color scheme
	// Dimensions
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	check_vk_result(vkCreateDescriptorPool(*logicalDevice, &pool_info, nullptr, &m_GDescriptorPool));

	ImGui_ImplSDL2_InitForVulkan(GraphicManager::Get()->GetWindow()->GetWindow());


	const auto surfaceCapabilities = surface->GetCapabilities();

	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance = *GraphicManager::Get()->GetInstance();
	initInfo.PhysicalDevice = *GraphicManager::Get()->GetPhysicalDevice();
	initInfo.Device = *logicalDevice;
	initInfo.QueueFamily = logicalDevice->GetGraphicsFamily();
	initInfo.Queue = logicalDevice->GetGraphicsQueue(); //Not sure about that
	initInfo.PipelineCache = VK_NULL_HANDLE;
	initInfo.DescriptorPool = m_GDescriptorPool;
	initInfo.Allocator = nullptr;
	initInfo.MinImageCount = surfaceCapabilities.minImageCount + 1;
	initInfo.ImageCount = GraphicManager::Get()->GetSwapchain()->GetImageCount();
	initInfo.Subpass = pipelineStage.second;
	initInfo.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&initInfo, GraphicManager::Get()->GetRenderStage(pipelineStage.first)->GetRenderPass()->GetRenderPass()); //Not sure about that

	io.DisplaySize = ImVec2(static_cast<uint32_t>(window->GetSize().x), static_cast<uint32_t>(window->GetSize().y));
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	// Create font texture
	unsigned char* fontData;
	int texWidth, texHeight;
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

	// Upload Fonts
	{
		// Use any command queue
		CommandBuffer commandBuffer = CommandBuffer();

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

		commandBuffer.SubmitIdle();

		check_vk_result(vkDeviceWaitIdle(*logicalDevice));
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	NewFrame();
}

RendererImGui::~RendererImGui()
{
	auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();
	vkDestroyDescriptorPool(*logicalDevice, m_GDescriptorPool, nullptr);

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f };

float objectMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
  0.f, 1.f, 0.f, 0.f,
  0.f, 0.f, 1.f, 0.f,
  0.f, 0.f, 0.f, 1.f };

void RendererImGui::NewFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(GraphicManager::Get()->GetWindow()->GetWindow());
	ImGui::NewFrame();
}

void RendererImGui::Draw(const CommandBuffer& commandBuffer)
{
	ImGui::Render();

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetCommandBuffer());
	NewFrame();
}
}
