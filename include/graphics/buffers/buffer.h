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

#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.h>

namespace dm
{
class LogicalDevice;

/**
 * \brief Wapper for VkBuffer and VkDeviceMemory
 */
class Buffer
{
public:
	enum class Status
	{
		RESET,
		CHANGED,
		NORMAL
	};

	Buffer(const VkDeviceSize &size, const VkBufferUsageFlags &usage, const VkMemoryPropertyFlags &properties, const void *data = nullptr);
	virtual ~Buffer();

	/**
	 * \brief Map data into the logical device memory
	 * \param data 
	 */
	void MapMemory(void **data);

	/**
	 * \brief Unmap data from the logical device's memory
	 */
	void UnmapMemory() const;

	const VkDeviceSize &GetSize() const { return m_Size; }

	const VkBuffer &GetBuffer() const { return m_Buffer; }

	const VkDeviceMemory &GetBufferMemory() const { return m_BufferMemory; }

	/**
	 * \brief Check if the needed memory property exist in the physical device
	 * \param typeFilter 
	 * \param requiredProperties 
	 * \return 
	 */
	static uint32_t FindMemoryType(const uint32_t &typeFilter, const VkMemoryPropertyFlags &requiredProperties);

protected:
	VkDeviceSize m_Size;
	VkBuffer m_Buffer;
	VkDeviceMemory m_BufferMemory;
};
}

#endif BUFFER_H