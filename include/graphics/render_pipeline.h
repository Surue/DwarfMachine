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

#ifndef RENDER_PIPELINE_H
#define RENDER_PIPELINE_H

#include <graphics/command_buffer.h>
#include <graphics/pipeline.h>
#include <entity/entity.h>

namespace dm
{
class RenderPipeline
{
public:
	explicit RenderPipeline(Pipeline::Stage stage):
		m_Stage(std::move(stage)),
		m_Enabled(true)
	{}

	virtual ~RenderPipeline() = default;

	virtual void Update() = 0;

	virtual void Draw(const CommandBuffer &commandBuffer) = 0;

	const Pipeline::Stage &GetStage() const { return m_Stage; }

	const bool &IsEnabled() const { return m_Enabled; }

	void SetEnabled(const bool &enable) { m_Enabled = enable; }

	virtual void RegisterEntity(const Entity entity)
	{
		m_RegisteredEntities.push_back(entity);
	}

	void UnRegisterEntity(const Entity entity)
	{
		for (auto it = m_RegisteredEntities.begin(); it != m_RegisteredEntities.end(); ++it)
		{
			const auto e = *it;

			if (e == entity)
			{
				m_RegisteredEntities.erase(it);
				return;
			}
		}
	}

	ComponentMask GetSignature() const
	{
		return m_Signature;
	}

private:
	Pipeline::Stage m_Stage;
	bool m_Enabled;

protected:
	std::vector<Entity> m_RegisteredEntities;
	ComponentMask m_Signature;
};
}

#endif RENDER_PIPELINE_H