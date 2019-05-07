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

#include <system/input_controller.h>
#include <entity/entity_handle.h>
#include <component/control_type.h>
#include <engine/Input.h>
#include <engine/vector.h>

namespace dm
{
InputController::InputController(Engine& engine) : System(engine)
{
	m_Signature.AddComponent(ComponentType::TRANSFORM);
	m_Signature.AddComponent(ComponentType::CONTROL_TYPE);
}

void InputController::Update(float dt)
{
	for (auto registeredEntity : m_RegisteredEntities)
	{
		auto entity = EntityHandle(registeredEntity, m_Engine);
		auto transform = entity.GetComponent<Transform>(ComponentType::TRANSFORM);
		const auto controller = entity.GetComponent<ControllerType>(ComponentType::CONTROL_TYPE);

		switch(controller->type)
		{
		case ControllerType::ControllerTypeEnum::CAMERA_EDITOR: {
			auto* inputManager = m_Engine.GetInputManager();

			Vec3f camFront;
			camFront.x = -cos(transform->rotation.x * (3.14f / 180)) * sin(transform->rotation.y * (3.14f / 180));
			camFront.y = sin(transform->rotation.x  * (3.14f / 180));
			camFront.z = cos(transform->rotation.x  * (3.14f / 180)) * cos(transform->rotation.y  * (3.14f / 180));
			camFront = camFront.Normalized();

			static Vec2i originalPos;
			static Vec2i lastPos;

			auto moveSpeed = 0.01f;

			if (inputManager->IsButtonDown(ButtonCode::MIDDLE))
			{
				originalPos = inputManager->GetMousePosition();
				lastPos = originalPos;
			}


			if (inputManager->IsButtonHeld(ButtonCode::MIDDLE))
			{
				Vec2f offset(inputManager->GetMousePosition() - originalPos);

				if (lastPos != inputManager->GetMousePosition()) {
					offset = offset.Normalized();

					transform->position.x += offset.x * 0.25f;
					transform->position.y += offset.y * -0.25f;

					lastPos = inputManager->GetMousePosition();
				}
				else
				{
					originalPos = inputManager->GetMousePosition();
					lastPos = originalPos;
				}
			}

			if (inputManager->IsButtonReleased(ButtonCode::MIDDLE))
			{

			}

			if (inputManager->IsKeyHeld(KeyCode::SHIFT_LEFT))
			{
				moveSpeed *= 2;
			}

			if (inputManager->IsKeyHeld(KeyCode::LEFT))
			{
				transform->position -= Vec3f::Cross(camFront, Vec3f(0.0f, 1.0f, 0.0f)).Normalized() * moveSpeed;
			}

			if (inputManager->IsKeyHeld(KeyCode::RIGHT))
			{
				transform->position += Vec3f::Cross(camFront, Vec3f(0.0f, 1.0f, 0.0f)).Normalized() * moveSpeed;
			}

			if (inputManager->IsKeyHeld(KeyCode::DOWN))
			{
				transform->position -= camFront * moveSpeed;
			}

			if (inputManager->IsKeyHeld(KeyCode::UP))
			{
				transform->position += camFront * moveSpeed;
			}

			if (inputManager->scrollY > 0.1f)
			{
				transform->position += camFront;
			}
			else if (inputManager->scrollY < -0.1f)
			{
				transform->position -= camFront;
			}

			static Vec2i originalRot;
			static Vec2i lastRot;

			if (inputManager->IsButtonDown(ButtonCode::RIGHT))
			{
				originalRot = inputManager->GetMousePosition();
				lastRot = originalRot;
			}

			if (inputManager->IsButtonHeld(ButtonCode::RIGHT))
			{
				Vec2f offset(inputManager->GetMousePosition() - originalRot);

				if (lastRot != inputManager->GetMousePosition()) {
					offset = offset.Normalized();

					transform->rotation.x += offset.y * 0.5f;
					transform->rotation.y += offset.x * 0.5f;

					lastRot = inputManager->GetMousePosition();
				}
				else
				{
					originalRot = inputManager->GetMousePosition();
					lastRot = originalRot;
				}
			}

			if (inputManager->IsKeyHeld(KeyCode::A))
			{
				transform->rotation.y -= 0.02f;
			}

			if (inputManager->IsKeyHeld(KeyCode::D))
			{
				transform->rotation.y += 0.02f;
			}

			if (inputManager->IsKeyHeld(KeyCode::W))
			{
				transform->rotation.x += 0.02f;
			}

			if (inputManager->IsKeyHeld(KeyCode::S))
			{
				transform->rotation.x -= 0.02f;
			}
		}
			break;
		default: ;
		}
	}
}
}
