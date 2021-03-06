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

#ifndef MESH_H
#define MESH_H
#include <component/component.h>
#include "graphics/Mesh.h"
#include <graphics/pipelines/shader.h>
#include "graphics/mesh_vertex.h"

namespace dm
{
struct Model final : ComponentBase
{
	Mesh* model = nullptr;
};

class ModelComponentManager : public ComponentBaseManager<Model>
{
public:
	explicit ModelComponentManager();

	~ModelComponentManager();

	void Init() override;

	void Update() override;

	Model* CreateComponent(Entity entity) override;

	void DestroyComponent(Entity entity) override;

	static Shader::VertexInput GetVertexInput(const uint32_t &binding = 0) { return VertexMesh::GetVertexInput(binding); }
	
	void OnDrawInspector(Entity entity) override;

	void DecodeComponent(json& componentJson, const Entity entity) override;

	void EncodeComponent(json& componentJson, const Entity entity) override;
};
}

#endif MESH_H