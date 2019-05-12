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

#ifndef RENDERER_CONTAINER_H
#define RENDERER_CONTAINER_H

#include <graphics/render_pipeline.h>
#include "entity/entity.h"
#include <iostream>

namespace dm
{
class RendererContainer
{
public:
	RendererContainer();

	RendererContainer(const RendererContainer &) = delete;

	RendererContainer &operator=(const RendererContainer &) = delete;

	~RendererContainer() = default;

	void Update();

	const std::map<Pipeline::Stage, std::vector<std::unique_ptr<RenderPipeline>>> &GetStages() const { return m_Stages; }

	void Clear() { m_Stages.clear(); }

	template<typename T>
	T *Get(const bool &allowDisabled = false) const
	{
		T *alternative = nullptr;

		for (const auto &[key, renderers] : m_Stages)
		{
			for (const auto &renderer : renderers)
			{
				auto casted = dynamic_cast<T *>(renderer.get());

				if (casted != nullptr)
				{
					if (!allowDisabled && !casted->IsEnabled())
					{
						alternative = casted;
						continue;
					}

					return casted;
				}
			}
		}

		return alternative;
	}

	RenderPipeline *Add(RenderPipeline *renderer);

	template<typename T, typename... Args>
	T *Add(Args &&... args)
	{
		auto created = new T(std::forward<Args>(args)...);
		Add(created);
		return created;
	}

	void Remove(RenderPipeline *renderer);

	template<typename T>
	void Remove()
	{
		for (auto it = m_Stages.begin(); it != m_Stages.end(); ++it)
		{
			for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it)
			{
				auto casted = dynamic_cast<T *>((*it2).get());

				if (casted != nullptr)
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

	void AddComponent(const Entity entity, const ComponentMask oldMask, ComponentMask newMask)
	{
		for (const auto &[key, renderer] : m_Stages)
		{
			for (auto& system : renderer)
			{
				const auto systemMask = system->GetSignature();
				if (newMask.IsNewMatch(oldMask, systemMask))
				{
					system->RegisterEntity(entity);
				}
			}
		}
	}

	void DestroyComponent(const Entity entity, const ComponentMask oldMask, ComponentMask newMask)
	{
		for (auto& stages : m_Stages)
		{
			for (auto& system : stages.second)
			{
				const auto systemMask = system->GetSignature();
				if (newMask.IsNoLongerMatch(oldMask, systemMask))
				{
					system->UnRegisterEntity(entity);
				}
			}
		}
	}
private:
	std::map<Pipeline::Stage, std::vector<std::unique_ptr<RenderPipeline>>> m_Stages;
};
}

#endif //RENDERER_CONTAINER_H
