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
		Attachment(0, "depth", Attachment::Type::DEPTH, false),
		Attachment(1, "swapchain", Attachment::Type::SWAPCHAIN, false, VK_FORMAT_R8G8B8A8_UNORM),
		Attachment(2, "position", Attachment::Type::IMAGE, false, VK_FORMAT_R16G16B16A16_SFLOAT),
		Attachment(3, "diffuse", Attachment::Type::IMAGE, false, VK_FORMAT_R8G8B8A8_UNORM),
		Attachment(4, "normal", Attachment::Type::IMAGE, false, VK_FORMAT_R16G16B16A16_SFLOAT),
		Attachment(5, "material", Attachment::Type::IMAGE, false, VK_FORMAT_R8G8B8A8_UNORM), 
		Attachment(6, "resolved", Attachment::Type::IMAGE, false, VK_FORMAT_R8G8B8A8_UNORM)
	};

	std::vector<SubpassType> renderpassSubpasses0 = { 
		SubpassType(0, { 0, 2, 3, 4, 5 }), 
		SubpassType(1, { 0, 6 }), 
		SubpassType(2, { 0, 1 }) 
	};

	renderStages.emplace_back(std::make_unique<RenderStage>(renderpassAttachment0, renderpassSubpasses0));

//	renderStages.emplace_back(std::make_unique<RenderStage>(renderpassAttachment1, renderpassSubpasses1));

	Engine::Get()->GetGraphicManager()->SetRenderStages(std::move(renderStages));

	auto &rendererContainer = GetRendererContainer();
	rendererContainer.Clear();

	//RendererMeshes rendererMeshes = RendererMeshes(*Engine::Get(), Pipeline::Stage(1, 0));
	rendererContainer.Add<RendererMeshes>(*Engine::Get(), Pipeline::Stage(0, 0));
}
void EditorRenderManager::Update()
{
	m_RendererContainer.Update();
}
}
