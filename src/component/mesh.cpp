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

#include <component/mesh.h>
#include "graphics/model_cube.h"

namespace dm
{
MeshManager::MeshManager() { }

MeshManager::~MeshManager()
{
	m_Components.clear();
}

void MeshManager::Awake() {}

void MeshManager::Start() {}

void MeshManager::Update() {}

Mesh* MeshManager::CreateComponent(const Entity entity)
{
	auto mesh = Mesh();
	mesh.componentType = ComponentType::TRANSFORM;

	mesh.model = nullptr;

	m_Components[entity - 1] = mesh;

	return &m_Components[entity - 1];
}
void MeshManager::DestroyComponent(Entity entity) {}
void MeshManager::OnDrawInspector(Entity entity) {}
}
