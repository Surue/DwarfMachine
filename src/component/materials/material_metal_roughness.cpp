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

#include <component/materials/material_metal_roughness.h>
#include "imgui.h"
#include "component/model.h"
#include "graphics/texture_manager.h"

namespace dm
{
	MaterialMetalRoughnessManager::MaterialMetalRoughnessManager() { }

	MaterialMetalRoughnessManager::~MaterialMetalRoughnessManager()
	{
		m_Components.clear();
	}

	void MaterialMetalRoughnessManager::Init() {}

	void MaterialMetalRoughnessManager::Update() {}

	MaterialMetalRoughness* MaterialMetalRoughnessManager::CreateComponent(const Entity entity)
	{
		auto material = MaterialMetalRoughness();
		material.componentType = ComponentType::MATERIAL_DEFAULT;

		material.color = Color(25, 25, 25, 1);

		material.diffuseTexture = nullptr;

		material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 },
			PipelineGraphicsCreate(
				{ "../Shaders/default_metal_roughness.vert", "../Shaders/default_metal_roughness.frag" },
				{ ModelComponentManager::GetVertexInput() },
				MaterialMetalRoughnessManager::GetDefines(material),
				PipelineGraphics::Mode::MRT)
		);

		m_Components[entity - 1] = std::move(material);


		return &m_Components[entity - 1];
	}

	void MaterialMetalRoughnessManager::DestroyComponent(Entity entity) {}

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

	std::vector<Shader::Define> MaterialMetalRoughnessManager::GetDefines(const MaterialMetalRoughness& component)
	{
		std::vector<Shader::Define> defines;
		defines.emplace_back("DIFFUSE_MAPPING", To<int32_t>(component.diffuseTexture != nullptr));
		defines.emplace_back("METAL_MAPPING", To<int32_t>(component.metalTexture != nullptr));
		defines.emplace_back("ROUGHNESS_MAPPING", To<int32_t>(component.roughnessTexture != nullptr));
		defines.emplace_back("NORMAL_MAPPING", To<int32_t>(component.normalTexture != nullptr));
		return defines;
	}

	void MaterialMetalRoughnessManager::PushDescriptor(MaterialMetalRoughness& material, DescriptorHandle &descriptorSet)
	{
		descriptorSet.Push("samplerDiffuse", material.diffuseTexture);
		descriptorSet.Push("samplerMetal", material.metalTexture);
		descriptorSet.Push("samplerRoughness", material.roughnessTexture);
		descriptorSet.Push("samplerNormal", material.normalTexture);
	}

	void MaterialMetalRoughnessManager::PushUniform(MaterialMetalRoughness& material, const glm::mat4x4 worldPos, UniformHandle& uniformObject)
	{
		uniformObject.Push("transform", worldPos);
		uniformObject.Push("baseDiffuse", material.color * 255);
		uniformObject.Push("metallic", static_cast<float>(material.metallic));
		uniformObject.Push("roughness", static_cast<float>(material.roughness));
		uniformObject.Push("ignoreFog", material.ignoreFog);
		uniformObject.Push("ignoreLighting", material.ignoreLighting);
	}

	MaterialMetalRoughness& MaterialMetalRoughnessManager::Get(const Entity entity)
	{
		return m_Components[entity - 1];
	}

	void MaterialMetalRoughnessManager::OnDrawInspector(const Entity entity)
	{
		ImGui::Separator();
		ImGui::TextWrapped("Material metal roughness");
		ImGui::ColorPicker4("diffuseColor", &m_Components[entity - 1].color[0]);
		ImGui::DragFloat("Roughness", &m_Components[entity - 1].roughness, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Metallic", &m_Components[entity - 1].metallic, 0.01f, 0.0f, 1.0f);

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

	MaterialMetalRoughness* MaterialMetalRoughnessManager::AddComponent(const Entity entity, MaterialMetalRoughness& component)
	{
		component.pipelineMaterial = PipelineMaterial::Create({ 1, 0 }, //TODO trouver une manière de setter ça de manière automatique
			PipelineGraphicsCreate(
				{ "../Shaders/default_metal_roughness.vert", "../Shaders/default_metal_roughness.frag" },
				{ ModelComponentManager::GetVertexInput() },
				MaterialMetalRoughnessManager::GetDefines(component),
				PipelineGraphics::Mode::MRT)
		);
		m_Components[entity - 1] = component;
		return &m_Components[entity - 1];
	}

	void MaterialMetalRoughnessManager::DecodeComponent(json& componentJson, const Entity entity)
	{
		auto material = MaterialMetalRoughness();
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

		if (CheckJsonExists(componentJson, "texture"))
			material.diffuseTexture = TextureManager::Get()->GetTextureByName(componentJson["texture"]);

		if (CheckJsonExists(componentJson, "normal"))
			material.normalTexture = TextureManager::Get()->GetTextureByName(componentJson["normal"]);

		if (CheckJsonExists(componentJson, "metal"))
			material.metalTexture = TextureManager::Get()->GetTextureByName(componentJson["metal"]);

		if (CheckJsonExists(componentJson, "roughness"))
			material.roughnessTexture = TextureManager::Get()->GetTextureByName(componentJson["roughness"]);

		material.pipelineMaterial = PipelineMaterial::Create({ 1, 0 },
			PipelineGraphicsCreate(
				{ "../Shaders/default_metal_roughness.vert", "../Shaders/default_metal_roughness.frag" },
				{ ModelComponentManager::GetVertexInput() },
				MaterialMetalRoughnessManager::GetDefines(material),
				PipelineGraphics::Mode::MRT)
		);

		m_Components[entity - 1] = std::move(material);
	}

	void MaterialMetalRoughnessManager::EncodeComponent(json& componentJson, const Entity entity)
	{
		componentJson["type"] = ComponentType::MATERIAL_DEFAULT;

		SetColorToJson(componentJson, "color", m_Components[entity - 1].color);

		componentJson["metallic"] = m_Components[entity - 1].metallic;

		componentJson["roughness"] = m_Components[entity - 1].roughness;

		SetBoolToJson(componentJson, "castShadow", m_Components[entity - 1].castsShadows);
		SetBoolToJson(componentJson, "ignoreLighting", m_Components[entity - 1].ignoreLighting);
		SetBoolToJson(componentJson, "ignoreFog", m_Components[entity - 1].ignoreFog);

		if (m_Components[entity - 1].diffuseTexture != nullptr)
			componentJson["texture"] = m_Components[entity - 1].diffuseTexture.get()->GetFilename();

		if (m_Components[entity - 1].normalTexture != nullptr)
			componentJson["normal"] = m_Components[entity - 1].normalTexture.get()->GetFilename();

		if (m_Components[entity - 1].metalTexture != nullptr)
			componentJson["metal"] = m_Components[entity - 1].metalTexture.get()->GetFilename();

		if (m_Components[entity - 1].roughnessTexture != nullptr)
			componentJson["roughness"] = m_Components[entity - 1].roughnessTexture.get()->GetFilename();
	}
}
