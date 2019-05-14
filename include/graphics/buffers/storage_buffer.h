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

#ifndef STORAGE_BUFFER_H
#define STORAGE_BUFFER_H

#include <graphics/descriptor.h>
#include <graphics/buffers/buffer.h>

namespace dm
{
/**
 * \brief Buffer to store data, mainly used for layout in shader
 */
class StorageBuffer : public Descriptor, public Buffer
{
public:
	explicit StorageBuffer(const VkDeviceSize &size, const void *data = nullptr);

	/**
	 * \brief Change data inside the buffer
	 * \param newData 
	 */
	void Update(const void *newData);

	/**
	 * \brief Get a descriptor set layout using all params
	 * \param binding 
	 * \param descriptorType 
	 * \param stage 
	 * \param count 
	 * \return 
	 */
	static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(const uint32_t &binding, const VkDescriptorType &descriptorType, const VkShaderStageFlags &stage, const uint32_t &count);

	/**
	 * \brief Get the write descriptor for a specific binding
	 * \param binding 
	 * \param descriptorType 
	 * \param offsetSize 
	 * \return 
	 */
	WriteDescriptorSet GetWriteDescriptor(const uint32_t& binding, const VkDescriptorType& descriptorType, const std::optional<OffsetSize>& offsetSize) const override;
};
}

#endif STORAGE_BUFFER_H