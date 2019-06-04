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
#include <graphics/graphic_manager.h>

namespace dm
{
void MaterialTerrainManager::Init() {}

void MaterialTerrainManager::Update() {}

MaterialTerrain* MaterialTerrainManager::AddComponent(const Entity entity, MaterialTerrain& component)
{
	component.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/terrain.vert", "../Shaders/terrain.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT)
	);

	m_Components[entity - 1] = std::move(component);
	return &m_Components[entity - 1];
}
MaterialTerrain* MaterialTerrainManager::CreateComponent(const Entity entity)
{
	MaterialTerrain material;
	material.componentType = ComponentType::MATERIAL_TERRAIN;

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/terrain.vert", "../Shaders/terrain.frag" },
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
	}
	else
	{
		if (ImGui::ArrowButton("Show shader code", ImGuiDir_Down))
		{
			showCode = !showCode;
		}
	}

	if (showCode)
	{
		ImGui::Text(m_Components[entity - 1].pipelineMaterial->GetPipeline()->GetShader()->ToString().c_str());
	}
}
void MaterialTerrainManager::PushDescriptor(MaterialTerrain& material, DescriptorHandle& descriptorSet)
{
	descriptorSet.Push("samplerGrass", material.grassSampler);
	descriptorSet.Push("samplerHeight", material.noiseMap);
}

void ComputeFrustumPlanes(glm::mat4 &viewMatrix, glm::mat4 &projMatrix, std::vector<glm::vec4>& planes)
{
	glm::mat4 matrix = projMatrix * viewMatrix;
	planes[0].x = matrix[0].w + matrix[0].x;
	planes[0].y = matrix[1].w + matrix[1].x;
	planes[0].z = matrix[2].w + matrix[2].x;
	planes[0].w = matrix[3].w + matrix[3].x;

	planes[1].x = matrix[0].w - matrix[0].x;
	planes[1].y = matrix[1].w - matrix[1].x;
	planes[1].z = matrix[2].w - matrix[2].x;
	planes[1].w = matrix[3].w - matrix[3].x;

	planes[2].x = matrix[0].w - matrix[0].y;
	planes[2].y = matrix[1].w - matrix[1].y;
	planes[2].z = matrix[2].w - matrix[2].y;
	planes[2].w = matrix[3].w - matrix[3].y;

	planes[3].x = matrix[0].w + matrix[0].y;
	planes[3].y = matrix[1].w + matrix[1].y;
	planes[3].z = matrix[2].w + matrix[2].y;
	planes[3].w = matrix[3].w + matrix[3].y;

	planes[4].x = matrix[0].w + matrix[0].z;
	planes[4].y = matrix[1].w + matrix[1].z;
	planes[4].z = matrix[2].w + matrix[2].z;
	planes[4].w = matrix[3].w + matrix[3].z;

	planes[5].x = matrix[0].w - matrix[0].z;
	planes[5].y = matrix[1].w - matrix[1].z;
	planes[5].z = matrix[2].w - matrix[2].z;
	planes[5].w = matrix[3].w - matrix[3].z;

	for (auto i = 0; i < planes.size(); i++)
	{
		float length = sqrtf(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
		planes[i] /= length;
	}
}

void MaterialTerrainManager::PushUniform(MaterialTerrain& material, const glm::mat4x4 worldPos,
	UniformHandle& uniformObject)
{
	auto camera = GraphicManager::Get()->GetCamera();
	uniformObject.Push("view", camera->viewMatrix); 
	uniformObject.Push("projection", camera->projectionMatrix); 
	uniformObject.Push("transform", worldPos);
}

void MaterialTerrainManager::DecodeComponent(json& componentJson, const Entity entity)
{
	auto material = MaterialTerrain();
	material.componentType = ComponentType::MATERIAL_DEFAULT;

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/terrain.vert", "../Shaders/terrain.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT)
	);

	m_Components[entity - 1] = std::move(material);
}
void MaterialTerrainManager::EncodeComponent(json& componentJson, const Entity entity) {}
}
