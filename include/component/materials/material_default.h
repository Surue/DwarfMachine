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

#include <component/materials/material.h>
#include <engine/color.h>
#include <graphics/buffers/uniform_handle.h>
#include <graphics/descriptor_handle.h>
#include <glm/mat4x4.hpp>

namespace dm
{

struct MaterialDefault : public Material
{
	Color color = Color::White;
	std::shared_ptr<Image2d> textureDiffuse;

	float metallic = 0;
	float roughness = 0;
	std::shared_ptr<Image2d> materialTexture;
	std::shared_ptr<Image2d> normalTexture;

	bool castsShadows = false;
	bool ignoreLighting = false;
	bool ignoreFog = false;
};

class MaterialDefaultManager final : public ComponentBaseManager<MaterialDefault>
{
public:
	explicit MaterialDefaultManager();

	~MaterialDefaultManager();

	void Init() override;

	void Update() override;

	MaterialDefault* CreateComponent(Entity entity) override;

	void DestroyComponent(Entity entity) override;

	static std::vector<Shader::Define> GetDefines(const MaterialDefault &component);

	static void PushDescriptor(MaterialDefault& material, DescriptorHandle &descriptorSet);

	static void PushUniform(MaterialDefault& material, const glm::mat4x4 worldPos, UniformHandle& uniformObject);

	MaterialDefault& Get(const Entity entity);

	void OnDrawInspector(Entity entity) override;

	MaterialDefault* AddComponent(const Entity entity, MaterialDefault& component) override;
private:
};
}

#endif MATERIAL_DEFAULT_H