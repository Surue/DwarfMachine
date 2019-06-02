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

#include <graphics/renderer_deferred.h>
#include <graphics/graphic_manager.h>
#include "component/materials/material_skybox.h"
#include "engine/engine.h"
#include <component/component_manager.h>
#include "graphics/pipelines/pipeline_compute.h"
#include "entity/entity_handle.h"
#include "component/lights/spot_light.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace dm
{
static const uint32_t MAX_LIGHTS = 32;

RendererDeferred::RendererDeferred(const Pipeline::Stage &pipelineStage) :
	RenderPipeline(pipelineStage),
	m_Pipeline(pipelineStage, { "Shaders/deferred.vert", "Shaders/deferred.frag" }, {}, GetDefines(), PipelineGraphics::Mode::POLYGON, PipelineGraphics::Depth::NONE) ,
	m_Skybox(nullptr)
{
	
	//TODO créer un worker pour le brdf et tout ce qui suit
	m_CurrentBRDF = ComputeBRDF(512);
}
void RendererDeferred::Update() {}

void RendererDeferred::Draw(const CommandBuffer& commandBuffer)
{
	auto camera = GraphicManager::Get()->GetCamera();

	if(m_Skybox == nullptr)
	{
		auto entities = Engine::Get()->GetEntityManager()->GetEntities();

		for (auto entity : entities)
		{
			if (entity == INVALID_ENTITY)
			{
				break;
			}

			auto entityHandle = EntityHandle(entity);

			if (entityHandle.HasComponent(ComponentType::MATERIAL_SKYBOX))
			{
				m_Skybox = entityHandle.GetComponent<MaterialSkybox>(ComponentType::MATERIAL_SKYBOX)->image;

				m_CurrentPrefiltered = ComputePrefiltered(m_Skybox, 512);
				m_CurrentIrradiance = ComputeIrradiance(m_Skybox, 64);

				break;
			}
		}
	}

	std::vector<DeferredPointLight> deferredPointLights(MAX_LIGHTS);
	std::vector<DeferredSpotLight> deferredSpotLights(MAX_LIGHTS);
	uint32_t pointLightCount = 0;
	uint32_t spotLightCount = 0;

	glm::vec3 directionalDirection;
	glm::vec4 directionalColor;

	for(auto entity : Engine::Get()->GetEntityManager()->GetEntities())
	{
		auto entityHandle = EntityHandle(entity);

		if (entity == INVALID_ENTITY)
		{
			break;
		}

		//Point lights
		if (entityHandle.HasComponent(ComponentType::POINT_LIGHT))
		{
			const auto light = entityHandle.GetComponent<PointLight>(ComponentType::POINT_LIGHT);
			const auto transform = entityHandle.GetComponent<Transform>(ComponentType::TRANSFORM);

			DeferredPointLight deferredLight = {};
			deferredLight.color = light->color * light->intensity;
			deferredLight.radius = light->radius;
			deferredLight.position = transform->position;

			deferredPointLights[pointLightCount] = deferredLight;
			pointLightCount++;

			if (pointLightCount + spotLightCount >= MAX_LIGHTS)
			{
				break;
			}
		}

		//Directional
		if (entityHandle.HasComponent(ComponentType::DIRECTIONAL_LIGHT))
		{
			const auto light = entityHandle.GetComponent<DirectionalLight>(ComponentType::DIRECTIONAL_LIGHT);

			directionalDirection = light->direction;
			directionalColor = glm::vec4(light->color.r, light->color.g, light->color.b, light->color.a) * light->intensity;
		}

		//Spot lights
		if (entityHandle.HasComponent(ComponentType::SPOT_LIGHT))
		{
			const auto light = entityHandle.GetComponent<SpotLight>(ComponentType::SPOT_LIGHT);
			const auto transform = entityHandle.GetComponent<Transform>(ComponentType::TRANSFORM);

			DeferredSpotLight deferredLight = {};
			deferredLight.color = light->color * light->intensity;
			deferredLight.target = light->target;
			deferredLight.range = light->range;
			deferredLight.position = transform->position;
			deferredLight.angle = light->angle;

			deferredSpotLights[spotLightCount] = deferredLight;
			spotLightCount++;

			if (pointLightCount + spotLightCount >= MAX_LIGHTS)
			{
				break;
			}
		}
	}


	//Compute lightSpaceMatrix
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

	glm::vec3 lightDir = directionalDirection;

	glm::mat4 lightView;
	if (directionalDirection == camera->up)
	{
		lightView = glm::mat4(glm::vec4(1.0f), glm::vec4(0), glm::vec4(0), glm::vec4(0));
	}
	else
	{
		lightView = glm::lookAt(glm::normalize(-directionalDirection) * -minExtents.z, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	}

	glm::mat4 lightProjection = glm::ortho<float>(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, -(maxExtents.z - minExtents.z), maxExtents.z - minExtents.z);

	m_UniformScene.Push("view", camera->viewMatrix);
	m_UniformScene.Push("cameraPosition", camera->position);
	m_UniformScene.Push("pointLightCount", pointLightCount);
	m_UniformScene.Push("spotLightCount", spotLightCount);
	//TODO créer un objet fog
	m_UniformScene.Push("fogColor", Color::White);
	m_UniformScene.Push("fogDensity", 0.001f);
	m_UniformScene.Push("fogGradient", 2.0f);
	m_UniformScene.Push("directionalLightDirection", directionalDirection);
	m_UniformScene.Push("directionalLightColor", directionalColor);
	m_UniformScene.Push("directionalLightViewMatrix", lightProjection * lightView);

	//Update storage buffer
	if (pointLightCount > 0)
	{
		m_StoragePointLight.Push(deferredPointLights.data(), sizeof(DeferredPointLight) * MAX_LIGHTS);
	}

	if (spotLightCount > 0)
	{
		m_StorageSpotLight.Push(deferredSpotLights.data(), sizeof(DeferredSpotLight) * MAX_LIGHTS);
	}

	//Update descriptor set
	m_DescriptorSet.Push("UniformScene", m_UniformScene);
	if (pointLightCount > 0)
	{
		m_DescriptorSet.Push("BufferPointLights", m_StoragePointLight);
	}
	if (spotLightCount > 0) {
		m_DescriptorSet.Push("BufferSpotLights", m_StorageSpotLight);
	}
	m_DescriptorSet.Push("samplerPosition", GraphicManager::Get()->GetAttachment("position"));
	m_DescriptorSet.Push("samplerDiffuse", GraphicManager::Get()->GetAttachment("diffuse"));
	m_DescriptorSet.Push("samplerNormal", GraphicManager::Get()->GetAttachment("normal"));
	m_DescriptorSet.Push("samplerMaterial", GraphicManager::Get()->GetAttachment("material"));
	m_DescriptorSet.Push("samplerSsao", GraphicManager::Get()->GetAttachment("ssao"));
	m_DescriptorSet.Push("shadowMap", GraphicManager::Get()->GetAttachment("shadow"));

	if(m_FutureBRDF.valid())
	{
		m_CurrentBRDF = m_FutureBRDF.get();
	}
	m_DescriptorSet.Push("samplerBRDF", *m_CurrentBRDF);

	if (m_FutureIrradiance.valid())
	{
		m_CurrentIrradiance = m_FutureIrradiance.get();
	}
	m_DescriptorSet.Push("samplerIrradiance", *m_CurrentIrradiance);

	if (m_FuturePrefiltered.valid())
	{
		m_CurrentPrefiltered = m_FuturePrefiltered.get();
	}
	m_DescriptorSet.Push("samplerPrefiltered", *m_CurrentPrefiltered);

	const auto updateSuccess = m_DescriptorSet.Update(m_Pipeline);

	if(!updateSuccess)
	{
		return;
	}

	m_Pipeline.BindPipeline(commandBuffer);

	m_DescriptorSet.BindDescriptor(commandBuffer, m_Pipeline);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

std::unique_ptr<Image2d> RendererDeferred::ComputeBRDF(const uint32_t& size)
{
	auto brdfImage = std::make_unique<Image2d>(size, size, nullptr, VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_LAYOUT_GENERAL);

	CommandBuffer commandBuffer = CommandBuffer(true, VK_QUEUE_COMPUTE_BIT);
	PipelineCompute computePipeline = PipelineCompute("Shaders/brdf.comp");

	computePipeline.BindPipeline(commandBuffer);

	DescriptorHandle descriptorSet = DescriptorHandle(computePipeline);
	descriptorSet.Push("outColor", brdfImage.get());
	descriptorSet.Update(computePipeline);

	descriptorSet.BindDescriptor(commandBuffer, computePipeline);
	glm::vec2 extent = glm::vec2(brdfImage->GetWidth(), brdfImage->GetHeight());
	computePipeline.Draw(commandBuffer, extent);
	commandBuffer.SubmitIdle();

	return brdfImage;
}

std::unique_ptr<ImageCube> RendererDeferred::ComputeIrradiance(const std::shared_ptr<ImageCube>& source,
	const uint32_t& size)
{
	if(source == nullptr)
	{
		return nullptr;
	}

	auto irradianceCubemap = std::make_unique<ImageCube>(size, size, nullptr, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_GENERAL);

	CommandBuffer commandBuffer = CommandBuffer(true, VK_QUEUE_COMPUTE_BIT);
	PipelineCompute computePipeline = PipelineCompute("Shaders/irradiance.comp");

	computePipeline.BindPipeline(commandBuffer);

	DescriptorHandle descriptorSet = DescriptorHandle(computePipeline);
	descriptorSet.Push("outColor", irradianceCubemap.get());
	descriptorSet.Push("samplerColor", source);
	descriptorSet.Update(computePipeline);

	descriptorSet.BindDescriptor(commandBuffer, computePipeline);
	glm::vec2 extent = glm::vec2(irradianceCubemap->GetWidth(), irradianceCubemap->GetHeight());
	computePipeline.Draw(commandBuffer, extent);
	commandBuffer.SubmitIdle();

	return irradianceCubemap;
}

std::unique_ptr<ImageCube> RendererDeferred::ComputePrefiltered(const std::shared_ptr<ImageCube>& source,
	const uint32_t& size)
{
	if(source == nullptr)
	{
		return nullptr;
	}

	auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	auto prefilteredCubemap = std::make_unique<ImageCube>(size, size, nullptr, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLE_COUNT_1_BIT, true, true);

	CommandBuffer commandBuffer = CommandBuffer(true, VK_QUEUE_COMPUTE_BIT);
	PipelineCompute computePipeline = PipelineCompute("Shaders/prefiltered.comp");

	DescriptorHandle descriptorSet = DescriptorHandle(computePipeline);
	PushHandle pushHandle = PushHandle(*computePipeline.GetShader()->GetUniformBlock("PushObject"));

	for(uint32_t i = 0; i < prefilteredCubemap->GetMipLevels(); i++)
	{
		VkImageView levelView = VK_NULL_HANDLE;
		Image::CreateImageView(prefilteredCubemap->GetImage(), levelView, VK_IMAGE_VIEW_TYPE_CUBE, prefilteredCubemap->GetFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1, i, 6, 0);

		commandBuffer.Begin();
		computePipeline.BindPipeline(commandBuffer);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.sampler = prefilteredCubemap->GetSampler();
		imageInfo.imageView = levelView;
		imageInfo.imageLayout = prefilteredCubemap->GetLayout();

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = VK_NULL_HANDLE;
		descriptorWrite.dstBinding = *computePipeline.GetShader()->GetDescriptorLocation("outPrefiltered");
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = *computePipeline.GetShader()->GetDescriptorType(descriptorWrite.dstBinding);

		auto writeDescriptorSet = WriteDescriptorSet(descriptorWrite, imageInfo);

		pushHandle.Push("roughness", static_cast<float>(i) / static_cast<float>(prefilteredCubemap->GetMipLevels() - 1));

		descriptorSet.Push("PushObject", pushHandle);
		descriptorSet.Push("outColor", prefilteredCubemap.get(), std::move(writeDescriptorSet));
		descriptorSet.Push("samplerColor", source);
		descriptorSet.Update(computePipeline);

		descriptorSet.BindDescriptor(commandBuffer, computePipeline);
		pushHandle.BindPush(commandBuffer, computePipeline);
		computePipeline.Draw(commandBuffer, glm::vec2(prefilteredCubemap->GetWidth() << 1));
		commandBuffer.SubmitIdle();

		vkDestroyImageView(*logicalDevice, levelView, nullptr);
	}

	return prefilteredCubemap;
}

std::vector<Shader::Define> RendererDeferred::GetDefines()
{
	std::vector<Shader::Define> defines;
	defines.emplace_back("MAX_LIGHTS", std::to_string(MAX_LIGHTS));
	return defines;
}
}
