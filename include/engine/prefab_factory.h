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

#ifndef PREFAB_FACTORY_H
#define PREFAB_FACTORY_H
#include <glm/vec3.hpp>
#include <engine/engine.h>
#include <entity/entity_handle.h>
#include <graphics/mesh_manager.h>

namespace dm
{
struct MaterialDefault;
class EntityManager;

class PrefabFactor
{
public:
	static void CreateRock1(glm::vec3 pos)
	{
		
	}

	static void CreateRock2(glm::vec3 pos)
	{

	}

	static void CreateRock3(glm::vec3 pos)
	{

	}

	static void CreateTree1(glm::vec3 pos)
	{

	}

	static void CreateTree2(glm::vec3 pos)
	{

	}

	static void CreateSphere(glm::vec3 pos)
	{
		auto entityManager = Engine::Get()->GetEntityManager();
		const auto entityHandle = entityManager->CreateEntity();
		auto sphere = dm::EntityHandle(entityHandle);

		//Transform
		auto t1 = sphere.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
		t1->position = pos;
		t1->scale = glm::vec3(1, 1, 1);

		//Mesh
		dm::Model mesh;
		mesh.componentType = ComponentType::MODEL;
		mesh.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere");
		sphere.AddComponent<dm::Model>(mesh);

		//Material
		auto material = MaterialDefault();
		material.componentType = ComponentType::MATERIAL_DEFAULT;
		material.castsShadows = true;
		material.ignoreLighting = false;
		material.ignoreFog = false;
		material.metallic = 0;
		material.roughness = 0;
		sphere.AddComponent<dm::MaterialDefault>(material);

		//Bounding sphere
		sphere.AddComponent<BoundingSphere>(BoundingSphereManager::GetBoundingSphere(*mesh.model));

		//Drawable
		sphere.CreateComponent<Drawable>(ComponentType::DRAWABLE);

		//MeshRenderer
		sphere.CreateComponent<MeshRenderer>(ComponentType::MESH_RENDERER);

		//Shadow Renderer
		sphere.CreateComponent<ShadowRenderer>(ComponentType::SHADOW_RENDERER);
	}

	static void CreateCube(glm::vec3 pos)
	{
		auto entityManager = Engine::Get()->GetEntityManager();
		const auto entityHandle = entityManager->CreateEntity();
		auto cube = EntityHandle(entityHandle);

		//Transform
		auto t1 = cube.CreateComponent<Transform>(ComponentType::TRANSFORM);
		t1->position = pos;
		t1->scale = glm::vec3(1, 1, 1);

		//Mesh
		Model mesh;
		mesh.componentType = ComponentType::MODEL;
		mesh.model = Engine::Get()->GetModelManager()->GetModel("ModelCube");
		cube.AddComponent<Model>(mesh);

		//Material
		auto material = MaterialDefault();
		material.componentType = ComponentType::MATERIAL_DEFAULT;
		material.castsShadows = true;
		material.ignoreLighting = false;
		material.ignoreFog = false;
		material.metallic = 0;
		material.roughness = 0;
		cube.AddComponent<MaterialDefault>(material);

		//Bounding sphere
		cube.AddComponent<BoundingSphere>(BoundingSphereManager::GetBoundingSphere(*mesh.model));

		//Drawable
		cube.CreateComponent<Drawable>(ComponentType::DRAWABLE);

		//MeshRenderer
		cube.CreateComponent<MeshRenderer>(ComponentType::MESH_RENDERER);

		//Shadow Renderer
		cube.CreateComponent<ShadowRenderer>(ComponentType::SHADOW_RENDERER);
	}

	static void CreatePlane(glm::vec3 pos)
	{
		auto entityManager = Engine::Get()->GetEntityManager();
		const auto entityHandle = entityManager->CreateEntity();
		auto plane = dm::EntityHandle(entityHandle);

		//Transform
		auto t1 = plane.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
		t1->position = pos;
		t1->scale = glm::vec3(1, 1, 1);

		//Mesh
		dm::Model mesh;
		mesh.componentType = ComponentType::MODEL;
		mesh.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelPlane");
		plane.AddComponent<dm::Model>(mesh);

		//Material
		auto material = MaterialDefault();
		material.componentType = ComponentType::MATERIAL_DEFAULT;
		material.castsShadows = true;
		material.ignoreLighting = false;
		material.ignoreFog = false;
		material.metallic = 0;
		material.roughness = 0;
		plane.AddComponent<dm::MaterialDefault>(material);

		//Bounding sphere
		plane.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh.model));

		//Drawable
		plane.CreateComponent<dm::Drawable>(ComponentType::DRAWABLE);

		//MeshRenderer
		plane.CreateComponent<dm::MeshRenderer>(ComponentType::MESH_RENDERER);

		//Shadow Renderer
		plane.CreateComponent<dm::ShadowRenderer>(ComponentType::SHADOW_RENDERER);
	}
};
}

#endif