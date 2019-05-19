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

#ifndef MATERIAL_SKYBOX_H
#define MATERIAL_SKYBOX_H

#include <component/materials/material.h>
#include <graphics/image_cube.h>
#include <engine/color.h>
#include <glm/vec2.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <graphics/buffers/uniform_handle.h>
#include<graphics/descriptor_handle.h>

namespace dm
{
struct MaterialSkybox : public Material
{
	std::shared_ptr<ImageCube> image;
	Color color;
	float blend = 1;
	Color fogColor = Color(0.1f, 0.1f, 0.1f, 1.0f);
	glm::vec2 fogLimit = glm::vec2(-10000.0f, 10000.0f);
};

class MaterialSkyboxManager final : public ComponentBaseManager<MaterialSkybox>
{
public:
	void Init() override;
	void Update() override;
	MaterialSkybox* CreateComponent(Entity entity) override;
	void DestroyComponent(Entity entity) override;
	void OnDrawInspector(Entity entity) override;

	static void PushDescriptor(MaterialSkybox& material, DescriptorHandle &descriptorSet);

	static void PushUniform(MaterialSkybox& material, const glm::mat4x4 worldPos, UniformHandle& uniformObject);
private:
};
}

#endif
