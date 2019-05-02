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

#include <graphics/uniform_handle.h>

namespace dm
{
UniformHandle::UniformHandle(const bool& multiPipeline) :
	m_MultiPipeline(multiPipeline),
	m_Size(0),
	m_Data(nullptr),
	m_UniformBuffer(nullptr),
	m_HandleStatus(Buffer::Status::NORMAL)
{ }

UniformHandle::UniformHandle(const Shader::UniformBlock& uniformBlock, const bool& multiPipeline) :
	m_MultiPipeline(multiPipeline),
	m_UniformBlock(uniformBlock),
	m_Size(static_cast<uint32_t>(m_UniformBlock->GetSize())),
	m_Data(std::make_unique<char[]>(m_Size)),
	m_UniformBuffer(std::make_unique<UniformBuffer>(static_cast<VkDeviceSize>(m_Size))),
	m_HandleStatus(Buffer::Status::NORMAL)
{ }

bool UniformHandle::Update(const std::optional<Shader::UniformBlock>& uniformBlock)
{
	if (m_HandleStatus == Buffer::Status::RESET || (m_MultiPipeline && !m_UniformBlock) || (!m_MultiPipeline && m_UniformBlock != uniformBlock))
	{
		if ((m_Size == 0 && !m_UniformBlock) || (m_UniformBlock && m_UniformBlock != uniformBlock && static_cast<uint32_t>(m_UniformBlock->GetSize()) == m_Size))
		{
			m_Size = static_cast<uint32_t>(uniformBlock->GetSize());
		}

		m_UniformBlock = uniformBlock;
		m_Data = std::make_unique<char[]>(m_Size);
		m_UniformBuffer = std::make_unique<UniformBuffer>(static_cast<VkDeviceSize>(m_Size));
		m_HandleStatus = Buffer::Status::CHANGED;
		return false;
	}

	if (m_HandleStatus != Buffer::Status::NORMAL)
	{
		m_UniformBuffer->Update(m_Data.get());
		m_HandleStatus = Buffer::Status::NORMAL;
	}

	return true;
}
}
