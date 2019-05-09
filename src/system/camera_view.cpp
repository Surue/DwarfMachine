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


#include <system/camera_view.h>
#include <component/camera.h>
#include <entity/entity_handle.h>
#include <glm/ext/matrix_transform.inl>
#include <glm/detail/func_trigonometric.inl>

namespace dm
{
CameraView::CameraView() : System()
{
	m_Signature.AddComponent(ComponentType::TRANSFORM);
	m_Signature.AddComponent(ComponentType::CAMERA);
}

void CameraView::Update(float dt)
{
	for (auto registeredEntity : m_RegisteredEntities)
	{
		auto entity = EntityHandle(registeredEntity);
		auto camera = entity.GetComponent<Camera>(ComponentType::CAMERA);
		const auto transform = entity.GetComponent<Transform>(ComponentType::TRANSFORM);

		// define your up vector
		glm::vec3 upVector = glm::vec3(0, 1, 0);
		// rotate around to a given bearing: yaw
		glm::mat4 ca = glm::rotate(glm::mat4(), 0.0f, upVector);
		// Define the 'look up' axis, should be orthogonal to the up axis
		glm::vec3 pitchVector = glm::vec3(1, 0, 0);
		// rotate around to the required head tilt: pitch
		ca = glm::rotate(ca, 0.0f, pitchVector);

		// now get the view matrix by taking the camera inverse
		glm::mat4 view = glm::inverse(ca);

		camera->viewMatrix = view;
	}
}
}
