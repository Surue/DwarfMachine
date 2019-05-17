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
#include <component/model.h>
#include <editor/editor.h>
#include <graphics/mesh_manager.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <utility>
#include "physic/bounding_sphere.h"

#include <thread>

TEST(Models, Cube)
{
	dm::EngineSettings settings;
	settings.windowSize = dm::Vec2i(800, 600);
	dm::Engine engine = dm::Engine(settings);
	engine.Init();

	engine.SetApplication(new dm::Editor());

	auto editor = static_cast<dm::Editor*>(engine.GetApplication());

	auto entityManager = engine.GetEntityManager();

	//Camera
	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	dm::Camera cameraInfo;
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMainCamera = true;
	cameraInfo.viewMatrix = glm::lookAt(cameraInfo.pos, cameraInfo.pos + cameraInfo.front, cameraInfo.up);
	cameraInfo.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	//Cube
	const auto e1 = entityManager->CreateEntity();
	auto cube = dm::EntityHandle(e1);
	auto t1 = cube.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t1->position = glm::vec3(0, 0, 0);
	t1->scaling = glm::vec3(1, 1, 1);
	dm::Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelCube");
	cube.AddComponent<dm::Model>(mesh);
	dm::MaterialDefault material;
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.color = dm::Color(100, 200, 0, 1);
	cube.AddComponent<dm::MaterialDefault>(material);
	cube.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh.model));

	dm::Gizmo gizmo;
	gizmo.transform = t1;
	gizmo.color = dm::Color(100, 0, 0, 1);
	gizmo.gizmoType = dm::GizmoType::Create(dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere"), 1, dm::Color::White);
	editor->GetGizmoManager()->AddGizmo(gizmo);

	//Sphere
	const auto e2 = entityManager->CreateEntity();
	auto sphere = dm::EntityHandle(e2);
	auto t2 = sphere.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t2->position = glm::vec3(2, 0, 0);
	t2->scaling = glm::vec3(1, 1, 1);
	dm::Model mesh2;
	mesh2.componentType = ComponentType::MODEL;
	mesh2.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelPlane");
	sphere.AddComponent<dm::Model>(mesh2);
	sphere.AddComponent<dm::MaterialDefault>(material);
	sphere.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh2.model));

	dm::Gizmo gizmo2;
	gizmo2.transform = t2;
	gizmo2.color = dm::Color(100, 0, 0, 1);
	gizmo2.gizmoType = gizmo.gizmoType;
	editor->GetGizmoManager()->AddGizmo(gizmo2);

	//Planes
	const auto e3 = entityManager->CreateEntity();
	auto plane = dm::EntityHandle(e3);
	auto t3 = plane.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t3->position = glm::vec3(-2, 0, 0); 
	t3->scaling = glm::vec3(1, 1, 1);
	t3->rotation = glm::vec3(180 * glm::pi<float>() / 180, 0, 0);
	dm::Model mesh3;
	mesh3.componentType = ComponentType::MODEL;
	mesh3.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere");
	plane.AddComponent<dm::Model>(mesh3);
	plane.AddComponent<dm::MaterialDefault>(material);
	plane.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh3.model));

	dm::Gizmo gizmo3;
	gizmo3.transform = t3;
	gizmo3.color = dm::Color(100, 0, 0, 1);
	gizmo3.gizmoType = gizmo.gizmoType;
	editor->GetGizmoManager()->AddGizmo(gizmo3);

	////Obj
	//const auto e4 = entityManager->CreateEntity();
	//auto obj = dm::EntityHandle(e4);
	//auto t4 = obj.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	//t4->position = glm::vec3(0, 0, 4);
	//t4->scaling = glm::vec3(1, 1, 1);
	//dm::Model mesh4;
	//mesh4.componentType = ComponentType::MODEL;
	//mesh4.model = dm::Engine::Get()->GetModelManager()->GetModel("ressources/models/chalet.obj");
	//obj.AddComponent<dm::Model>(mesh4);
	//dm::MaterialDefault material1;
	//material1.componentType = ComponentType::MATERIAL_DEFAULT;
	//material1.color = dm::Color(100, 200, 0, 1);
	//material1.textureDiffuse = dm::Image2d::Create("ressources/textures/chalet.jpg");
	//obj.AddComponent<dm::MaterialDefault>(material1);

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}

