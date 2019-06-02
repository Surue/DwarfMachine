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

#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include <component/component.h>
#include <graphics/descriptor_handle.h>

namespace dm
{
struct MeshRenderer: public ComponentBase
{
	enum class MaterialType
	{
		DEFAULT,
		SKYBOX
	};

	MaterialType materialType = MaterialType::DEFAULT;

	DescriptorHandle descriptorSet;
	UniformHandle uniformObject;

	bool castsShadows = true;
};

class MeshRendererManager : public ComponentBaseManager<MeshRenderer>
{
public:
	void Init() override;

	void Update() override;

	MeshRenderer* CreateComponent(Entity entity) override;

	void DestroyComponent(Entity entity) override;

	void OnDrawInspector(Entity entity) override;

	void CreateComponent(json& componentJson, const Entity entity) override;
};
}

#endif
