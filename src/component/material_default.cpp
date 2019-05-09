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

#include <component/material_default.h>

namespace dm
{
MaterialDefaultManager::MaterialDefaultManager() { }

MaterialDefaultManager::~MaterialDefaultManager()
{
	m_Components.clear();
}

void MaterialDefaultManager::Awake() {}
void MaterialDefaultManager::Start() {}

void MaterialDefaultManager::Update() {}

MaterialDefault* MaterialDefaultManager::CreateComponent(const Entity entity)
{
	auto t = MaterialDefault();
	t.componentType = ComponentType::MATERIAL_DEFAULT;

	t.color = Color(25, 25, 25, 1);

	t.textureDiffuse = nullptr;

	m_Components[entity - 1] = std::move(t);

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
	defines.emplace_back("DIFFUSE_MAPPING", To<int32_t>(false));
	defines.emplace_back("MATERIAL_MAPPING", To<int32_t>(false));
	defines.emplace_back("NORMAL_MAPPING", To<int32_t>(false));
	defines.emplace_back("ANIMATED", To<int32_t>(false));
	defines.emplace_back("MAX_JOINTS", To(false));
	defines.emplace_back("MAX_WEIGHTS", To(false));
	return defines;
}

void MaterialDefaultManager::PushDescriptor(MaterialDefault& material)
{
	material.descriptorSet.Push("samplerDiffuse", material.textureDiffuse);
}

void MaterialDefaultManager::PushUniform(MaterialDefault& material, const glm::mat4x4 worldPos)
{
	material.uniformObject.Push("transform", worldPos);
	material.uniformObject.Push("baseDiffuse", material.color);
	material.uniformObject.Push("metallic", static_cast<float>(material.metallic));
	material.uniformObject.Push("roughness", static_cast<float>(material.roughness));
	material.uniformObject.Push("ignoreFog", material.ignoreFog);
	material.uniformObject.Push("ignoreLighting", material.ignoreLighting);
}

MaterialDefault& MaterialDefaultManager::Get(const Entity entity)
{
	return m_Components[entity - 1];
}

void MaterialDefaultManager::OnDrawInspector(Entity entity) {}
}
