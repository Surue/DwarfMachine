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

#include <component/materials/material_terrain.h>
#include "imgui_impl_vulkan.h"
#include "component/model.h"

namespace dm
{
void MaterialTerrainManager::Init() {}

void MaterialTerrainManager::Update() {}

MaterialTerrain* MaterialTerrainManager::AddComponent(const Entity entity, MaterialTerrain& component)
{
	component.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/shader.vert", "../Shaders/shader.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT)
	);

	m_Components[entity - 1] = component;
	return &m_Components[entity - 1];
}
MaterialTerrain* MaterialTerrainManager::CreateComponent(const Entity entity)
{
	auto material = MaterialTerrain();
	material.componentType = ComponentType::MATERIAL_DEFAULT;

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 },
		PipelineGraphicsCreate(
			{ "../Shaders/shader.vert", "../Shaders/shader.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT)
	);

	m_Components[entity - 1] = std::move(material);


	return &m_Components[entity - 1];
}

void MaterialTerrainManager::DestroyComponent(Entity entity) {}

void MaterialTerrainManager::OnDrawInspector(Entity entity)
{
	ImGui::Separator();
	ImGui::TextWrapped("Terrain Material");

	static bool showCode = true;
	if (showCode) {
		if (ImGui::ArrowButton("Hide shader code", ImGuiDir_Up))
		{
			showCode = !showCode;
		}

		ImGui::Text(m_Components[entity - 1].pipelineMaterial->GetPipeline()->GetShader()->ToString().c_str());
	}
	else
	{
		if (ImGui::ArrowButton("Show shader code", ImGuiDir_Down))
		{
			showCode = !showCode;
		}
	}
}
void MaterialTerrainManager::PushDescriptor(MaterialTerrain& material, DescriptorHandle& descriptorSet) {}

void MaterialTerrainManager::PushUniform(MaterialTerrain& material, const glm::mat4x4 worldPos,
	UniformHandle& uniformObject) {}

void MaterialTerrainManager::DecodeComponent(json& componentJson, const Entity entity)
{
	auto material = MaterialTerrain();
	material.componentType = ComponentType::MATERIAL_DEFAULT;

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/shader.vert", "../Shaders/shader.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT)
	);

	m_Components[entity - 1] = material;
}
void MaterialTerrainManager::EncodeComponent(json& componentJson, const Entity entity) {}
}
