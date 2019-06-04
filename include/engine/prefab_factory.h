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
#include <graphics/image_2d.h>
#include "graphics/texture_manager.h"

namespace dm
{
struct MaterialDefault;
class EntityManager;

class PrefabFactor
{
public:
	static void CreateRock1(const glm::vec3 pos)
	{
		auto entityManager = Engine::Get()->GetEntityManager();
		const auto entityHandle = entityManager->CreateEntity();
		auto rock1 = EntityHandle(entityHandle);

		//Transform
		auto t1 = rock1.CreateComponent<Transform>(ComponentType::TRANSFORM);
		t1->position = pos;
		t1->scale = glm::vec3(0.01, 0.01, 0.01);

		//Mesh
		Model mesh;
		mesh.componentType = ComponentType::MODEL;
		mesh.model = Engine::Get()->GetModelManager()->GetModel("ressources/models/rocks_01_model.obj");
		rock1.AddComponent<Model>(mesh);

		//Material
		auto material = MaterialDefault();
		material.componentType = ComponentType::MATERIAL_DEFAULT;
		material.castsShadows = true;
		material.ignoreLighting = false;
		material.ignoreFog = false;
		material.metallic = 0;
		material.roughness = 0;
		material.diffuseTexture = TextureManager::Get()->rock1Diffuse;
		material.normalTexture = TextureManager::Get()->rock1Normal;
		rock1.AddComponent<MaterialDefault>(material);

		//Bounding sphere
		rock1.AddComponent<BoundingSphere>(BoundingSphereManager::GetBoundingSphere(*mesh.model));

		//Drawable
		rock1.CreateComponent<Drawable>(ComponentType::DRAWABLE);

		//MeshRenderer
		rock1.CreateComponent<MeshRenderer>(ComponentType::MESH_RENDERER);

		//Shadow Renderer
		rock1.CreateComponent<ShadowRenderer>(ComponentType::SHADOW_RENDERER);
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

	static void CreateSphere(glm::vec3 pos);

	static void CreateCube(glm::vec3 pos);

	static void CreatePlane(glm::vec3 pos);
};
}

#endif