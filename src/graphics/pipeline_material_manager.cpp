/*
MIT License

Copyright(c) 2019 Nicolas Schneider

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <graphics/pipeline_material_manager.h>
#include "editor/log.h"

namespace dm
{
PipelineMaterialManager::PipelineMaterialManager() {}

void PipelineMaterialManager::Init()
{
}

void PipelineMaterialManager::Update()
{
}

void PipelineMaterialManager::Clear()
{
	m_RegisteredMaterials.clear();
	m_RegisteredInfos.clear();
}

void PipelineMaterialManager::Draw()
{
}

PipelineMaterial* PipelineMaterialManager::GetMaterial(const Pipeline::Stage& pipelineStage,
                                                       const PipelineGraphicsCreate& pipelineCreate)
{
	int i = 0;
	for (const auto& infoMaterial : m_RegisteredInfos)
	{
		if (infoMaterial.first == pipelineStage && infoMaterial.second == pipelineCreate)
		{
			return m_RegisteredMaterials[i].get();
		}
		i++;
	}

	Debug::Log("Error while loading material, the material is not loaded in the manager yet");
	return nullptr;
}

PipelineMaterial* PipelineMaterialManager::AddMaterial(const Pipeline::Stage& pipelineStage,
	const PipelineGraphicsCreate& pipelineCreate)
{
	m_RegisteredMaterials.push_back(std::make_unique<PipelineMaterial>(pipelineStage, pipelineCreate));
	m_RegisteredInfos.emplace_back(pipelineStage, pipelineCreate);
	return m_RegisteredMaterials[m_RegisteredMaterials.size() - 1].get();
}
}
