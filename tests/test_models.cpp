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
#include <graphics/graphic_manager.h>
#include <component/mesh.h>
#include <editor/editor.h>
#include <graphics/model_manager.h>

#include <glm/ext/matrix_clip_space.inl>
#include <glm/detail/func_trigonometric.inl>
#include <glm/ext/matrix_transform.inl>
#include <glm/ext/scalar_constants.hpp>

TEST(Models, Cube)
{
	dm::EngineSettings settings;
	settings.windowSize = dm::Vec2i(800, 600);
	dm::Engine engine = dm::Engine(settings);
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

	//Cube
	const auto e1 = entityManager->CreateEntity();
	auto cube = dm::EntityHandle(e1);
	auto t1 = cube.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t1->position = glm::vec3(0, 0, 0);
	t1->scaling = glm::vec3(1, 1, 1);
	dm::Mesh mesh;
	mesh.componentType = ComponentType::MESH;
	mesh.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelCube");
	cube.AddComponent<dm::Mesh>(mesh);
	dm::MaterialDefault material;
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.color = dm::Color(100, 200, 0, 1);
	cube.AddComponent<dm::MaterialDefault>(material);

	//Sphere
	const auto e2 = entityManager->CreateEntity();
	auto sphere = dm::EntityHandle(e2);
	auto t2 = sphere.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t2->position = glm::vec3(2, 0, 0);
	t2->scaling = glm::vec3(1, 1, 1);
	dm::Mesh mesh2;
	mesh2.componentType = ComponentType::MESH;
	mesh2.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelPlane");
	sphere.AddComponent<dm::Mesh>(mesh2);
	sphere.AddComponent<dm::MaterialDefault>(material);

	//Planes
	const auto e3 = entityManager->CreateEntity();
	auto plane = dm::EntityHandle(e3);
	auto t3 = plane.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t3->position = glm::vec3(-2, 0, 0); 
	t3->scaling = glm::vec3(1, 1, 1);
	t3->rotation = glm::vec3(180 * glm::pi<float>() / 180, 0, 0);
	dm::Mesh mesh3;
	mesh3.componentType = ComponentType::MESH;
	mesh3.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere");
	plane.AddComponent<dm::Mesh>(mesh3);
	plane.AddComponent<dm::MaterialDefault>(material);

	//Obj
	const auto e4 = entityManager->CreateEntity();
	auto obj = dm::EntityHandle(e4);
	auto t4 = obj.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t4->position = glm::vec3(0, 0, 4);
	t4->scaling = glm::vec3(1, 1, 1);
	dm::Mesh mesh4;
	mesh4.componentType = ComponentType::MESH;
	mesh4.model = dm::Engine::Get()->GetModelManager()->GetModel("ressources/models/chalet.obj");
	obj.AddComponent<dm::Mesh>(mesh4);
	dm::MaterialDefault material1;
	material1.componentType = ComponentType::MATERIAL_DEFAULT;
	material1.color = dm::Color(100, 200, 0, 1);
	material1.textureDiffuse = dm::Image2d::Create("ressources/textures/chalet.jpg");
	obj.AddComponent<dm::MaterialDefault>(material1);

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}