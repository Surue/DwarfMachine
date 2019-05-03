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

TEST(Models, Cube)
{
	dm::Engine engine;
	engine.Init();

	engine.GetGraphicManager()->SetManager(new dm::EditorRenderManager());

	auto entityManager = engine.GetEntityManager();

	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0, engine);

	auto t = entity.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t->position = dm::Vec3f(0, 0, 0);
	t->rotation = dm::Vec3f(0, 0, 0);

	dm::Camera cameraInfo;
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMainCamera = true;
	cameraInfo.view = dm::Matrix4::ViewMatrix(t->position, t->rotation);
	cameraInfo.proj = dm::Matrix4::PerspectiveMatrix(45.0f  * (3.14f / 180), 800.0f / 600.0f, 0.1f, 100.0f);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	auto controller = entity.CreateComponent<dm::ControllerType>(ComponentType::CONTROL_TYPE);
	controller->type = dm::ControllerType::ControllerTypeEnum::CAMERA_EDITOR;

	const auto e1 = entityManager->CreateEntity();
	auto cube = dm::EntityHandle(e1, engine);
	auto transform = cube.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	transform->position = dm::Vec3f(10, 10, 10);
	dm::Mesh mesh;
	mesh.componentType = ComponentType::MESH;
	mesh.model = dm::ModelCube::Create(dm::Vec3f(1.0f, 1.0f, 1.0f));
	cube.AddComponent<dm::Mesh>(mesh);

	dm::MaterialDefault material;
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.color = dm::Color(100, 200, 0, 1);
	cube.AddComponent<dm::MaterialDefault>(material);
	
	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}