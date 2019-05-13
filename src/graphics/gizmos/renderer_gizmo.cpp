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

#include <graphics/Gizmos/renderer_gizmo.h>
#include <graphics/graphic_manager.h>
#include <graphics/Gizmos/gizmo_type.h>

namespace dm
{
RendererGizmo::RendererGizmo(const Pipeline::Stage& stage): 
	RenderPipeline(stage),
	m_Pipeline(stage, {"Shaders/gizmo.vert", "Shaders/gizmo.frag"}, {VertexMesh::GetVertexInput(0), GizmoType::Instance::GetVertexInput(1)}, {}, PipelineGraphics::Mode::POLYGON, PipelineGraphics::Depth::READ_WRITE, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE)
{

}

void RendererGizmo::Update()
{
}

void RendererGizmo::Draw(const CommandBuffer& commandBuffer)
{
	auto camera = GraphicManager::Get()->GetCamera();
	m_UniformScene.Push("projection", camera->proj);
	m_UniformScene.Push("view", camera->viewMatrix);

	auto &gizmos = Gizmos::Get()->GetGizmos();

	m_Pipeline.BindPipeline(commandBuffer);

	for(const auto &[type, TypeGizmos] : gizmos)
	{
		type->CmdRender(commandBuffer, m_Pipeline, m_UniformScene);
	}
}
}
