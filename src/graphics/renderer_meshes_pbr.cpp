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

#include <graphics/renderer_meshes_pbr.h>
#include <graphics/graphic_manager.h>
#include "entity/entity_handle.h"
#include "component/model.h"
#include <component/materials/material_default.h>

#include <component/mesh_renderer.h>>

namespace dm
{
RendererMeshesPBR::RendererMeshesPBR(const Pipeline::Stage& pipelineStage) :
	RenderPipeline(pipelineStage),
	m_UniformScene(true)
{
	m_Signature.AddComponent(ComponentType::MESH_RENDERER);
	m_Signature.AddComponent(ComponentType::MODEL);
	m_Signature.AddComponent(ComponentType::TRANSFORM);
	m_Signature.AddComponent(ComponentType::DRAWABLE);
	m_Signature.AddComponent(ComponentType::MATERIAL_METAL_ROUGHNESS);
}

void RendererMeshesPBR::Update()
{
	for (const auto &meshRender : m_RegisteredEntities)
	{
		auto entity = EntityHandle(meshRender);
		const auto transform = entity.GetComponent<Transform>(ComponentType::TRANSFORM);
		const auto meshRenderer = entity.GetComponent<MeshRenderer>(ComponentType::MESH_RENDERER);

		const auto material = entity.GetComponent<MaterialMetalRoughness>(ComponentType::MATERIAL_METAL_ROUGHNESS);
		MaterialMetalRoughnessManager::PushUniform(*material, TransformManager::GetWorldMatrix(*transform), meshRenderer->uniformObject);
	}
}

void RendererMeshesPBR::Draw(const CommandBuffer& commandBuffer)
{
	const auto camera = GraphicManager::Get()->GetCamera();
	m_UniformScene.Push("projection", camera->projectionMatrix);
	m_UniformScene.Push("view", camera->viewMatrix);
	m_UniformScene.Push("cameraPos", camera->position);

	for (const auto &entity : m_RegisteredEntities)
	{
		auto entityHandle = EntityHandle(entity);
		const auto drawable = entityHandle.GetComponent<Drawable>(ComponentType::DRAWABLE);
		if (!drawable->isDrawable)
		{
			continue;
		}

		const auto meshRenderer = entityHandle.GetComponent<MeshRenderer>(ComponentType::MESH_RENDERER);
		const auto mesh = entityHandle.GetComponent<Model>(ComponentType::MODEL);

		MaterialMetalRoughness* material = entityHandle.GetComponent<MaterialMetalRoughness>(ComponentType::MATERIAL_METAL_ROUGHNESS);


		if (material == nullptr || mesh == nullptr)
		{
			std::cout << "Missing material or mesh => Don't render\n";
			continue;
		}

		const auto meshModel = mesh->model;
		auto materialPipeline = material->pipelineMaterial;

		if (meshModel == nullptr || materialPipeline == nullptr || materialPipeline->GetStage() != GetStage())
		{
			std::cout << "Missing model or material pipeline or stage is not the same : \n";
			if (meshModel == nullptr)
			{
				std::cout << "	- Missing model\n";
			}
			else if (materialPipeline == nullptr)
			{
				std::cout << "	- Missing materialPipeline\n";
			}
			else
			{
				std::cout << "	- MaterialPipeline->GetStage() != GetStage()\n";
			}
			continue;
		}

		const auto bindSuccess = materialPipeline->BindPipeline(commandBuffer);

		if (!bindSuccess)
		{
			std::cout << "Bind fail\n";
			continue;
		}

		auto &pipeline = *materialPipeline->GetPipeline();

		meshRenderer->descriptorSet.Push("UboScene", m_UniformScene);
		meshRenderer->descriptorSet.Push("UboObject", meshRenderer->uniformObject);

		MaterialMetalRoughnessManager::PushDescriptor(*material, meshRenderer->descriptorSet);

		const auto updateSuccess = meshRenderer->descriptorSet.Update(pipeline);


		if (!updateSuccess)
		{
			continue;
		}

		// Draws the object.
		meshRenderer->descriptorSet.BindDescriptor(commandBuffer, pipeline);
		if (meshModel->CmdRender(commandBuffer)) {

		}
	}
}

void RendererMeshesPBR::RegisterEntity(const Entity entity)
{
	m_RegisteredEntities.push_back(entity);

}
}
