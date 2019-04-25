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

#include <graphics/push_handle.h>

namespace dm
{
PushHandle::PushHandle(const bool& multiPipeline) :
	m_MultiPipeline(multiPipeline),
	m_Data(nullptr) {}

PushHandle::PushHandle(const Shader::UniformBlock& uniformBlock, const bool& multiPipeline) :
	m_MultiPipeline(multiPipeline),
	m_UniformBlock(uniformBlock),
	m_Data(std::make_unique<char[]>(m_UniformBlock->GetSize())) 
{}

bool PushHandle::Update(const std::optional<Shader::UniformBlock>& uniformBlock)
{
	if((m_MultiPipeline && !m_UniformBlock) || (!m_MultiPipeline && m_UniformBlock != uniformBlock))
	{
		m_UniformBlock = uniformBlock;
		m_Data = std::make_unique<char[]>(m_UniformBlock->GetSize());
		return false;
	}

	return true;
}

void PushHandle::BindPush(const CommandBuffer& commandBuffer, const Pipeline& pipeline)
{
	vkCmdPushConstants(commandBuffer, pipeline.GetPipelineLayout(), m_UniformBlock->GetStageFlags(), 0, static_cast<uint32_t>(m_UniformBlock->GetSize()), m_Data.get());
}
}
