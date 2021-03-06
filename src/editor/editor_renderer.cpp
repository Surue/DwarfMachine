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

#include <editor/editor_renderer.h>
#include <graphics/render_stage.h>
#include <graphics/graphic_manager.h>
#include <graphics/renderer_meshes.h>
#include <editor/renderer_imgui.h>
#include <engine/engine.h>
#include <graphics/gizmos/renderer_gizmo.h>
#include <graphics/renderer_deferred.h>
#include <graphics/filters/filter_default.h>
#include <graphics/filters/filter_fxaa.h>
#include <graphics/filters/filter_ssao.h>
#include <graphics/filters/filter_ssao_blur.h>
#include <graphics/renderer_directional_shadow.h>
#include <graphics/renderer_forward.h>
#include <graphics/filters/filter_fog.h>
#include <graphics/renderer_terrain.h>
#include "graphics/renderer_meshes_pbr.h"

namespace dm
{
EditorRenderManager::EditorRenderManager()
{
	EditorRenderManager::Start();
	m_Started = true;
}

void EditorRenderManager::Start()
{
	std::vector<std::unique_ptr<RenderStage>> renderStages;

	std::vector<Attachment> renderpassAttachment0 = {
		Attachment(0, "shadow", Attachment::Type::DEPTH, false)
	};

	std::vector<SubpassType> renderpassSubpasses0 = {
		SubpassType(0, { 0 }), //Shadow pass
	};
	renderStages.emplace_back(std::make_unique<RenderStage>(renderpassAttachment0, renderpassSubpasses0, Viewport(glm::vec2(4096, 4096))));

	std::vector<Attachment> renderpassAttachment1 = {
		Attachment(0, "depth", Attachment::Type::DEPTH, false),
		Attachment(1, "swapchain", Attachment::Type::SWAPCHAIN, false, VK_FORMAT_R8G8B8A8_UNORM),
		Attachment(2, "position", Attachment::Type::IMAGE, false, VK_FORMAT_R16G16B16A16_SFLOAT),
		Attachment(3, "diffuse", Attachment::Type::IMAGE, false, VK_FORMAT_R8G8B8A8_UNORM),
		Attachment(4, "normal", Attachment::Type::IMAGE, false, VK_FORMAT_R16G16B16A16_SFLOAT),
		Attachment(5, "material", Attachment::Type::IMAGE, false, VK_FORMAT_R8G8B8A8_UNORM), 
		Attachment(6, "resolved", Attachment::Type::IMAGE, false, VK_FORMAT_R8G8B8A8_UNORM),
		Attachment(7, "ssao", Attachment::Type::IMAGE, false, VK_FORMAT_R8G8B8A8_UNORM)
	};

	std::vector<SubpassType> renderpassSubpasses1 = { 
		SubpassType(0, { 0, 2, 3, 4 ,5}), //Geometry pass
		SubpassType(1, { 0, 7}), //SSAO
		SubpassType(2, { 0, 6}), //Light pass
		SubpassType(3, { 0, 6}), //Forward pass
		SubpassType(4, { 0, 1}) //Post process pass
	};

	renderStages.emplace_back(std::make_unique<RenderStage>(renderpassAttachment1, renderpassSubpasses1));
	GraphicManager::Get()->SetRenderStages(std::move(renderStages));

	auto &rendererContainer = GetRendererContainer();
	rendererContainer.Clear();

	rendererContainer.Add<RendererDirectionalShadow>(Pipeline::Stage(0, 0));

	rendererContainer.Add<RendererTerrain>(Pipeline::Stage(1, 0));
	rendererContainer.Add<RendererMeshes>(Pipeline::Stage(1, 0));
	rendererContainer.Add<RendererMeshesPBR>(Pipeline::Stage(1, 0));

	rendererContainer.Add<FilterSsao>(Pipeline::Stage(1, 1));
	rendererContainer.Add<FilterSsaoBlur>(Pipeline::Stage(1, 1));

	rendererContainer.Add<RendererDeferred>(Pipeline::Stage(1, 2));

	rendererContainer.Add<RendererForward>(Pipeline::Stage(1, 3));

	//rendererContainer.Add<FilterFog>(Pipeline::Stage(1, 4));
	rendererContainer.Add<FilterFxaa>(Pipeline::Stage(1, 4));
	rendererContainer.Add<FilterDefault>(Pipeline::Stage(1, 4), true);
	rendererContainer.Add<RendererGizmo>(Pipeline::Stage(1, 4));
	rendererContainer.Add<RendererImGui>(Pipeline::Stage(1, 4)); //Must be the last one otherwise draw inside an imgui window
}
void EditorRenderManager::Update()
{
	m_RendererContainer.Update();
}
}
