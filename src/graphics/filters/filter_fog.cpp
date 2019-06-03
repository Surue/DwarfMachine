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

#include <graphics/filters/filter_fog.h>
#include "graphics/graphic_manager.h"
#include "component/lights/directional_light.h"
#include "entity/entity_handle.h"
#include <engine/engine.h>

namespace dm{
FilterFog::FilterFog(const Pipeline::Stage& pipelineStage, const bool& lastFilter) :
	PostFilter(pipelineStage, { "../Shaders/filter_fog.vert", "../Shaders/filter_fog.frag" }, {})
{
}

void FilterFog::Draw(const CommandBuffer& commandBuffer)
{
	auto camera = GraphicManager::Get()->GetCamera();

	PushConditional("writeColor", "samplerColor", "resolved", "diffuse");

	m_UniformScene.Push("view", camera->viewMatrix);
	m_UniformScene.Push("cameraPosition", camera->position);

	for (auto entity : Engine::Get()->GetEntityManager()->GetEntities())
	{
		auto entityHandle = EntityHandle(entity);

		if (entity == INVALID_ENTITY)
		{
			break;
		}

		//Directional
		if (entityHandle.HasComponent(ComponentType::DIRECTIONAL_LIGHT))
		{
			const auto light = entityHandle.GetComponent<DirectionalLight>(ComponentType::DIRECTIONAL_LIGHT);

			m_UniformScene.Push("lightDir", light->direction);
			break;
		}
	}

	m_DescriptorSet.Push("UniformScene", m_UniformScene);
	m_DescriptorSet.Push("samplerPosition", GraphicManager::Get()->GetAttachment("position"));
	m_DescriptorSet.Push("samplerColor", GraphicManager::Get()->GetAttachment("resolved"));

	bool updateSuccess = m_DescriptorSet.Update(m_Pipeline);

	if (!updateSuccess)
	{
		return;
	}

	m_Pipeline.BindPipeline(commandBuffer);

	m_DescriptorSet.BindDescriptor(commandBuffer, m_Pipeline);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
}
