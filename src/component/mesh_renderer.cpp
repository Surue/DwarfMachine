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

#include <component/mesh_renderer.h>

namespace dm
{
void MeshRendererManager::Init()
{
}

void MeshRendererManager::Update()
{
}

MeshRenderer* MeshRendererManager::CreateComponent(const Entity entity)
{
	MeshRenderer meshRenderer;
	m_Components[entity - 1] = std::move(meshRenderer);
	return &m_Components[entity - 1];
}

void MeshRendererManager::DestroyComponent(Entity entity)
{
}

void MeshRendererManager::OnDrawInspector(Entity entity)
{
}

void MeshRendererManager::DecodeComponent(json& componentJson, const Entity entity)
{
	MeshRenderer meshRenderer;

	if(CheckJsonExists(componentJson, "materialType") && CheckJsonNumber(componentJson, "materialType"))
		meshRenderer.materialType = static_cast<MeshRenderer::MaterialType>(componentJson["materialType"]);

	if (CheckJsonExists(componentJson, "castShadow"))
		meshRenderer.castsShadows = GetBoolFromJson(componentJson, "castShadow");

	m_Components[entity - 1] = std::move(meshRenderer);
}

void MeshRendererManager::EncodeComponent(json& componentJson, const Entity entity)
{
	componentJson["type"] = ComponentType::MESH_RENDERER;

	componentJson["materialType"] = static_cast<MeshRenderer::MaterialType>(m_Components[entity - 1].materialType);

	SetBoolToJson(componentJson, "castShadow", m_Components[entity - 1].castsShadows);
}
}
