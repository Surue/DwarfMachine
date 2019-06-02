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

#include <component/materials/material_skybox.h>
#include "component/model.h"
#include "imgui.h"

namespace dm
{
void MaterialSkyboxManager::Init()
{
}

void MaterialSkyboxManager::Update()
{
}

MaterialSkybox* MaterialSkyboxManager::AddComponent(const Entity entity, MaterialSkybox& component)
{
	component.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/skybox.vert", "../Shaders/skybox.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT,
			PipelineGraphics::Depth::NONE,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_FRONT_BIT)
	);

	m_Components[entity - 1] = component;

	return &m_Components[entity - 1];
} 

MaterialSkybox* MaterialSkyboxManager::CreateComponent(const Entity entity)
{
	auto material = MaterialSkybox();
	material.componentType = ComponentType::MATERIAL_SKYBOX;

	material.color = Color(25, 25, 25, 1);

	material.image = nullptr;
	material.blend = 1.0f;
	material.fogLimit = glm::vec2(-1.0f, 1.0f);

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/skybox.vert", "../Shaders/skybox.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT, 
			PipelineGraphics::Depth::NONE, 
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			VK_POLYGON_MODE_FILL, 
			VK_CULL_MODE_FRONT_BIT)
	);

	m_Components[entity - 1] = std::move(material);

	return &m_Components[entity - 1];
}

void MaterialSkyboxManager::DestroyComponent(Entity entity)
{
}

void MaterialSkyboxManager::OnDrawInspector(Entity entity)
{
	ImGui::Separator();
	ImGui::TextWrapped("Skybox Material");
	//ImGui::ColorPicker4("diffuseColor", &m_Components[entity - 1].color[0]);
	/*ImGui::Text(m_Components[entity - 1].pipelineMaterial->GetPipeline()->GetShader()->ToString().c_str());*/
}

void MaterialSkyboxManager::PushDescriptor(MaterialSkybox& material, DescriptorHandle &descriptorSet)
{
	descriptorSet.Push("samplerColor", material.image);
}

void MaterialSkyboxManager::PushUniform(MaterialSkybox& material, const glm::mat4x4 worldPos, UniformHandle& uniformObject)
{
	uniformObject.Push("transform", worldPos);
	uniformObject.Push("baseColor", material.color);
	uniformObject.Push("fogColor", material.fogColor);
	uniformObject.Push("fogLimits", glm::vec2(material.fogLimit.x * 50, material.fogLimit.y * 50)); //TODO prendre en compte le scale du transform
	uniformObject.Push("blendFactor", material.blend);
}

void MaterialSkyboxManager::DecodeComponent(json& componentJson, const Entity entity)
{
	auto material = MaterialSkybox();
	material.componentType = ComponentType::MATERIAL_SKYBOX;

	if (CheckJsonExists(componentJson, "color"))
		material.color = GetColorFromJson(componentJson, "color");

	if (CheckJsonExists(componentJson, "fogColor"))
		material.fogColor = GetColorFromJson(componentJson, "fogColor");

	if (CheckJsonExists(componentJson, "blend"))
		material.blend = componentJson["blend"];

	if (CheckJsonExists(componentJson, "image") && CheckJsonExists(componentJson, "extension"))
		material.image = ImageCube::Create(componentJson["image"], componentJson["extension"], VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, true, true);

	if (CheckJsonExists(componentJson, "fogLimit") )
		material.fogLimit = GetVector2FromJson(componentJson, "fogLimit");

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/skybox.vert", "../Shaders/skybox.frag" },
			{ ModelComponentManager::GetVertexInput() },
			{},
			PipelineGraphics::Mode::MRT,
			PipelineGraphics::Depth::NONE,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_FRONT_BIT)
	);

	m_Components[entity - 1] = std::move(material);
}

void MaterialSkyboxManager::EncodeComponent(json& componentJson, const Entity entity)
{
	componentJson["type"] = ComponentType::MATERIAL_SKYBOX;

	SetColorToJson(componentJson, "color", m_Components[entity - 1].color);
	SetColorToJson(componentJson, "fogColor", m_Components[entity - 1].fogColor);

	componentJson["blend"] = m_Components[entity - 1].blend;

	SetVector2ToJson(componentJson, "fogLimit", m_Components[entity - 1].fogLimit);

	componentJson["image"] = m_Components[entity - 1].image.get()->GetFilename();
	componentJson["extension"] = m_Components[entity - 1].image.get()->GetFileSuffix();
}
}
