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

#ifndef IMAGE_DEPTH_H
#define IMAGE_DEPTH_H

#include <graphics/descriptor.h>
#include <graphics/image.h>

namespace dm
{
class ImageDepth : public Descriptor
{
public:
	ImageDepth(const uint32_t &width, const uint32_t &height, const VkSampleCountFlagBits &samples = VK_SAMPLE_COUNT_1_BIT);

	~ImageDepth();

	static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(const uint32_t &binding, const VkDescriptorType &descriptorType, const VkShaderStageFlags &stage);

	WriteDescriptorSet GetWriteDescriptor(const uint32_t& binding, const VkDescriptorType& descriptorType, const std::optional<OffsetSize>& offsetSize) const override;

	const uint32_t &GetWidth() const { return m_Width; }

	const uint32_t &GetHeight() const { return m_Height; }

	const VkImage &GetImage() const { return m_Image; }

	const VkDeviceMemory &GetMemory() const { return m_Memory; }

	const VkSampler &GetSampler() const { return m_Sampler; }

	const VkImageView &GetView() const { return m_View; }

	const VkFormat &GetFormat() const { return m_Format; }
	
private:
	uint32_t m_Width;
	uint32_t m_Height;

	VkImage m_Image;
	VkDeviceMemory m_Memory;
	VkSampler m_Sampler;
	VkImageView m_View;
	VkFormat m_Format;
};
}
#endif IMAGE_DEPTH_H