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
RendererImGui::RendererImGui(const Pipeline::Stage &pipelineStage) :
	RenderPipeline(pipelineStage),
	m_VertexBuffer(nullptr),
	m_IndexBuffer(nullptr),
	m_VertexCount(0),
	m_IndexCount(0),
	m_Pipeline(pipelineStage, { "Shaders/ImGui.vert", "Shaders/ImGui.frag" }, { VertexModel::GetVertexInput() })
{
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForVulkan(Engine::Get()->GetGraphicManager()->GetWindow()->GetWindow());

	ImGui_ImplVulkan_InitInfo init_info = {};
	//init_info.Instance = Engine::Get()->GetGraphicManager()->Get;
	//init_info.PhysicalDevice = *Engine::Get()->GetGraphicManager()->GetPhysicalDevice();
	//init_info.Device = *Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	//init_info.QueueFamily = g_QueueFamily;
	//init_info.Queue = g_Queue;
	//init_info.PipelineCache = g_PipelineCache;
	//init_info.DescriptorPool = m_Pipeline.GetDescriptorPool();
	//init_info.Allocator = nullptr;
	//init_info.MinImageCount = g_MinImageCount;
	//init_info.ImageCount = wd->ImageCount;
	//init_info.CheckVkResultFn = check_vk_result;
	//ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

	// Color scheme
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	// Dimensions
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2(Engine::Get()->GetGraphicManager()->GetWindow()->GetSize().x, Engine::Get()->GetGraphicManager()->GetWindow()->GetSize().y);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	// Create font texture
	unsigned char* fontData;
	int texWidth, texHeight;
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
	VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);
}

RendererImGui::~RendererImGui()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void RendererImGui::NewFrame(bool updateFrameGraph)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(Engine::Get()->GetGraphicManager()->GetWindow()->GetWindow());
	ImGui::NewFrame();

	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	// Init imGui windows and elements

	ImVec4 clear_color = ImColor(114, 144, 154);
	static float f = 0.0f;
	ImGui::TextUnformatted("TITRE");

	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Example settings");
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
	ImGui::ShowDemoWindow();

	// Render to generate draw buffers
	ImGui::Render();
}

void RendererImGui::Draw(const CommandBuffer& commandBuffer)
{
	//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}
}
