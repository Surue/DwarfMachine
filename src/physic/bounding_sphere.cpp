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


#include <physic/bounding_sphere.h>
#include "imgui.h"
#include <graphics/Mesh.h>

namespace dm
{
BoundingSphere BoundingSphereManager::GetBoundingSphere(Mesh& mesh)
{
	BoundingSphere boundingSphere;
	boundingSphere.componentType = ComponentType::BOUNDING_SPHERE;
	boundingSphere.m_Radius = mesh.GetRadius();
	return boundingSphere;
}

BoundingSphereManager::BoundingSphereManager()
{
	
}

BoundingSphereManager::~BoundingSphereManager() {}

void BoundingSphereManager::Init()
{
}

void BoundingSphereManager::Update()
{
}

BoundingSphere* BoundingSphereManager::CreateComponent(const Entity entity)
{
	auto b = BoundingSphere();
	b.m_Radius = 1;
	m_Components[entity - 1] = b;

	return &m_Components[entity - 1];
}

BoundingSphere* BoundingSphereManager::AddComponent(const Entity entity, BoundingSphere& component)
{
	m_Components[entity - 1] = component;

	return &m_Components[entity - 1];
}

void BoundingSphereManager::DestroyComponent(Entity entity)
{
}

void BoundingSphereManager::OnDrawInspector(Entity entity)
{
	ImGui::Separator();
	ImGui::TextWrapped("Bounding Sphere");
	ImGui::TextWrapped("Radius : %f ", m_Components[entity - 1].m_Radius);
}
}
