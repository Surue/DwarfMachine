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

#include <graphics/pipeline_material.h>

#include <graphics/graphic_manager.h>
#include <engine/engine.h>
#include <graphics/pipeline_material_manager.h>

namespace dm
{
PipelineMaterial* PipelineMaterial::Create(const Pipeline::Stage& pipelineStage,
	const PipelineGraphicsCreate& pipelineCreate)
{
	auto* tmp = Engine::Get()->GetPipelineMaterialManager()->GetMaterial(pipelineStage, pipelineCreate);
	if(tmp != nullptr)
	{
		return tmp;
	}

	return Engine::Get()->GetPipelineMaterialManager()->AddMaterial(pipelineStage, pipelineCreate);
}

PipelineMaterial::PipelineMaterial(Pipeline::Stage pipelineStage, PipelineGraphicsCreate pipelineCreate) :
	m_PipelineStage(std::move(pipelineStage)),
	m_PipelineGraphicsCreate(std::move(pipelineCreate)),
	m_RenderStage(nullptr),
	m_Pipeline(nullptr)
{
	
}

bool PipelineMaterial::BindPipeline(const CommandBuffer& commandBuffer)
{
	const auto renderStage = GraphicManager::Get()->GetRenderStage(m_PipelineStage.first);

	if(renderStage == nullptr)
	{
		return false;
	}

	if(m_RenderStage != renderStage)
	{
		m_RenderStage = renderStage;
		m_Pipeline.reset(m_PipelineGraphicsCreate.Create(m_PipelineStage));
	}

	m_Pipeline->BindPipeline(commandBuffer);
	return true;
}
}
