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

#include <graphics/buffers/storage_handle.h>

namespace dm
{
StorageHandle::StorageHandle(const bool& multiPipeline) :
	m_MultiPipeline(multiPipeline),
	m_Size(0),
	m_Data(nullptr),
	m_StorageBuffer(nullptr),
	m_HandleStatus(Buffer::Status::NORMAL) {}

StorageHandle::StorageHandle(const Shader::UniformBlock& uniformBlock, const bool& multiPipeline) : 
	m_MultiPipeline(multiPipeline),
	m_UniformBlock(uniformBlock),
	m_Size(static_cast<uint32_t>(m_UniformBlock->GetSize())),
	m_Data(std::make_unique<char[]>(m_Size)),
	m_StorageBuffer(std::make_unique<StorageBuffer>(static_cast<VkDeviceSize>(m_Size))),
	m_HandleStatus(Buffer::Status::NORMAL)
{}

void StorageHandle::Push(void* data, const std::size_t& size)
{
	if (size != m_Size)
	{
		m_Size = static_cast<uint32_t>(size);
		m_HandleStatus = Buffer::Status::RESET;
		return;
	}

	if (!m_UniformBlock)
	{
		return;
	}

	if (std::memcmp(m_Data.get(), data, size) != 0)
	{
		std::memcpy(m_Data.get(), data, size);
		m_HandleStatus = Buffer::Status::CHANGED;
	}
}

bool StorageHandle::Update(const std::optional<Shader::UniformBlock>& uniformBlock)
{
	if (m_HandleStatus == Buffer::Status::RESET || (m_MultiPipeline && !m_UniformBlock) || (!m_MultiPipeline && m_UniformBlock != uniformBlock))
	{
		if ((m_Size == 0 && !m_UniformBlock) || (m_UniformBlock && m_UniformBlock != uniformBlock && static_cast<uint32_t>(m_UniformBlock->GetSize()) == m_Size)) {
			m_Size = static_cast<uint32_t>(uniformBlock->GetSize());
		}

		m_UniformBlock = uniformBlock;
		m_Data = std::make_unique<char[]>(m_Size);
		m_StorageBuffer = std::make_unique<StorageBuffer>(static_cast<VkDeviceSize>(m_Size));
		m_HandleStatus = Buffer::Status::CHANGED;
		return false;
	}

	if (m_HandleStatus != Buffer::Status::NORMAL)
	{
		m_StorageBuffer->Update(m_Data.get());
		m_HandleStatus = Buffer::Status::NORMAL;
	}

	return true;
}
}
