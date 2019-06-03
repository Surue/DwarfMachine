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
			{ "../Shaders/terrain.vert", "../Shaders/terrain.tesc", "../Shaders/terrain.tese", "../Shaders/terrain.frag" },
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
			{ "../Shaders/terrain.vert", "../Shaders/terrain.tesc", "../Shaders/terrain.tese", "../Shaders/terrain.frag" },
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
void MaterialTerrainManager::PushDescriptor(MaterialTerrain& material, DescriptorHandle& descriptorSet)
{
	descriptorSet.Push("samplerNoise", material.noiseMap);
}

void ComputeFrustumPlanes(glm::mat4 &viewMatrix, glm::mat4 &projMatrix, glm::vec4 *plane)
{
	glm::mat4 viewProj = projMatrix * viewMatrix;
	plane[0] = viewProj[3] + viewProj[0];   // left
	plane[1] = viewProj[3] - viewProj[0];   // right
	plane[2] = viewProj[3] + viewProj[1];   // bottom
	plane[3] = viewProj[3] - viewProj[1];   // top
	plane[4] = viewProj[3] + viewProj[2];   // far
	plane[5] = viewProj[3] - viewProj[2];   // near

	// normalize planes
	for (int i = 0; i < 6; i++) {
		float l = glm::length(glm::vec3(plane[i]));
		plane[i] = plane[i] / l;
	}
}

void MaterialTerrainManager::PushUniform(MaterialTerrain& material, const glm::mat4x4 worldPos,
	UniformHandle& uniformObject)
{
	uniformObject.Push("innerTessFactor", 64.0f);
	uniformObject.Push("outerTessFactor", 64.0f);
	uniformObject.Push("noiseFreq", 2.0f);
	uniformObject.Push("noiseOctave", 10.0f);
	uniformObject.Push("invNoiseSize", 1 / material.noiseMap->GetHeight());
	float heightScale = 0.1f;
	uniformObject.Push("heightScale", heightScale);
	uniformObject.Push("triSize", 1.0f);

	glm::vec2 size = material.pipelineMaterial->GetPipeline()->GetSize();
	uniformObject.Push("viewport", glm::vec4(0, 0, size.x, size.y)); 

	auto camera = GraphicManager::Get()->GetCamera();
	uniformObject.Push("ModelView", camera->viewMatrix); 
	uniformObject.Push("ModelViewProjection", camera->projectionMatrix * camera->viewMatrix); 
	uniformObject.Push("Projection", camera->projectionMatrix); 
	uniformObject.Push("InvProjection", glm::inverse(camera->projectionMatrix)); 
	uniformObject.Push("InvView", glm::inverse(camera->viewMatrix));
	uniformObject.Push("eyePosWorld", glm::inverse(camera->viewMatrix) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	glm::vec4 frustumPlanes[6];
	ComputeFrustumPlanes(camera->viewMatrix, camera->projectionMatrix, frustumPlanes);
	uniformObject.Push("frustumPlanes", frustumPlanes);

	int gridW, gridH;
	gridW = gridH = 64;
	uniformObject.Push("gridW", gridW);
	uniformObject.Push("gridH", gridH);
	glm::vec3 tileSize = glm::vec3(0.25f, 0.0f, 0.25f);
	uniformObject.Push("tileSize", tileSize);
	uniformObject.Push("gridOrigin", glm::vec3(-tileSize.x * gridW * 0.5f, 0.0f, -tileSize.z * gridH * 0.5f));

	glm::vec3 halfTileSize = glm::vec3(tileSize.x, heightScale, tileSize.z)*0.5f;
	uniformObject.Push("tileBoundingSphereR", glm::length(halfTileSize));
}

void MaterialTerrainManager::DecodeComponent(json& componentJson, const Entity entity)
{
	auto material = MaterialTerrain();
	material.componentType = ComponentType::MATERIAL_DEFAULT;

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/terrain.vert", "../Shaders/terrain.tesc", "../Shaders/terrain.tese", "../Shaders/terrain.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT)
	);

	m_Components[entity - 1] = material;
}
void MaterialTerrainManager::EncodeComponent(json& componentJson, const Entity entity) {}
}
