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

#include <component/transform.h>
#include <entity/entity_handle.h>
#include <graphics/graphic_manager.h>
#include <component/mesh.h>

#include <glm/ext/matrix_clip_space.inl>
#include <glm/detail/func_trigonometric.inl>
#include <glm/ext/matrix_transform.inl>

TEST(Entity, CreateEntity)
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	const auto e0 = entityManager->CreateEntity();
	const auto e1 = entityManager->CreateEntity();
	const auto e2 = entityManager->CreateEntity();

	std::cout << "e0 = " << e0 << "\n";
	std::cout << "e1 = " << e1 << "\n";
	std::cout << "e2 = " << e2 << "\n";
}

TEST(Entity, CreateMultipleEntity)
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	for(size_t i = 0; i < 100; i++)
	{
		const auto e = entityManager->CreateEntity();
		ASSERT_EQ(i, e - 1);
	}
}

void CreateAndDelete(dm::EntityManager* entityManager)
{
	const auto e0 = entityManager->CreateEntity();
	std::cout << "e0 = " << e0 << "\n";
	const auto e1 = entityManager->CreateEntity();
	std::cout << "e1 = " << e1 << "\n";
	std::cout << "destroy e1\n";
	entityManager->DestroyEntity(e1);
	const auto e2 = entityManager->CreateEntity();
	std::cout << "e2 = " << e2 << "\n";
}

TEST(Entity, CreateAndDestroyEntity)
{
	dm::Engine engine;
	engine.Init();

	const auto entityManager = engine.GetEntityManager();
	GTEST_TEST_NO_THROW_(CreateAndDelete(entityManager), GTEST_FATAL_FAILURE_);
}

TEST(Entity, TransformComponentEntity)
{
	dm::Engine engine;
	engine.Init();

	const auto x = 4;
	const auto y = 5;
	const auto z = 6;

	auto entityManager = engine.GetEntityManager();

	const auto e1 = entityManager->CreateEntity();
	auto entity1 = dm::EntityHandle(e1);
	std::cout << "ici\n";
	dm::Transform transformInfo;
	transformInfo.componentType = ComponentType::TRANSFORM;
	transformInfo.position.x = x;
	transformInfo.position.y = y;
	transformInfo.position.z = z;

	transformInfo.rotation.x = 0;
	transformInfo.rotation.y = 0;
	transformInfo.rotation.z = 0;
	auto* t2 = entity1.AddComponent<dm::Transform>(transformInfo);

	auto* transform2 = entity1.GetComponent<dm::Transform>(ComponentType::TRANSFORM);

	ASSERT_FLOAT_EQ(x, transform2->position.x);
	ASSERT_FLOAT_EQ(y, transform2->position.y);
	ASSERT_FLOAT_EQ(z, transform2->position.z);

	ASSERT_FLOAT_EQ(x, t2->position.x);
	ASSERT_FLOAT_EQ(y, t2->position.y);
	ASSERT_FLOAT_EQ(z, t2->position.z);

	ASSERT_FLOAT_EQ(0, transform2->rotation.x);
	ASSERT_FLOAT_EQ(0, transform2->rotation.y);
	ASSERT_FLOAT_EQ(0, transform2->rotation.z);

	ASSERT_FLOAT_EQ(0, t2->rotation.x);
	ASSERT_FLOAT_EQ(0, t2->rotation.y);
	ASSERT_FLOAT_EQ(0, t2->rotation.z);
}

TEST(Entity, HasComponentEntity)
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	entity.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);

	ASSERT_TRUE(entity.HasComponent(ComponentType::TRANSFORM));

	ASSERT_FALSE(entity.HasComponent(ComponentType::CAMERA));
}

TEST(Entity, DestroyComponentEntity)
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	std::cout << "Create component Transform\n";
	entity.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);

	ASSERT_TRUE(entity.HasComponent(ComponentType::TRANSFORM));

	std::cout << "Delete component Transform\n";
	entity.DestroyComponent(ComponentType::TRANSFORM);

	ASSERT_FALSE(entity.HasComponent(ComponentType::TRANSFORM));
}

TEST(Entity, SystemAddComponent)
{
	dm::Engine engine;
	engine.Init();

	auto entityManager = engine.GetEntityManager();

	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	auto t = entity.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t->position = glm::vec3(0, 0, 10.0f);
	t->rotation = glm::vec3(0, 0, 0);

	dm::Camera cameraInfo;
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMainCamera = true;
	cameraInfo.viewMatrix = glm::lookAt(glm::vec3(10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	cameraInfo.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);
}