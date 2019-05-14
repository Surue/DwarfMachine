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

#include <graphics/renderer_meshes.h>
#include <graphics/graphic_manager.h>
#include "entity/entity_handle.h"
#include "component/model.h"
#include <component/material_default.h>

#include <glm/gtx/string_cast.hpp>

namespace dm
{
RendererMeshes::RendererMeshes(Engine& engine, const Pipeline::Stage& pipelineStage) : 
	RenderPipeline(pipelineStage),
	m_UniformScene(true)
{
	m_Signature.AddComponent(ComponentType::MATERIAL_DEFAULT);
	m_Signature.AddComponent(ComponentType::MODEL);
	m_Signature.AddComponent(ComponentType::TRANSFORM);
}

void RendererMeshes::Update()
{
	int i = 0;
	for (const auto &meshRender : m_RegisteredEntities)
	{
		auto entity = EntityHandle(meshRender);
		const auto material = entity.GetComponent<MaterialDefault>(ComponentType::MATERIAL_DEFAULT);
		const auto transform = entity.GetComponent<Transform>(ComponentType::TRANSFORM);

		transform->position.y *= 1;
		MaterialDefaultManager::PushUniform(*material, TransformManager::GetWorldMatrix(*transform));
		transform->position.y *= 1;
		i++;
	}
}

void RendererMeshes::Draw(const CommandBuffer& commandBuffer)
{
	auto camera = GraphicManager::Get()->GetCamera();
	m_UniformScene.Push("projection", camera->proj);
	m_UniformScene.Push("view", camera->viewMatrix);
	m_UniformScene.Push("cameraPos", camera->cameraPos); //TODO la position de la caméra ne passe pas

	auto i = 0;
	for (const auto &meshRender : m_RegisteredEntities)
	{

		auto entity = EntityHandle(meshRender);
		const auto material = entity.GetComponent<MaterialDefault>(ComponentType::MATERIAL_DEFAULT);
		const auto mesh = entity.GetComponent<Model>(ComponentType::MODEL);

		if (material == nullptr || mesh == nullptr)
		{
			std::cout << "Missing material or mesh => Don't render\n";
			return;
		}

		auto meshModel = mesh->model;
		auto materialPipeline = material->pipelineMaterial;

		if (meshModel == nullptr || materialPipeline == nullptr || materialPipeline->GetStage() != GetStage())
		{
			std::cout << "Missing model or material pipeline or stage is not the same : \n";
			if(meshModel == nullptr)
			{
				std::cout << "	- Missing model\n";
			}

			if (materialPipeline == nullptr)
			{
				std::cout << "	- Missing materialPipeline\n";
			}

			if (materialPipeline->GetStage() != GetStage())
			{
				std::cout << "	- MaterialPipeline->GetStage() != GetStage()\n";
			}

			return;
		}

		const auto bindSuccess = materialPipeline->BindPipeline(commandBuffer);

		if (!bindSuccess)
		{
			std::cout << "Bind fail\n";
			return;
		}

		auto &pipeline = *materialPipeline->GetPipeline();

		material->descriptorSet.Push("UboScene", m_UniformScene);
		material->descriptorSet.Push("UboObject", material->uniformObject);

		MaterialDefaultManager::PushDescriptor(*material);

		const auto updateSuccess = material->descriptorSet.Update(pipeline);

		if (!updateSuccess)
		{
			return;
		}

		// Draws the object.
		material->descriptorSet.BindDescriptor(commandBuffer, pipeline);
		if (meshModel->CmdRender(commandBuffer)) {
			//std::cout << "Draw success\n";
		}

		i++;
	}
}

void RendererMeshes::RegisterEntity(const Entity entity)
{
	m_RegisteredEntities.push_back(entity);

	auto entityHandle = EntityHandle(entity);
	auto material = entityHandle.GetComponent<MaterialDefault>(ComponentType::MATERIAL_DEFAULT);

	//TODO vérifier si c'est possible de mettre ça lors de la création du component
	//TODO Il faut lier la stage manuellement, il faut trouver une solution
	material->pipelineMaterial = PipelineMaterial::Create({ 0, 0 }, PipelineGraphicsCreate({ "../Shaders/shader.vert", "../Shaders/shader.frag" }, { ModelComponentManager::GetVertexInput() }, MaterialDefaultManager::GetDefines(*material), PipelineGraphics::Mode::MRT));
}
}
