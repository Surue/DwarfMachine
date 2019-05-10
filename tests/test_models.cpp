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

#include <gtest/gtest.h>

#include <engine/engine.h>

#include <component/transform.h>
#include <entity/entity_handle.h>
#include <editor/editor_renderer.h>
#include <graphics/graphic_manager.h>
#include "component/mesh.h"
#include "graphics/model_cube.h"
#include <editor/editor.h>

#include <glm/ext/matrix_clip_space.inl>
#include <glm/detail/func_trigonometric.inl>
#include <glm/ext/matrix_transform.inl>

TEST(Models, Cube)
{
	dm::Engine engine;
	engine.Init();

	engine.SetApplication(new dm::Editor());

	auto entityManager = engine.GetEntityManager();

	//Camera
	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	dm::Camera cameraInfo;
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMainCamera = true;
	cameraInfo.viewMatrix = glm::lookAt(cameraInfo.cameraPos, cameraInfo.cameraPos + cameraInfo.cameraFront, cameraInfo.cameraUp);
	cameraInfo.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	//Cube 0
	const auto e1 = entityManager->CreateEntity();
	auto cube = dm::EntityHandle(e1);
	auto transform = cube.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	transform->position = glm::vec3(0, 0, 0);
	transform->scaling = glm::vec3(1, 1, 1);
	dm::Mesh mesh;
	mesh.componentType = ComponentType::MESH;
	mesh.model = dm::ModelCube::Create(dm::Vec3f(1.0f, 1.0f, 1.0f));
	cube.AddComponent<dm::Mesh>(mesh);

	dm::MaterialDefault material;
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.color = dm::Color(100, 200, 0, 1);
	cube.AddComponent<dm::MaterialDefault>(material);

	//Cube 1
	/*const auto e2 = entityManager->CreateEntity();
	auto cube2 = dm::EntityHandle(e2);
	auto transform2 = cube2.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	transform2->position = glm::vec3(2, 0, 0);
	transform2->scaling = glm::vec3(1, 1, 1);
	dm::Mesh mesh2;
	mesh2.componentType = ComponentType::MESH;
	mesh2.model = dm::ModelCube::Create(dm::Vec3f(1.0f, 1.0f, 1.0f));
	cube2.AddComponent<dm::Mesh>(mesh2);

	dm::MaterialDefault material2;
	material2.componentType = ComponentType::MATERIAL_DEFAULT;
	material2.color = dm::Color(100, 200, 0, 1);
	cube2.AddComponent<dm::MaterialDefault>(material2);*/
	
	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}