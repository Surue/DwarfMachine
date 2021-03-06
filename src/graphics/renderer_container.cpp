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

#include <graphics/renderer_container.h>

namespace dm
{
RendererContainer::RendererContainer()
{
	
}
void RendererContainer::Update()
{
	for(auto& stages : m_Stages)
	{
		for (auto& system : stages.second)
		{
			system->Update();
		}
	}
}

RenderPipeline* RendererContainer::Add(RenderPipeline* renderer)
{
	if(renderer == nullptr)
	{
		return nullptr;
	}

	auto emplaced = false;

	do
	{
		auto stage = m_Stages.find(renderer->GetStage());

		if(stage == m_Stages.end())
		{
			m_Stages.emplace(renderer->GetStage(), std::vector<std::unique_ptr<RenderPipeline>>());
		}else
		{
			(*stage).second.emplace_back(renderer);
			emplaced = true;
		}
	} while (!emplaced);

	return renderer;
}

void RendererContainer::Remove(RenderPipeline* renderer)
{
	for (auto it = m_Stages.begin(); it != m_Stages.end(); ++it)
	{
		for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it)
		{
			if ((*it2).get() == renderer)
			{
				(*it).second.erase(it2);

				if ((*it).second.empty())
				{
					m_Stages.erase(it);
				}
			}
		}
	}
}

void RendererContainer::RenderStage(const Pipeline::Stage& stage, const CommandBuffer& commandBuffer)
{
	
	/*for (const auto &typeId : m_stages)
	{
		if (typeId.first.first != stage)
		{
			continue;
		}

		auto &subrender = m_subrenders[typeId.second];

		if (subrender != nullptr)
		{
			if (subrender->IsEnabled())
			{
				subrender->Render(commandBuffer);
			}
		}
	}*/
}
}
