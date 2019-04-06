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

#define GLM_FORCE_RADIANS
#define FLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <system/camera_view.h>
#include <component/camera.h>
#include <entity/entity_handle.h>
#include <glm/ext/matrix_transform.inl>

namespace dm
{
CameraView::CameraView(Engine& engine) : System(engine)
{
	m_Signature.AddComponent(ComponentType::TRANSFORM);
	m_Signature.AddComponent(ComponentType::CAMERA);
}

void CameraView::Update()
{
	for (auto registeredEntity : m_RegisteredEntities)
	{
		auto entity = EntityHandle(registeredEntity, m_Engine);
		auto camera = entity.GetComponent<Camera>(ComponentType::CAMERA);
		const auto transform = entity.GetComponent<Transform>(ComponentType::TRANSFORM);

		auto rotM = glm::mat4(1.0f);

		rotM = rotate(rotM, glm::radians(transform->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotM = rotate(rotM, glm::radians(transform->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotM = rotate(rotM, glm::radians(transform->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		const auto transM = translate(glm::mat4(1.0f), glm::vec3(transform->position.x, transform->position.y, transform->position.z));

		camera->view = rotM * transM;

		//std::cout << *transform << "\n";
	}
}
}
