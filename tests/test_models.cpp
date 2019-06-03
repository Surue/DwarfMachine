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
#include "graphics/image_cube.h"
#include "component/materials/material_skybox.h"
#include "component/mesh_renderer.h"
#include "component/lights/point_light.h"
#include "component/lights/spot_light.h"

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
	cameraInfo.isMain = true;
	cameraInfo.viewMatrix = glm::lookAt(cameraInfo.position, cameraInfo.position + cameraInfo.front, cameraInfo.up);
	cameraInfo.projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	//Cube
	const auto e1 = entityManager->CreateEntity();
	auto cube = dm::EntityHandle(e1);
	auto t1 = cube.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t1->position = glm::vec3(0, 0, 0);
	t1->scale = glm::vec3(1, 1, 1);
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
	t2->scale = glm::vec3(1, 1, 1);
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
	t3->scale = glm::vec3(1, 1, 1);
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
	t1->scale = glm::vec3(1, 1, 1);

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

	//Drawable
	cube.CreateComponent<dm::Drawable>(ComponentType::DRAWABLE);

	//MeshRenderer
	cube.CreateComponent<dm::MeshRenderer>(ComponentType::MESH_RENDERER);

	//Shadow Renderer
	cube.CreateComponent<dm::ShadowRenderer>(ComponentType::SHADOW_RENDERER);
}

dm::EntityHandle CreateSphere(glm::vec3 pos, dm::EntityManager* entityManager, dm::MaterialDefault material)
{
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
	sphere.AddComponent<dm::MaterialDefault>(material);

	//Bounding sphere
	sphere.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh.model));

	//Drawable
	sphere.CreateComponent<dm::Drawable>(ComponentType::DRAWABLE);

	//MeshRenderer
	sphere.CreateComponent<dm::MeshRenderer>(ComponentType::MESH_RENDERER);

	//Shadow Renderer
	sphere.CreateComponent<dm::ShadowRenderer>(ComponentType::SHADOW_RENDERER);

	return entityHandle;
}

void CreatePlane(glm::vec3 pos, dm::EntityManager* entityManager, dm::MaterialDefault material)
{
	const auto entityHandle = entityManager->CreateEntity();
	auto sphere = dm::EntityHandle(entityHandle);

	//Transform
	auto t1 = sphere.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t1->position = pos;
	t1->scale = glm::vec3(20, 1, 20);

	//Mesh
	dm::Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelCube");
	sphere.AddComponent<dm::Model>(mesh);

	//Material
	sphere.AddComponent<dm::MaterialDefault>(material);

	//Bounding sphere
	sphere.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh.model));

	//Drawable
	sphere.CreateComponent<dm::Drawable>(ComponentType::DRAWABLE);

	//MeshRenderer
	sphere.CreateComponent<dm::MeshRenderer>(ComponentType::MESH_RENDERER);

	//Shadow Renderer
	sphere.CreateComponent<dm::ShadowRenderer>(ComponentType::SHADOW_RENDERER);
}

void CreateTerrain(glm::vec3 pos, dm::EntityManager* entityManager, dm::MaterialDefault material)
{
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
	terrain.AddComponent<dm::MaterialDefault>(material);

	//Bounding sphere
	terrain.AddComponent<dm::BoundingSphere>(dm::BoundingSphereManager::GetBoundingSphere(*mesh.model));

	//Drawable
	terrain.CreateComponent<dm::Drawable>(ComponentType::DRAWABLE);

	//MeshRenderer
	terrain.CreateComponent<dm::MeshRenderer>(ComponentType::MESH_RENDERER);

	//Shadow Renderer
	terrain.CreateComponent<dm::ShadowRenderer>(ComponentType::SHADOW_RENDERER);
}

