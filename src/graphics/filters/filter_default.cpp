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

#include <graphics/filters/filter_default.h>
#include "editor/log.h"

namespace dm
{
FilterDefault::FilterDefault(const Pipeline::Stage& pipelineStage, const bool& lastFilter) :
	PostFilter(pipelineStage, {"../Shaders/filter_default.vert", "../Shaders/filter_default.frag"}, {}),
	m_LastFilter(lastFilter)
{
	Debug::Log(m_Pipeline.GetShader()->ToString());
}

void FilterDefault::Draw(const CommandBuffer& commandBuffer)
{
	PushConditional("writeColor", "samplerColor", "resolved", "diffuse");
	const auto updateSucceed = m_DescriptorSet.Update(m_Pipeline);

	if(!updateSucceed)
	{
		return;
	}

	m_Pipeline.BindPipeline(commandBuffer); 

	m_DescriptorSet.BindDescriptor(commandBuffer, m_Pipeline);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	if(m_LastFilter)
	{
		GlobalSwitching = 0;
	}
}
}