void CreateCube(glm::vec3 pos, dm::EntityManager* entityManager, dm::Editor* editor, std::shared_ptr<dm::GizmoType> gizmoType, dm::MaterialDefault material)
{
	const auto entityHandle = entityManager->CreateEntity();
	auto cube = dm::EntityHandle(entityHandle);

	//Transform
	auto t1 = cube.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scaling = glm::vec3(1, 1, 1);

	//Mesh
	dm::Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelCube");
	cube.AddComponent<dm::Model>(mesh);

	//Material
	cube.AddComponent<dm::MaterialDefault>(material);

	//Bounding sphere
	cube.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh.model));

	//Gizmo
	dm::Gizmo gizmo;
	gizmo.transform = t1;
	gizmo.color = dm::Color(100, 0, 0, 1);
	gizmo.gizmoType = std::move(gizmoType);
	gizmo.entity = entityHandle;
	editor->GetGizmoManager()->AddGizmo(gizmo);
}

TEST(Models, FrustumCulling)
{
	dm::EngineSettings settings;
	settings.windowSize = dm::Vec2i(800, 600);
	dm::Engine engine = dm::Engine(settings);
	engine.Init();

	engine.SetApplication(new dm::Editor());

	auto editor = static_cast<dm::Editor*>(engine.GetApplication());

	auto entityManager = engine.GetEntityManager();

	//Camera
	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	dm::Camera cameraInfo;
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMainCamera = true;
	cameraInfo.isCullingCamera = true;
	cameraInfo.viewMatrix = glm::lookAt(cameraInfo.pos, cameraInfo.pos + cameraInfo.front, cameraInfo.up);
	cameraInfo.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	std::shared_ptr<dm::GizmoType> gizmoType = dm::GizmoType::Create(dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere"), 1, dm::Color::White);
	
	//Cube
	float maxCube = 5;

	dm::MaterialDefault material;
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.color = dm::Color(100, 200, 0, 1);

	for(size_t i = 0; i < maxCube; i++)
	{
		for(size_t j = 0; j < maxCube; j++)
		{
			glm::vec3 pos = glm::vec3(i - maxCube / 2.0f + i * 1.0f, 0, j - maxCube / 2.0f + j * 1.0f);
			CreateCube(pos, entityManager, editor, gizmoType, std::move(material));		
		}
	}

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}

void Test()
{
	//long long time = 0;

	//int a = 0;
	//for (int i = 0; i < 100000; i++) {

	//	std::chrono::high_resolution_clock::time_point before = std::chrono::high_resolution_clock::now();
	//	a += sqrt(i);
	//	std::chrono::high_resolution_clock::time_point after = std::chrono::high_resolution_clock::now();

	//	time += std::chrono::duration_cast<std::chrono::duration<long long, std::chrono::nanoseconds>>(after - before).count();
	//}

	//time /= 100000;

	//std::cout << time << "\n";
}

void Start()
{
	/*float time = 0;
	for (int i = 0; i < 100000; i++) {
		std::thread myThread(Test);

		std::chrono::high_resolution_clock::time_point before = std::chrono::high_resolution_clock::now();
		myThread.join();
		std::chrono::high_resolution_clock::time_point after = std::chrono::high_resolution_clock::now();

		time += std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(after - before).count() / 1000.0f;
	}

	time /= 100000;

	std::cout << time << "\n";*/

	Test();
}

TEST(Models, thread)
{
	Start();

	system("pause");
}