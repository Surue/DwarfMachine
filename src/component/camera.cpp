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
#include "entity/entity_handle.h"
#include <glm/ext/matrix_clip_space.inl>
#include <glm/detail/func_trigonometric.inl>
#include <glm/ext/matrix_transform.inl>

namespace dm
{
CameraManager::CameraManager()
{
	m_GraphicManager = GraphicManager::Get();
}

void CameraManager::Awake() {}

void CameraManager::Start() {}

void CameraManager::Update() {}

Camera* CameraManager::CreateComponent(const Entity entity)
{
	auto c = Camera();
	c.componentType = ComponentType::CAMERA;
	c.viewMatrix = glm::lookAt(glm::vec3(10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	c.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	c.isMainCamera = false;
	//TODO mettre les valeurs depuis l'exterieur

	m_Components[entity - 1] = c;

	return &m_Components[entity - 1];
}

Camera* CameraManager::AddComponent(const Entity entity, Camera& componentBase)
{
	m_Components[entity - 1] = componentBase;

	if(componentBase.isMainCamera)
	{
		m_GraphicManager->SetMainCamera(&m_Components[entity - 1]);
	}

	return &m_Components[entity - 1];
}

Transform* CameraManager::GetTransformOfCamera(Camera& component)
{
	for(auto i = 0; i < m_Components.size(); i++)
	{
		if(m_Components.at(i).isMainCamera)
		{
			auto entity = EntityHandle(i + 1);
			return entity.GetComponent<Transform>(ComponentType::TRANSFORM);
		}
	}

	return nullptr;
}

void CameraManager::DestroyComponent(Entity entity)
{
}

void CameraManager::OnDrawInspector(Entity entity) {}
}
