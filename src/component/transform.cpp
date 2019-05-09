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

#include <component/transform.h>
#include "imgui.h"
#include "imgui_gizmo.h"
#include <graphics/graphic_manager.h>

namespace dm
{

TransformManager::TransformManager()
{
	
}

void TransformManager::Awake() {}

void TransformManager::Start() {}

void TransformManager::Update() {}

Transform* TransformManager::CreateComponent(const Entity entity)
{
	auto t = Transform();
	t.componentType = ComponentType::TRANSFORM;

	t.position.x = 0;
	t.position.y = 0;
	t.position.z = 0;

	t.rotation.x = 0;
	t.rotation.y = 0;
	t.rotation.z = 0;

	t.scaling.x = 1;
	t.scaling.y = 1;
	t.scaling.z = 1;

	m_Components[entity - 1] = t;

	m_Components[entity - 1].worldMatrix = GetWorldMatrix(t);

	return &m_Components[entity - 1];
}

Transform* TransformManager::AddComponent(const Entity entity, Transform& component)
{
	m_Components[entity - 1] = component;

	m_Components[entity - 1].worldMatrix = GetWorldMatrix(component);

	std::cout << "ICI\n";

	return &m_Components[entity - 1];
}

void TransformManager::DestroyComponent(Entity entity) { }

Matrix4 TransformManager::GetWorldMatrix(Transform& component) 
{
	return Matrix4::TransformationMatrix(component.position, (3.14f / 180) * component.rotation, component.scaling);
}

void TransformManager::OnDrawInspector(Entity entity)
{
	m_Components[entity - 1].worldMatrix = GetWorldMatrix(m_Components[entity - 1]);

	ImGui::Separator();
	ImGui::TextWrapped("Transform");/*
	ImGui::DragFloat3("Position", &m_Components[entity - 1].position[0], 0.5f);
	ImGui::DragFloat3("Rotation", &m_Components[entity - 1].rotation[0], 0.5f);
	ImGui::DragFloat3("Scale", &m_Components[entity - 1].scaling[0], 0.5f);*/

	//ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &m_Components[entity - 1].position[0], &m_Components[entity - 1].rotation[0], &matrixScale[0]);
	ImGui::DragFloat3("Position", &m_Components[entity - 1].position[0], 3);
	ImGui::DragFloat3("Rotation", &m_Components[entity - 1].rotation[0], 3);
	ImGui::DragFloat3("Scale", &m_Components[entity - 1].scaling[0], 3);
	//ImGuizmo::RecomposeMatrixFromComponents(&m_Components[entity - 1].position[0], &matrixRotation[0], &matrixRotation[0], &model[0][0]);

	m_Components[entity - 1].worldMatrix = GetWorldMatrix(m_Components[entity - 1]);

	Matrix4 model = m_Components[entity - 1].worldMatrix;

	ImGuizmo::BeginFrame();
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	auto camera = Engine::Get()->GetGraphicManager()->GetCamera();
	ImGuizmo::Manipulate(&camera->viewMatrix[0][0], &camera->proj[0][0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, &(model[0][0]), NULL, NULL);

	m_Components[entity - 1].worldMatrix = model;
	Vec3f pos;
	Vec3f rot;
	Vec3f sca;
	ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &pos[0], &rot[0], &sca[0]);
	m_Components[entity - 1].position = pos;
	m_Components[entity - 1].rotation = rot;
	m_Components[entity - 1].scaling = sca;


}
}
