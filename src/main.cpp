/*
MIT License

Copyright (c) 2017 SAE Institute Switzerland AG

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
#include <iostream>

#define GLM_FORCE_RADIANS
#define FLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <engine/engine.h>

#include <component/transform.h>
#include <entity/entity_handle.h>
#include <glm/ext/matrix_transform.inl>
#include <glm/ext/matrix_clip_space.inl>

int main()
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0, engine);

	auto t = entity.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t->position = dm::Vec3f(0, 0, -10.0f);

	dm::Camera cameraInfo;
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMainCamera = true;
	cameraInfo.view = lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cameraInfo.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	auto controller = entity.CreateComponent<dm::ControllerType>(ComponentType::CONTROL_TYPE);
	controller->type = dm::ControllerType::ControllerTypeEnum::CAMERA_EDITOR;

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}