void CreateSkybox(dm::EntityManager* entityManager)
{
	const auto entityHandle = entityManager->CreateEntity();
	auto skybox = dm::EntityHandle(entityHandle);

	//Transform
	auto t1 = skybox.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	t1->position = glm::vec3(0.0f);
	t1->scale = glm::vec3(90.0f);

	//Mesh
	dm::Model mesh;
	mesh.componentType = ComponentType::MODEL;
	mesh.model = dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere");
	skybox.AddComponent<dm::Model>(mesh);

	//Skybox material
	auto image = dm::ImageCube::Create("ressources/textures/skybox", ".png", VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, true, true);
	auto material = dm::MaterialSkybox();
	material.image = image;
	material.color = dm::Color::White;
	material.componentType = ComponentType::MATERIAL_SKYBOX;

	//Material
	skybox.AddComponent<dm::MaterialSkybox>(material);

	//Drawable
	dm::Drawable drawable;
	drawable.componentType = ComponentType::DRAWABLE;
	drawable.isDrawable = true;
	skybox.AddComponent<dm::Drawable>(drawable);

	//MeshRenderer
	dm::MeshRenderer meshRenderer;
	meshRenderer.materialType = dm::MeshRenderer::MaterialType::SKYBOX;
	meshRenderer.componentType = ComponentType::MESH_RENDERER;
	skybox.AddComponent<dm::MeshRenderer>(meshRenderer);
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
	cameraInfo.isMain = true;
	cameraInfo.isCulling = true;
	cameraInfo.viewMatrix = glm::lookAt(cameraInfo.position, cameraInfo.position + cameraInfo.front, cameraInfo.up);
	cameraInfo.fov = 45;
	cameraInfo.farFrustum = 1024.0f;
	cameraInfo.nearFrustum = 0.1f;
	cameraInfo.aspect = 800.0f / 600.0f;
	cameraInfo.projectionMatrix = glm::perspective(glm::radians(cameraInfo.fov), cameraInfo.aspect, cameraInfo.nearFrustum, cameraInfo.farFrustum);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	std::shared_ptr<dm::GizmoType> gizmoType = dm::GizmoType::Create(dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere"), 1, dm::Color::White);

	//Skybox
	CreateSkybox(entityManager);

	//Cube
	float maxCube = 1;

	dm::MaterialDefault material;
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.color = dm::Color(0.85f, 0.85f, 0.85f, 1);

	for(size_t i = 0; i < maxCube; i++)
	{
		for(size_t j = 0; j < maxCube; j++)
		{
			glm::vec3 pos = glm::vec3(i - maxCube / 2.0f + i * 1.0f, 0, j - maxCube / 2.0f + j * 1.0f);
			CreateCube(pos, entityManager, editor, gizmoType, material);		
		}
	}

	//PointLight

	const auto e1 = entityManager->CreateEntity();
	auto light = dm::EntityHandle(e1);
	light.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	dm::PointLight lightComponent;
	lightComponent.componentType = ComponentType::POINT_LIGHT;
	light.AddComponent(lightComponent);

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}

TEST(Models, PBR)
{
	dm::EngineSettings settings;
	settings.windowSize = dm::Vec2i(1024, 720);
	dm::Engine engine = dm::Engine(settings);
	engine.Init();

	engine.SetApplication(new dm::Editor());

	auto editor = static_cast<dm::Editor*>(engine.GetApplication());

	auto entityManager = engine.GetEntityManager();

	//Camera
	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	dm::Camera cameraInfo;
	cameraInfo.up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMain = true;
	cameraInfo.isCulling = true;
	cameraInfo.viewMatrix = glm::lookAt(cameraInfo.position, cameraInfo.position + cameraInfo.front, cameraInfo.up);
	cameraInfo.fov = 45;
	cameraInfo.farFrustum = 1024.0f;
	cameraInfo.nearFrustum = 0.1f;
	cameraInfo.aspect = 1024.0f / 720.0f;
	cameraInfo.projectionMatrix = glm::perspective(glm::radians(cameraInfo.fov), cameraInfo.aspect, cameraInfo.nearFrustum, cameraInfo.farFrustum);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	std::shared_ptr<dm::GizmoType> gizmoType = dm::GizmoType::Create(dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere"), 1, dm::Color::White);

	//Skybox
	CreateSkybox(entityManager);

	//Sphere
	float maxSphere = 9;

	for (size_t i = 0; i <= maxSphere; i++)
	{
		for (size_t j = 0; j <= maxSphere; j++)
		{
			dm::MaterialDefault material;
			material.componentType = ComponentType::MATERIAL_DEFAULT;
			material.color = dm::Color(1, 1, 1, 1);
			material.roughness = i / maxSphere;
			material.metallic = j / maxSphere;
			material.ignoreLighting = false;
			material.ignoreFog = false;

			glm::vec3 pos = glm::vec3(i - maxSphere / 2.0f + i * 1.0f, 0, j - maxSphere / 2.0f + j * 1.0f);
			CreateSphere(pos, entityManager, material);
		}
	}

	//PointLight
	const auto e1 = entityManager->CreateEntity();
	auto light = dm::EntityHandle(e1);
	light.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	dm::PointLight lightComponent;
	lightComponent.componentType = ComponentType::POINT_LIGHT;
	light.AddComponent(lightComponent);

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}

TEST(Models, Lights)
{
	dm::EngineSettings settings;
	settings.windowSize = dm::Vec2i(1024, 720);
	dm::Engine engine = dm::Engine(settings);
	engine.Init();

	engine.SetApplication(new dm::Editor());

	auto editor = static_cast<dm::Editor*>(engine.GetApplication());

	auto entityManager = engine.GetEntityManager();

	//Camera
	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	dm::Camera cameraInfo;
	cameraInfo.up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMain = true;
	cameraInfo.isCulling = true;
	cameraInfo.viewMatrix = glm::lookAt(cameraInfo.position, cameraInfo.position + cameraInfo.front, cameraInfo.up);
	cameraInfo.fov = 45;
	cameraInfo.farFrustum = 100.0f;
	cameraInfo.nearFrustum = 0.1f;
	cameraInfo.aspect = 1024.0f / 720.0f;
	cameraInfo.projectionMatrix = glm::perspective(glm::radians(cameraInfo.fov), cameraInfo.aspect, cameraInfo.nearFrustum, cameraInfo.farFrustum);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	std::shared_ptr<dm::GizmoType> gizmoType = dm::GizmoType::Create(dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere"), 1, dm::Color::White);

	//Skybox
	CreateSkybox(entityManager);

	//Cube
	dm::MaterialDefault materialS;
	materialS.componentType = ComponentType::MATERIAL_DEFAULT;
	materialS.color = dm::Color(1, 1, 1, 1);
	materialS.ignoreLighting = false;
	materialS.ignoreFog = false;
	CreateSphere(camera->position, entityManager, materialS);

	//Sphere
	float maxSphere = 9;

	for (size_t i = 0; i <= maxSphere; i++)
	{
		for (size_t j = 0; j <= maxSphere; j++)
		{
			dm::MaterialDefault material;
			material.componentType = ComponentType::MATERIAL_DEFAULT;
			material.color = dm::Color(1, 1, 1, 1);
			material.roughness = i / maxSphere;
			material.metallic = j / maxSphere;
			material.ignoreLighting = false;
			material.ignoreFog = false;

			glm::vec3 pos = glm::vec3(i - maxSphere / 2.0f + i * 1.0f, 0, j - maxSphere / 2.0f + j * 1.0f);
			auto sphere = CreateSphere(pos, entityManager, material);

			//dm::Gizmo gizmo;
			//gizmo.transform = sphere.GetComponent<dm::Transform>(ComponentType::TRANSFORM);
			//gizmo.color = dm::Color(100, 0, 0, 1);
			//gizmo.gizmoType = gizmoType;
			//gizmo.entity = sphere.GetEntity();
			//editor->GetGizmoManager()->AddGizmo(gizmo);
		}
	}

	//Plane
	dm::MaterialDefault material;
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.color = dm::Color(1, 1, 1, 1);
	material.ignoreLighting = false;
	material.ignoreFog = false;
	CreatePlane(glm::vec3(3.8, -1.5, 4.5), entityManager, material);

	//PointLight
	const auto e1 = entityManager->CreateEntity();
	auto light = dm::EntityHandle(e1);
	light.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	dm::PointLight lightComponent;
	lightComponent.componentType = ComponentType::POINT_LIGHT;
	light.AddComponent(lightComponent);

	//DirectionalLight
	const auto e2 = entityManager->CreateEntity();
	auto sun = dm::EntityHandle(e2);
	dm::DirectionalLight sunLightComponent;
	sunLightComponent.componentType = ComponentType::DIRECTIONAL_LIGHT;
	sun.AddComponent(sunLightComponent);

	//SpotLight
	const auto e3 = entityManager->CreateEntity();
	auto spot = dm::EntityHandle(e3);
	auto spotPos = spot.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	spotPos->position = glm::vec3(0, 10, 0);
	dm::SpotLight spotLightComponent;
	spotLightComponent.target = glm::vec3(0, 0, 0);
	spotLightComponent.componentType = ComponentType::SPOT_LIGHT;
	spot.AddComponent(spotLightComponent);

	//

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}

TEST(Models, Scene)
{
	dm::EngineSettings settings;
	settings.windowSize = dm::Vec2i(1024, 720);
	dm::Engine engine = dm::Engine(settings);
	engine.Init();

	engine.SetApplication(new dm::Editor());

	engine.LoadScene("../ressources/scenes/newScene.scene");

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}

TEST(Models, Terrain)
{
	dm::EngineSettings settings;
	settings.windowSize = dm::Vec2i(1024, 720);
	dm::Engine engine = dm::Engine(settings);
	engine.Init();

	engine.SetApplication(new dm::Editor());

	auto entityManager = engine.GetEntityManager();

	//Camera
	const auto e0 = entityManager->CreateEntity();
	auto entity = dm::EntityHandle(e0);

	dm::Camera cameraInfo;
	cameraInfo.up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraInfo.componentType = ComponentType::CAMERA;
	cameraInfo.isMain = true;
	cameraInfo.isCulling = true;
	cameraInfo.viewMatrix = glm::lookAt(cameraInfo.position, cameraInfo.position + cameraInfo.front, cameraInfo.up);
	cameraInfo.fov = 45;
	cameraInfo.farFrustum = 100.0f;
	cameraInfo.nearFrustum = 0.1f;
	cameraInfo.aspect = 1024.0f / 720.0f;
	cameraInfo.projectionMatrix = glm::perspective(glm::radians(cameraInfo.fov), cameraInfo.aspect, cameraInfo.nearFrustum, cameraInfo.farFrustum);

	auto camera = entity.AddComponent<dm::Camera>(cameraInfo);

	std::shared_ptr<dm::GizmoType> gizmoType = dm::GizmoType::Create(dm::Engine::Get()->GetModelManager()->GetModel("ModelSphere"), 1, dm::Color::White);

	//Skybox
	CreateSkybox(entityManager);

	//Plane
	dm::MaterialDefault material;
	material.componentType = ComponentType::MATERIAL_DEFAULT;
	material.color = dm::Color(1, 1, 1, 1);
	material.ignoreLighting = false;
	material.ignoreFog = false;
	CreateTerrain(glm::vec3(0, 0, 0), entityManager, material);

	//PointLight
	const auto e1 = entityManager->CreateEntity();
	auto light = dm::EntityHandle(e1);
	light.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	dm::PointLight lightComponent;
	lightComponent.componentType = ComponentType::POINT_LIGHT;
	light.AddComponent(lightComponent);

	//DirectionalLight
	const auto e2 = entityManager->CreateEntity();
	auto sun = dm::EntityHandle(e2);
	dm::DirectionalLight sunLightComponent;
	sunLightComponent.componentType = ComponentType::DIRECTIONAL_LIGHT;
	sun.AddComponent(sunLightComponent);

	//SpotLight
	const auto e3 = entityManager->CreateEntity();
	auto spot = dm::EntityHandle(e3);
	auto spotPos = spot.CreateComponent<dm::Transform>(ComponentType::TRANSFORM);
	spotPos->position = glm::vec3(0, 10, 0);
	dm::SpotLight spotLightComponent;
	spotLightComponent.target = glm::vec3(0, 0, 0);
	spotLightComponent.componentType = ComponentType::SPOT_LIGHT;
	spot.AddComponent(spotLightComponent);

	try
	{
		engine.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}