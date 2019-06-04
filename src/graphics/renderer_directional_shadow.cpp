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

#include <graphics/renderer_directional_shadow.h>
#include "component/model.h"
#include "graphics/buffers/uniform_handle.h"
#include "engine/engine.h"
#include "entity/entity_handle.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <graphics/graphic_manager.h>

namespace dm
{
RendererDirectionalShadow::RendererDirectionalShadow(const Pipeline::Stage& stage): 
	RenderPipeline(stage),
	m_Pipeline(stage, { "Shaders/shadow_directional.vert", "Shaders/shadow_directional.frag" }, { VertexMesh::GetVertexInput() }, {}, PipelineGraphics::Mode::MRT, PipelineGraphics::Depth::READ_WRITE, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT)
{
	m_Signature.AddComponent(ComponentType::MESH_RENDERER);
	m_Signature.AddComponent(ComponentType::SHADOW_RENDERER);
	m_Signature.AddComponent(ComponentType::MODEL);
	m_Signature.AddComponent(ComponentType::TRANSFORM);
}

void RendererDirectionalShadow::Update()
{
}

void RendererDirectionalShadow::Draw(const CommandBuffer& commandBuffer)
{
	//Get Directional Light
	glm::vec3 lightDirection = glm::vec3(0, -1, 0);

	auto entities = Engine::Get()->GetEntityManager()->GetEntities();

	for (auto entity : entities)
	{
		if (entity == INVALID_ENTITY)
		{
			break;
		}

		auto entityHandle = EntityHandle(entity);

		if (entityHandle.HasComponent(ComponentType::DIRECTIONAL_LIGHT))
		{
			const auto light = entityHandle.GetComponent<DirectionalLight>(ComponentType::DIRECTIONAL_LIGHT);

			lightDirection = light->direction;
			break;
		}
	}

	//Compute lightSpaceMatrix
	const auto camera = GraphicManager::Get()->GetCamera();

	glm::vec3 frustumCorners[8] = {
	glm::vec3(-1.0f,  1.0f, -1.0f),
	glm::vec3(1.0f,  1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f,  1.0f,  1.0f),
	glm::vec3(1.0f,  1.0f,  1.0f),
	glm::vec3(1.0f, -1.0f,  1.0f),
	glm::vec3(-1.0f, -1.0f,  1.0f),
	};

	// Project frustum corners into world space
	glm::mat4 invCam = glm::inverse(camera->projectionMatrix * camera->viewMatrix);
	for (uint32_t i = 0; i < 8; i++) {
		glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
		frustumCorners[i] = invCorner / invCorner.w;
	}

	for (uint32_t i = 0; i < 4; i++) {
		glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
		frustumCorners[i + 4] = frustumCorners[i] + (dist);
		frustumCorners[i] = frustumCorners[i] + (dist);
	}

	// Get frustum center
	glm::vec3 frustumCenter = glm::vec3(0.0f);
	for (uint32_t i = 0; i < 8; i++) {
		frustumCenter += frustumCorners[i];
	}
	frustumCenter /= 8.0f;

	float radius = 0.0f;
	for (uint32_t i = 0; i < 8; i++) {
		float distance = glm::length(frustumCorners[i] - frustumCenter);
		radius = glm::max(radius, distance);
	}
	radius = std::ceil(radius * 16.0f) / 16.0f;

	glm::vec3 maxExtents = glm::vec3(radius);
	glm::vec3 minExtents = -maxExtents;

	glm::vec3 lightDir = lightDirection;
	
	glm::mat4 lightView;
	if(lightDirection == camera->up)
	{
		lightView = glm::mat4(glm::vec4(1.0f), glm::vec4(0), glm::vec4(0), glm::vec4(0));
	}else
	{
		lightView = glm::lookAt(glm::normalize(-lightDirection) * -minExtents.z, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	}

	glm::mat4 lightProjection = glm::ortho<float>(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, -(maxExtents.z - minExtents.z), maxExtents.z - minExtents.z);

	for (const auto &entity : m_RegisteredEntities)
	{
		auto entityHandle = EntityHandle(entity);

		auto transform = entityHandle.GetComponent<Transform>(ComponentType::TRANSFORM);
		auto mesh = entityHandle.GetComponent<Model>(ComponentType::MODEL);
		auto shadowRenderer = entityHandle.GetComponent<ShadowRenderer>(ComponentType::SHADOW_RENDERER);

		m_Pipeline.BindPipeline(commandBuffer);

		glm::mat4x4 matrix = lightProjection * lightView * TransformManager::GetWorldMatrix(*transform);

		shadowRenderer->uniformScene.Push("mvp", matrix);

		shadowRenderer->descriptorSet.Push("UniformScene", shadowRenderer->uniformScene);
		shadowRenderer->descriptorSet.Push("shadowMap", GraphicManager::Get()->GetAttachment("shadow"));

		const auto updateSuccess = shadowRenderer->descriptorSet.Update(m_Pipeline);


		if (!updateSuccess)
		{
			continue;
		}

		shadowRenderer->descriptorSet.BindDescriptor(commandBuffer, m_Pipeline);

		if(mesh->model->CmdRender(commandBuffer)){}
	}
}

void RendererDirectionalShadow::RegisterEntity(const Entity entity)
{
	m_RegisteredEntities.push_back(entity);
}
}
