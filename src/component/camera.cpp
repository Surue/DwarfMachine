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


#include <component/camera.h>
#include <graphics/graphic_manager.h>
#include <component/transform.h>
#include <entity/entity_handle.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "imgui.h"

namespace dm
{
CameraManager::CameraManager()
{
	m_GraphicManager = GraphicManager::Get();
}

void CameraManager::Init() {}

void CameraManager::Update() {}

Camera* CameraManager::CreateComponent(const Entity entity)
{
	auto c = Camera();
	c.componentType = ComponentType::CAMERA;
	c.viewMatrix = glm::lookAt(c.position, c.position + c.front, c.up);
	c.projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	c.isMain = false;

	c.right = glm::normalize(glm::cross(c.front, glm::vec3(0, -1, 0)));

	c.up = glm::normalize(glm::cross(c.right, c.front));
	//TODO mettre les valeurs depuis l'exterieur

	m_Components[entity - 1] = c;

	return &m_Components[entity - 1];
}

Camera* CameraManager::AddComponent(const Entity entity, Camera& componentBase)
{
	m_Components[entity - 1] = componentBase;

	if(componentBase.isMain)
	{
		m_GraphicManager->SetMainCamera(&m_Components[entity - 1]);
	}

	return &m_Components[entity - 1];
}

void CameraManager::DestroyComponent(Entity entity)
{
}

void CameraManager::OnDrawInspector(const Entity entity)
{
	ImGui::Separator();
	ImGui::TextWrapped("Camera");
	//ImGui::Text("Position ( " + std::to_string(m_Components[entity - 1].position.x) + ", " + std::to_string(m_Components[entity - 1].position.y) + ", " + std::to_string(m_Components[entity - 1].position.z) + " )");

	ImGui::InputFloat("Near", &m_Components[entity - 1].nearFrustum);
	ImGui::InputFloat("Far", &m_Components[entity - 1].farFrustum);
}

void CameraManager::OnEntityResize(const int newSize)
{

	m_Components.resize(newSize);

	for (auto& component : m_Components)
	{
		if (component.isMain)
		{
			m_GraphicManager->SetMainCamera(&component);
			return;
		}
	}
}

void CameraManager::UpdateAspect(const float newAspect)
{
	for (auto& component : m_Components)
	{
		component.aspect = newAspect;
		component.projectionMatrix = glm::perspective(glm::radians(component.fov), component.aspect, component.nearFrustum, component.farFrustum);
	}
}

void CameraManager::CreateComponent(json& componentJson, const Entity entity)
{
	Camera camera;

	if (CheckJsonExists(componentJson, "position"))
		camera.position = GetVector3FromJson(componentJson, "position");

	if (CheckJsonExists(componentJson, "up"))
		camera.up = GetVector3FromJson(componentJson, "up");

	if (CheckJsonExists(componentJson, "right"))
		camera.right = GetVector3FromJson(componentJson, "right");

	if (CheckJsonExists(componentJson, "front"))
		camera.front = GetVector3FromJson(componentJson, "front");

	if (CheckJsonExists(componentJson, "aspect") && CheckJsonNumber(componentJson, "aspect"))
		camera.aspect = componentJson["aspect"];

	if (CheckJsonExists(componentJson, "fov") && CheckJsonNumber(componentJson, "fov"))
		camera.fov = componentJson["fov"];

	if (CheckJsonExists(componentJson, "near") && CheckJsonNumber(componentJson, "near"))
		camera.nearFrustum = componentJson["near"];

	if (CheckJsonExists(componentJson, "far") && CheckJsonNumber(componentJson, "far"))
		camera.farFrustum = componentJson["far"];

	if (CheckJsonExists(componentJson, "yaw") && CheckJsonNumber(componentJson, "yaw"))
		camera.yaw = componentJson["yaw"];

	if (CheckJsonExists(componentJson, "pitch") && CheckJsonNumber(componentJson, "pitch"))
		camera.pitch = componentJson["pitch"];

	if (CheckJsonExists(componentJson, "isMain") && CheckJsonNumber(componentJson, "isMain"))
	{
		if (componentJson["isMain"] == 1)
		{
			camera.isMain = true;
		}
		else
		{
			camera.isMain = false;
		}
	}

	if (CheckJsonExists(componentJson, "isCulling") && CheckJsonNumber(componentJson, "isCulling"))
	{
		if (componentJson["isCulling"] == 1)
		{
			camera.isCulling = true;
		}
		else
		{
			camera.isCulling = false;
		}
	}

	camera.viewMatrix = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
	camera.projectionMatrix = glm::perspective(glm::radians(camera.fov), camera.aspect, camera.nearFrustum, camera.farFrustum);

	m_Components[entity - 1] = camera;
}
}
