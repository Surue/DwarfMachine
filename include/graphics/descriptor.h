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

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <vulkan/vulkan.h>
#include <memory>
#include <optional>
#include <iostream>

namespace dm
{
class OffsetSize
{
public:
	OffsetSize(const uint32_t &offset, const uint32_t &size) :
		m_Offset(offset),
		m_Size(offset)
	{
		std::cout << offset << ", " << size << "\n";

		if(offset > size)
		{
			m_Size = offset;
		}
	}

	const uint32_t &GetOffset() const { return m_Offset; }

	const uint32_t &GetSize() const { return m_Size; }

	bool operator==(const OffsetSize & other) const
	{
		return m_Offset == other.m_Offset && m_Size == other.m_Size;
	}

	bool operator!=(const OffsetSize &other) const
	{
		return !(*this == other);
	}
private:
	uint32_t m_Offset;
	uint32_t m_Size;
};

class WriteDescriptorSet
{
public:
	WriteDescriptorSet(const VkWriteDescriptorSet &writeDescriptorSet, const VkDescriptorImageInfo &imageInfo) :
		m_WriteDescriptorSet(writeDescriptorSet),
		m_ImageInfo(std::make_unique<VkDescriptorImageInfo>(imageInfo)),
		m_BufferInfo(nullptr)
	{
		m_WriteDescriptorSet.pImageInfo = m_ImageInfo.get();
	}

	WriteDescriptorSet(const VkWriteDescriptorSet &writeDescriptorSet, const VkDescriptorBufferInfo &bufferInfo) :
		m_WriteDescriptorSet(writeDescriptorSet),
		m_ImageInfo(nullptr),
		m_BufferInfo(std::make_unique<VkDescriptorBufferInfo>(bufferInfo))
	{
		m_WriteDescriptorSet.pBufferInfo = m_BufferInfo.get();
	}

	const VkWriteDescriptorSet &GetWriteDescriptorSet() const { return m_WriteDescriptorSet; }
private:
	VkWriteDescriptorSet m_WriteDescriptorSet;
	std::unique_ptr<VkDescriptorImageInfo> m_ImageInfo;
	std::unique_ptr<VkDescriptorBufferInfo> m_BufferInfo;
};

class Descriptor
{
public:
	virtual WriteDescriptorSet GetWriteDescriptor(const uint32_t &binding, const VkDescriptorType &descriptorType, const std::optional<OffsetSize> &offsetSize) const = 0;

	Descriptor() = default;

	virtual ~Descriptor() = default;
};
}

#endif DESCRIPTOR_H