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

#ifndef MATERIAL_DEFAULT_H
#define MATERIAL_DEFAULT_H

#include <component/material.h>
#include <engine/color.h>
#include <graphics/uniform_handle.h>
#include <graphics/descriptor_handle.h>
#include <engine/matrix_4.h>

namespace dm
{

struct MaterialDefault : public Material
{
	Color color;
	std::shared_ptr<Image2d> textureDiffuse;

	float metallic;
	float roughness;
	std::shared_ptr<Image2d> materialTexture;
	std::shared_ptr<Image2d> normalTexture;

	bool castsShadows;
	bool ignoreLighting;
	bool ignoreFog;

	DescriptorHandle descriptorSet;
	UniformHandle uniformObject;
};

class MaterialDefaultManager final : public ComponentBaseManager<MaterialDefault>
{
public:
	explicit MaterialDefaultManager(Engine& engine);

	void Init() override;

	void Update() override;

	MaterialDefault* CreateComponent(Entity entity) override;

	void DestroyComponent(Entity entity) override;

	static std::vector<Shader::Define> GetDefines(const MaterialDefault &component);

	static void PushDescriptor(MaterialDefault &material)
	{
		material.descriptorSet.Push("samplerDiffuse", material.textureDiffuse);
	}

	static void PushUniform(MaterialDefault &material, const Matrix4 worldPos)
	{
		//if (m_animated)
		//{
		//	auto meshAnimated = GetParent()->GetComponent<MeshAnimated>();
		//	auto joints = meshAnimated->GetJointTransforms(); // TODO: Move into storage buffer and update every frame.
		//	uniformObject.Push("jointTransforms", *joints.data(), sizeof(Matrix4) * joints.size());
		//}

		material.uniformObject.Push("transform", worldPos);
		material.uniformObject.Push("baseDiffuse", material.textureDiffuse);
		material.uniformObject.Push("metallic", material.metallic);
		material.uniformObject.Push("roughness", material.roughness);
		material.uniformObject.Push("ignoreFog", static_cast<float>(material.ignoreFog));
		material.uniformObject.Push("ignoreLighting", static_cast<float>(material.ignoreLighting));
	}

	MaterialDefault& Get(const Entity entity)
	{
		return m_Components[entity - 1];
	}
private:
};
}

#endif MATERIAL_DEFAULT_H