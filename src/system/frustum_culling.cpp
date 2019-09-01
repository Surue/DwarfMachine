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

#include <system/frustum_culling.h>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <component/camera.h>
#include <engine/engine.h>
#include <component/component_manager.h>
#include <entity/entity_handle.h>

namespace dm
{
FrustumCulling::FrustumCulling() :
	m_CameraForCulling(nullptr)
{
	m_Signature.AddComponent(ComponentType::BOUNDING_SPHERE);
	m_Signature.AddComponent(ComponentType::TRANSFORM);
	m_Signature.AddComponent(ComponentType::DRAWABLE);
}

void FrustumCulling::Update()
{
	//Get the rendering camera
	//TODO optimiser cette partie pour setter une seule fois la caméra
	auto cameras = Engine::Get()->GetComponentManager()->GetCameraManager()->GetComponents();

	for (Camera& camera : cameras)
	{
		if(camera.isCulling)
		{
			m_CameraForCulling = &camera;
			break;
		}
	}

	//Compute camera planes
	const auto aspect = m_CameraForCulling->aspect;

	float angle = glm::radians(45.0f) / 2.0f;

	const auto leftDir = glm::normalize(glm::rotate(m_CameraForCulling->front, angle * aspect, m_CameraForCulling->up));
	const auto leftNormal = glm::normalize(glm::cross(leftDir, m_CameraForCulling->up));

	const auto rightDir = glm::normalize(glm::rotate(m_CameraForCulling->front, -angle * aspect, m_CameraForCulling->up));
	const auto rightNormal = glm::normalize(-glm::cross(rightDir, m_CameraForCulling->up));

	const auto upDir = glm::normalize(glm::rotate(m_CameraForCulling->front, -angle, m_CameraForCulling->right));
	const auto upNormal = glm::normalize(glm::cross(upDir, m_CameraForCulling->right));

	const auto downDir = glm::normalize(glm::rotate(m_CameraForCulling->front, angle, m_CameraForCulling->right));
	const auto downNormal = glm::normalize(-glm::cross(downDir, m_CameraForCulling->right));

	for (auto entity : m_RegisteredEntities)
	{
		auto entityHandle = EntityHandle(entity);
		auto boundingSphere = entityHandle.GetComponent<BoundingSphere>(ComponentType::BOUNDING_SPHERE);
		auto transform = entityHandle.GetComponent<Transform>(ComponentType::TRANSFORM);
		auto drawable = entityHandle.GetComponent<Drawable>(ComponentType::DRAWABLE);
		drawable->isDrawable = true;

		const auto cameraToSphere = transform->position - m_CameraForCulling->position;

		//near culling
		if (glm::dot(cameraToSphere, m_CameraForCulling->front) < m_CameraForCulling->nearFrustum + boundingSphere->radius)
		{
			continue;
		}

		//far culling
		if (glm::dot(cameraToSphere, m_CameraForCulling->front) > m_CameraForCulling->farFrustum - boundingSphere->radius)
		{
			continue;
		}

		//left culling
		if (glm::dot(cameraToSphere, leftNormal) < -boundingSphere->radius)
		{
			continue;
		}

		//right culling
		if (glm::dot(cameraToSphere, rightNormal) < -boundingSphere->radius)
		{
			continue;
		}

		//up culling
		if (glm::dot(cameraToSphere, upNormal) > boundingSphere->radius)
		{
			continue;
		}

		//down culling
		if (glm::dot(cameraToSphere, downNormal) > boundingSphere->radius)
		{
			continue;
		}

		drawable->isDrawable = true;
	}
}
}
