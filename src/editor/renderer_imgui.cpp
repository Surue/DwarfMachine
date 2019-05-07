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
#include <imgui_internal.h>
#include "imgui_gizmo.h"

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
	check_vk_result(vkCreateDescriptorPool(*logicalDevice, &pool_info, nullptr, &m_GDescriptorPool));

	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForVulkan(Engine::Get()->GetGraphicManager()->GetWindow()->GetWindow());


	// Color scheme
	// Dimensions
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	auto surfaceCapabilities = surface->GetCapabilities();

	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance = *Engine::Get()->GetGraphicManager()->GetInstance();
	initInfo.PhysicalDevice = *Engine::Get()->GetGraphicManager()->GetPhysicalDevice();
	initInfo.Device = *logicalDevice;
	initInfo.QueueFamily = logicalDevice->GetGraphicsFamily();
	initInfo.Queue = logicalDevice->GetGraphicsQueue(); //Not sure about that
	initInfo.PipelineCache = VK_NULL_HANDLE;
	initInfo.DescriptorPool = m_GDescriptorPool;
	initInfo.Allocator = nullptr;
	initInfo.MinImageCount = surfaceCapabilities.minImageCount + 1;
	initInfo.ImageCount = Engine::Get()->GetGraphicManager()->GetSwapchain()->GetImageCount();
	initInfo.Subpass = pipelineStage.second;
	initInfo.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&initInfo, Engine::Get()->GetGraphicManager()->GetRenderStage(pipelineStage.first)->GetRenderPass()->GetRenderPass()); //Not sure about that

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

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

		commandBuffer.SubmitIdle();

		check_vk_result(vkDeviceWaitIdle(*logicalDevice));
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

RendererImGui::~RendererImGui()
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
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
	ImGui_ImplSDL2_NewFrame(Engine::Get()->GetGraphicManager()->GetWindow()->GetWindow());
	ImGui::NewFrame();

	/*ImGuizmo::SetOrthographic(false);

	ImGuizmo::BeginFrame();
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	auto camera = Engine::Get()->GetGraphicManager()->GetCamera();
	ImGuizmo::DrawCube(&camera->viewMatrix[0][0], &camera->proj[0][0], objectMatrix);
	ImGuizmo::DrawGrid(&camera->viewMatrix[0][0], &camera->proj[0][0], identityMatrix, 10.f); //Fix this function
	ImGuizmo::Manipulate(&camera->viewMatrix[0][0], &camera->proj[0][0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, objectMatrix, NULL, NULL);

	bool show_demo_window = true;

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::DockBuilderRemoveNode(1); // Clear out existing layout
	ImGui::DockBuilderAddNode(1, ImGuiConfigFlags_DockingEnable); // Add empty node
	ImGui::DockBuilderSetNodeSize(1, ImVec2(100, 100));

	ImGuiID dock_main_id = 1; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
	ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
	ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

	ImGui::DockBuilderDockWindow("Log", dock_id_bottom);
	ImGui::DockBuilderDockWindow("Properties", dock_id_prop);
	ImGui::DockBuilderDockWindow("Mesh", dock_id_prop);
	ImGui::DockBuilderDockWindow("Extra", dock_id_prop);
	ImGui::DockBuilderFinish(1);*/

	bool show_demo_window = true;

	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &show_demo_window, window_flags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Docking"))
		{
			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspaceFlags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspaceFlags ^= ImGuiDockNodeFlags_NoSplit;
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspaceFlags & ImGuiDockNodeFlags_NoResize) != 0))                dockspaceFlags ^= ImGuiDockNodeFlags_NoResize;
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspaceFlags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspaceFlags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspaceFlags ^= ImGuiDockNodeFlags_PassthruCentralNode;
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspaceFlags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspaceFlags ^= ImGuiDockNodeFlags_AutoHideTabBar;
			ImGui::Separator();

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();

	//Hierarchy
	ImGui::Begin("Hierarchy");

	ImGui::End();

	//Inspector
	ImGui::Begin("Inspector");

	ImGui::End();

	//Render
	ImGui::Render();
}

void RendererImGui::Draw(const CommandBuffer& commandBuffer)
{
	NewFrame();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetCommandBuffer());
}
}
