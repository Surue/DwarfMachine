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

#include <component/model.h>
#include "engine/engine.h"
#include <graphics/mesh_manager.h>

namespace dm
{
ModelComponentManager::ModelComponentManager() { }

ModelComponentManager::~ModelComponentManager()
{
	m_Components.clear();
}

void ModelComponentManager::Init() {}

void ModelComponentManager::Update() {}

Model* ModelComponentManager::CreateComponent(const Entity entity)
{
	auto mesh = Model();
	mesh.componentType = ComponentType::MODEL;

	mesh.model = nullptr;

	m_Components[entity - 1] = mesh;

	return &m_Components[entity - 1];
}
void ModelComponentManager::DestroyComponent(Entity entity) {}
void ModelComponentManager::OnDrawInspector(Entity entity) {}

void ModelComponentManager::DecodeComponent(json& componentJson, const Entity entity)
{
	auto mesh = Model();

	if (CheckJsonExists(componentJson, "modelName"))
	{
		mesh.model = Engine::Get()->GetModelManager()->GetModel(componentJson["modelName"]);
	}else
	{
		mesh.model = nullptr;
	}

	m_Components[entity - 1] = mesh;
}

void ModelComponentManager::EncodeComponent(json& componentJson, const Entity entity)
{
	componentJson["type"] = ComponentType::MODEL;

	componentJson["modelName"] = Engine::Get()->GetModelManager()->GetModelName(m_Components[entity - 1].model);
}
}
