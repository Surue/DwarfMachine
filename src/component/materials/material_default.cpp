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

#include <component/materials/material_default.h>
#include "imgui.h"
#include "component/model.h"

namespace dm
{
MaterialDefaultManager::MaterialDefaultManager() { }

MaterialDefaultManager::~MaterialDefaultManager()
{
	m_Components.clear();
}

void MaterialDefaultManager::Init() {}

void MaterialDefaultManager::Update() {}

MaterialDefault* MaterialDefaultManager::CreateComponent(const Entity entity)
{
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;

	material.color = Color(25, 25, 25, 1);

	material.diffuseTexture = nullptr;

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, 
		PipelineGraphicsCreate(
			{ "../Shaders/shader.vert", "../Shaders/shader.frag" }, 
			{ ModelComponentManager::GetVertexInput() }, 
			MaterialDefaultManager::GetDefines(material), 
			PipelineGraphics::Mode::MRT)
	);

	m_Components[entity - 1] = std::move(material);


	return &m_Components[entity - 1];
}

void MaterialDefaultManager::DestroyComponent(Entity entity) {}

template<typename T>
static std::string To(const T &val)
{
	if constexpr (std::is_enum_v<T>)
	{
		typedef typename std::underlying_type<T>::type safe_type;
		return std::to_string(static_cast<safe_type>(val));
	}
	else if constexpr (std::is_same_v<bool, T>)
	{
		return val ? "true" : "false";
	}
	else if constexpr (std::is_same_v<std::string, T>)
	{
		return val;
	}
	else
	{
		return std::to_string(static_cast<T>(val));
	}
}

std::vector<Shader::Define> MaterialDefaultManager::GetDefines(const MaterialDefault& component)
{
	std::vector<Shader::Define> defines;
	//TODO Changer pour mettre correctement les valeur poru les defines
	defines.emplace_back("DIFFUSE_MAPPING", To<int32_t>(component.diffuseTexture != nullptr));
	defines.emplace_back("MATERIAL_MAPPING", To<int32_t>(component.materialTexture != nullptr));
	defines.emplace_back("NORMAL_MAPPING", To<int32_t>(component.normalTexture != nullptr));
	return defines;
}

void MaterialDefaultManager::PushDescriptor(MaterialDefault& material, DescriptorHandle &descriptorSet)
{
	descriptorSet.Push("samplerDiffuse", material.diffuseTexture);
	descriptorSet.Push("samplerMaterial", material.materialTexture);
	descriptorSet.Push("samplerNormal", material.normalTexture);
}

void MaterialDefaultManager::PushUniform(MaterialDefault& material, const glm::mat4x4 worldPos, UniformHandle& uniformObject)
{
	uniformObject.Push("transform", worldPos);
	uniformObject.Push("baseDiffuse", material.color * 255);
	uniformObject.Push("metallic", static_cast<float>(material.metallic));
	uniformObject.Push("roughness", static_cast<float>(material.roughness));
	uniformObject.Push("ignoreFog", material.ignoreFog);
	uniformObject.Push("ignoreLighting", material.ignoreLighting);
}

MaterialDefault& MaterialDefaultManager::Get(const Entity entity)
{
	return m_Components[entity - 1];
}

void MaterialDefaultManager::OnDrawInspector(const Entity entity)
{
	ImGui::Separator();
	ImGui::TextWrapped("Default Material");
	ImGui::ColorPicker4("diffuseColor", &m_Components[entity - 1].color[0]);
	ImGui::DragFloat("Roughness", &m_Components[entity - 1].roughness, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Metallic", &m_Components[entity - 1].metallic, 0.01f, 0.0f, 1.0f);
	//ImGui::Text(m_Components[entity - 1].pipelineMaterial->GetPipeline()->GetShader()->ToString().c_str());
}

MaterialDefault* MaterialDefaultManager::AddComponent(const Entity entity, MaterialDefault& component)
{
	component.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
		PipelineGraphicsCreate(
			{ "../Shaders/shader.vert", "../Shaders/shader.frag" },
			{ ModelComponentManager::GetVertexInput() },
			MaterialDefaultManager::GetDefines(component),
			PipelineGraphics::Mode::MRT)
	);
	m_Components[entity - 1] = component;
	return &m_Components[entity - 1];
}

void MaterialDefaultManager::DecodeComponent(json& componentJson, const Entity entity)
{
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;

	if (CheckJsonExists(componentJson, "color"))
		material.color = GetColorFromJson(componentJson, "color");

	if (CheckJsonExists(componentJson, "metallic") && CheckJsonNumber(componentJson, "metallic"))
		material.metallic = componentJson["metallic"];

	if (CheckJsonExists(componentJson, "roughness") && CheckJsonNumber(componentJson, "roughness"))
		material.roughness = componentJson["roughness"];

	if (CheckJsonExists(componentJson, "castShadow"))
		material.castsShadows = GetBoolFromJson(componentJson, "castShadow");

	if (CheckJsonExists(componentJson, "ignoreLighting"))
		material.ignoreLighting = GetBoolFromJson(componentJson, "ignoreLighting");

	if (CheckJsonExists(componentJson, "ignoreFog"))
		material.ignoreFog = GetBoolFromJson(componentJson, "ignoreFog");

	if(CheckJsonExists(componentJson, "texture"))
		material.diffuseTexture = Image2d::Create(componentJson["texture"]);

	if (CheckJsonExists(componentJson, "normal"))
		material.normalTexture = Image2d::Create(componentJson["normal"]);

	if (CheckJsonExists(componentJson, "material"))
		material.materialTexture = Image2d::Create(componentJson["material"]);

	material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 },
		PipelineGraphicsCreate(
			{ "../Shaders/shader.vert", "../Shaders/shader.frag" },
			{ ModelComponentManager::GetVertexInput() },
			MaterialDefaultManager::GetDefines(material),
			PipelineGraphics::Mode::MRT)
	);

	m_Components[entity - 1] = std::move(material);
}

void MaterialDefaultManager::EncodeComponent(json& componentJson, const Entity entity)
{
	componentJson["type"] = ComponentType::MATERIAL_DEFAULT;

	SetColorToJson(componentJson, "color", m_Components[entity - 1].color);

	componentJson["metallic"] = m_Components[entity - 1].metallic;

	componentJson["roughness"] = m_Components[entity - 1].roughness;

	SetBoolToJson(componentJson, "castShadow", m_Components[entity - 1].castsShadows);
	SetBoolToJson(componentJson, "ignoreLighting", m_Components[entity - 1].ignoreLighting);
	SetBoolToJson(componentJson, "ignoreFog", m_Components[entity - 1].ignoreFog);

	if(m_Components[entity - 1].diffuseTexture != nullptr)
	componentJson["texture"] = m_Components[entity - 1].diffuseTexture.get()->GetFilename();

	if (m_Components[entity - 1].normalTexture != nullptr)
	componentJson["normal"] = m_Components[entity - 1].normalTexture.get()->GetFilename();

	if (m_Components[entity - 1].materialTexture != nullptr)
	componentJson["material"] = m_Components[entity - 1].materialTexture.get()->GetFilename();
}
}
