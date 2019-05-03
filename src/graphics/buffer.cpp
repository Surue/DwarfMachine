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

#include <graphics/buffer.h>

#include <graphics/graphic_manager.h>

namespace dm
{
Buffer::Buffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties,
               const void* data) : 
	m_Size(size), 
	m_Buffer(VK_NULL_HANDLE),
    m_BufferMemory(VK_NULL_HANDLE)
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	const auto graphicsFamily = logicalDevice->GetGraphicsFamily();
	const auto presentFamily = logicalDevice->GetPresentFamily();
	const auto computeFamily = logicalDevice->GetComputeFamily();

	std::array<uint32_t, 3> queueFamily = { graphicsFamily, presentFamily, computeFamily };

	// Create the buffer handle.
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
	bufferCreateInfo.pQueueFamilyIndices = queueFamily.data();
	GraphicManager::CheckVk(vkCreateBuffer(*logicalDevice, &bufferCreateInfo, nullptr, &m_Buffer));

	// Create the memory backing up the buffer handle.
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(*logicalDevice, m_Buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);
	GraphicManager::CheckVk(vkAllocateMemory(*logicalDevice, &memoryAllocateInfo, nullptr, &m_BufferMemory));

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data.
	if (data != nullptr)
	{
		void *mapped;
		MapMemory(&mapped);
		std::memcpy(mapped, data, size);

		// If host coherency hasn't been requested, do a manual flush to make writes visible.
		if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange mappedMemoryRange = {};
			mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedMemoryRange.memory = m_BufferMemory;
			mappedMemoryRange.offset = 0;
			mappedMemoryRange.size = size;
			vkFlushMappedMemoryRanges(*logicalDevice, 1, &mappedMemoryRange);
		}

		UnmapMemory();
	}

	// Attach the memory to the buffer object.
	GraphicManager::CheckVk(vkBindBufferMemory(*logicalDevice, m_Buffer, m_BufferMemory, 0));
}

Buffer::~Buffer()
{
	std::cout << "~Buffer\n";
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	vkDestroyBuffer(*logicalDevice, m_Buffer, nullptr);
	vkFreeMemory(*logicalDevice, m_BufferMemory, nullptr);
}

void Buffer::MapMemory(void** data) const
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	GraphicManager::CheckVk(vkMapMemory(*logicalDevice, GetBufferMemory(), 0, m_Size, 0, data));
}

void Buffer::UnmapMemory() const
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	vkUnmapMemory(*logicalDevice, GetBufferMemory());
}

uint32_t Buffer::FindMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& requiredProperties)
{
	const auto physicalDevice = Engine::Get()->GetGraphicManager()->GetPhysicalDevice();
	const auto memoryProperties = physicalDevice->GetMemoryProperties();

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		const uint32_t memoryTypeBits = 1 << i;
		const bool isRequiredMemoryType = typeFilter & memoryTypeBits;

		const auto properties = memoryProperties.memoryTypes[i].propertyFlags;
		const auto hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

		if (isRequiredMemoryType && hasRequiredProperties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find a valid memory type for buffer");
}
}
