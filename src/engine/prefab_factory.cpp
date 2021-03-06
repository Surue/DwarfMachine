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

#include <engine/prefab_factory.h>

namespace dm
{
void PrefabFactor::CreateRock1(const glm::vec3 pos)
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
	material.diffuseTexture = TextureManager::Get()->GetTextureByName(
		"ressources/textures/rocks_01_model/rocks_01_dif.tga");
	material.normalTexture = TextureManager::Get()->GetTextureByName(
		"ressources/textures/rocks_01_model/rocks_01_nm.tga");
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

void PrefabFactor::CreateRock2(const glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto rock1 = EntityHandle(entityHandle);

	//Transform
	auto t1 = rock1.CreateComponent<Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(0.1, 0.1, 0.1);

	//Mesh
	Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = Engine::Get()->GetModelManager()->GetModel("ressources/models/rock_v2.obj");
	rock1.AddComponent<Model>(mesh);

	//Material
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.castsShadows = true;
	material.ignoreLighting = false;
	material.ignoreFog = false;
	material.metallic = 0;
	material.roughness = 0;
	material.diffuseTexture = TextureManager::Get()->GetTextureByName(
		"ressources/textures/rock_v2/rock_v2_obj_lambert2SG_BaseColor.png");
	material.normalTexture = TextureManager::Get()->GetTextureByName(
		"ressources/textures/rock_v2/rock_v2_obj_lambert2SG_Normal.png");
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

void PrefabFactor::CreateTree1(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto rock1 = EntityHandle(entityHandle);

	//Transform
	auto t1 = rock1.CreateComponent<Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(0.01, 0.01, 0.01);
	t1->rotation = glm::vec3(4.7f, 0, 0);

	//Mesh
	Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = Engine::Get()->GetModelManager()->GetModel("ressources/models/tree_1.obj");
	rock1.AddComponent<Model>(mesh);

	//Material
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.castsShadows = true;
	material.ignoreLighting = false;
	material.ignoreFog = false;
	material.metallic = 0;
	material.roughness = 0;
	material.diffuseTexture = TextureManager::Get()->GetTextureByName("ressources/textures/tree_1.jpg");
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

void PrefabFactor::CreateTree2(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto rock1 = EntityHandle(entityHandle);

	//Transform
	auto t1 = rock1.CreateComponent<Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(0.01, 0.01, 0.01);
	t1->rotation = glm::vec3(4.7f, 0, 0);

	//Mesh
	Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = Engine::Get()->GetModelManager()->GetModel("ressources/models/tree_2.obj");
	rock1.AddComponent<Model>(mesh);

	//Material
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.castsShadows = true;
	material.ignoreLighting = false;
	material.ignoreFog = false;
	material.metallic = 0;
	material.roughness = 0;
	material.diffuseTexture = TextureManager::Get()->GetTextureByName("ressources/textures/tree_2.jpg");
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

void PrefabFactor::CreateTree3(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto rock1 = EntityHandle(entityHandle);

	//Transform
	auto t1 = rock1.CreateComponent<Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(0.01, 0.01, 0.01);
	t1->rotation = glm::vec3(4.7f, 0, 0);

	//Mesh
	Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = Engine::Get()->GetModelManager()->GetModel("ressources/models/tree_3.obj");
	rock1.AddComponent<Model>(mesh);

	//Material
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.castsShadows = true;
	material.ignoreLighting = false;
	material.ignoreFog = false;
	material.metallic = 0;
	material.roughness = 0;
	material.diffuseTexture = TextureManager::Get()->GetTextureByName("ressources/textures/tree_3.jpg");
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

void PrefabFactor::CreateTree4(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto rock1 = EntityHandle(entityHandle);

	//Transform
	auto t1 = rock1.CreateComponent<Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(0.01, 0.01, 0.01);
	t1->rotation = glm::vec3(4.7f, 0, 0);

	//Mesh
	Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = Engine::Get()->GetModelManager()->GetModel("ressources/models/tree_4.obj");
	rock1.AddComponent<Model>(mesh);

	//Material
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.castsShadows = true;
	material.ignoreLighting = false;
	material.ignoreFog = false;
	material.metallic = 0;
	material.roughness = 0;
	material.diffuseTexture = TextureManager::Get()->GetTextureByName("ressources/textures/tree_4.jpg");
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

void PrefabFactor::CreateTree5(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto rock1 = EntityHandle(entityHandle);

	//Transform
	auto t1 = rock1.CreateComponent<Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(0.01, 0.01, 0.01);
	t1->rotation = glm::vec3(4.7f, 0, 0);

	//Mesh
	Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = Engine::Get()->GetModelManager()->GetModel("ressources/models/tree_5.obj");
	rock1.AddComponent<Model>(mesh);

	//Material
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.castsShadows = true;
	material.ignoreLighting = false;
	material.ignoreFog = false;
	material.metallic = 0;
	material.roughness = 0;
	material.diffuseTexture = TextureManager::Get()->GetTextureByName("ressources/textures/tree_5.jpg");
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

void PrefabFactor::CreateSphere(const glm::vec3 pos)
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

void PrefabFactor::CreateCube(const glm::vec3 pos)
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

void PrefabFactor::CreatePlane(const glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto plane = dm::EntityHandle(entityHandle);

	//Transform
	auto t1 = plane.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(2, 2, 2);

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
	material.diffuseTexture = TextureManager::Get()->GetTextureByName("ressources/textures/grass/grass1-albedo3.png");
	plane.AddComponent<MaterialDefault>(material);

	//Bounding sphere
	plane.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh.model));

	//Drawable
	plane.CreateComponent<dm::Drawable>(ComponentType::DRAWABLE);

	//MeshRenderer
	plane.CreateComponent<dm::MeshRenderer>(ComponentType::MESH_RENDERER);

	//Shadow Renderer
	plane.CreateComponent<dm::ShadowRenderer>(ComponentType::SHADOW_RENDERER);
}

void PrefabFactor::CreatePointLight(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto e1 = entityManager->CreateEntity();
	auto light = dm::EntityHandle(e1);

	//Transform
	auto transform = light.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	transform->position = pos;
	
	//Point light
	dm::PointLight lightComponent;
	lightComponent.componentType = ComponentType::POINT_LIGHT;
	light.AddComponent(lightComponent);
}

void PrefabFactor::CreateSpotLight(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto e3 = entityManager->CreateEntity();
	auto spot = dm::EntityHandle(e3);

	//Transform
	auto transform = spot.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	transform->position = pos;

	//Spot light
	dm::SpotLight spotLightComponent;
	spotLightComponent.target = glm::vec3(0, -10, 0);
	spotLightComponent.componentType = ComponentType::SPOT_LIGHT;
	spot.AddComponent(spotLightComponent);
}

void PrefabFactor::CreateTerrain(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto terrain = dm::EntityHandle(entityHandle);

	//Transform
	auto t1 = terrain.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(1, 1, 1);

	//Mesh
	dm::Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelPlane");
	terrain.AddComponent<dm::Model>(mesh);

	//Material
	auto material = MaterialTerrain();
	material.componentType = ComponentType::MATERIAL_TERRAIN;
	//material.noiseMap = Image2d::CreateNoiseTexture(glm::vec2(256, 256));
	material.noiseMap = TextureManager::Get()->GetTextureByName("ressources/textures/heightmap.jpg");
	material.grassSampler = TextureManager::Get()->GetTextureByName("ressources/textures/grass/grass1-albedo3.png");
	terrain.AddComponent<MaterialTerrain>(material);

	//Bounding sphere
	terrain.AddComponent<BoundingSphere>(BoundingSphereManager::GetBoundingSphere(*mesh.model));

	//Drawable
	terrain.CreateComponent<Drawable>(ComponentType::DRAWABLE);

	//MeshRenderer
	terrain.CreateComponent<MeshRenderer>(ComponentType::MESH_RENDERER);

	//Shadow Renderer
	terrain.CreateComponent<ShadowRenderer>(ComponentType::SHADOW_RENDERER);
}

void PrefabFactor::CreateTerrainPrefab(glm::vec3 pos)
{
	auto entityManager = Engine::Get()->GetEntityManager();
	const auto entityHandle = entityManager->CreateEntity();
	auto rock1 = EntityHandle(entityHandle);

	//Transform
	auto t1 = rock1.CreateComponent<Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(0.1, 0.1, 0.1);

	//Mesh
	Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = Engine::Get()->GetModelManager()->GetModel("ressources/models/mountain.obj");
	rock1.AddComponent<Model>(mesh);

	//Material
	auto material = MaterialDefault();
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.castsShadows = true;
	material.ignoreLighting = false;
	material.ignoreFog = false;
	material.metallic = 0;
	material.roughness = 0;
	material.diffuseTexture = TextureManager::Get()->GetTextureByName("ressources/textures/grass/grass1-albedo3.png");
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

void PrefabFactor::CreateSpherePBR(const glm::vec3 pos)
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
	auto material = MaterialMetalRoughness();
	material.componentType = ComponentType::MATERIAL_METAL_ROUGHNESS;
	material.castsShadows = true;
	material.ignoreLighting = false;
	material.ignoreFog = false;
	material.metallic = 0;
	material.roughness = 0;
	material.diffuseTexture = TextureManager::Get()->GetTextureByName("ressources/textures/PBR/copper-rock1-alb.png");
	material.normalTexture = TextureManager::Get()->GetTextureByName("ressources/textures/PBR/copper-rock1-normal.png");
	material.metalTexture = TextureManager::Get()->GetTextureByName("ressources/textures/PBR/copper-rock1-metal.png");
	material.roughnessTexture = TextureManager::Get()->GetTextureByName("ressources/textures/PBR/copper-rock1-rough.png");
	sphere.AddComponent<MaterialMetalRoughness>(material);

	//Bounding sphere
	sphere.AddComponent<BoundingSphere>(BoundingSphereManager::GetBoundingSphere(*mesh.model));

	//Drawable
	sphere.CreateComponent<Drawable>(ComponentType::DRAWABLE);

	//MeshRenderer
	sphere.CreateComponent<MeshRenderer>(ComponentType::MESH_RENDERER);

	//Shadow Renderer
	sphere.CreateComponent<ShadowRenderer>(ComponentType::SHADOW_RENDERER);
}
}
