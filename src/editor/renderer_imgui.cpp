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
#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"
#include <graphics/graphic_manager.h>
#include <graphics/model_vertex.h>
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
	auto surface = Engine::Get()->GetGraphicManager()->GetSurface();
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	auto window = Engine::Get()->GetGraphicManager()->GetWindow();

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
	check_vk_result(vkCreateDescriptorPool(*logicalDevice, &pool_info, nullptr, &g_DescriptorPool));

	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForVulkan(Engine::Get()->GetGraphicManager()->GetWindow()->GetWindow());


	// Color scheme
	// Dimensions
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	auto surfaceCapabilities = surface->GetCapabilities();

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = *Engine::Get()->GetGraphicManager()->GetInstance();
	init_info.PhysicalDevice = *Engine::Get()->GetGraphicManager()->GetPhysicalDevice();
	init_info.Device = *logicalDevice;
	init_info.QueueFamily = logicalDevice->GetGraphicsFamily();
	init_info.Queue = logicalDevice->GetGraphicsQueue(); //Not sure about that
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = g_DescriptorPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = surfaceCapabilities.minImageCount + 1;
	init_info.ImageCount = Engine::Get()->GetGraphicManager()->GetSwapchain()->GetImageCount();
	init_info.Subpass = pipelineStage.second;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, Engine::Get()->GetGraphicManager()->GetRenderStage(pipelineStage.first)->GetRenderPass()->GetRenderPass()); //Not sure about that

	io.DisplaySize = ImVec2(window->GetSize().x, window->GetSize().y);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	// Create font texture
	unsigned char* fontData;
	int texWidth, texHeight;
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
	VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

	// Upload Fonts
	{
		// Use any command queue
		CommandBuffer commandBuffer = CommandBuffer();

		commandBuffer.Begin();

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer.GetCommandBuffer());

		commandBuffer.Submit();

		check_vk_result(vkDeviceWaitIdle(*logicalDevice));
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

RendererImGui::~RendererImGui()
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	vkDestroyDescriptorPool(*logicalDevice, g_DescriptorPool, nullptr);

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void RendererImGui::NewFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(Engine::Get()->GetGraphicManager()->GetWindow()->GetWindow());
	ImGui::NewFrame();

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}


	// Render to generate draw buffers
	ImGui::Render();
}

void RendererImGui::Draw(const CommandBuffer& commandBuffer)
{
	NewFrame();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetCommandBuffer());
}
}
