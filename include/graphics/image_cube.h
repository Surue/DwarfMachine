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

#ifndef IMAGE_CUBE_H
#define IMAGE_CUBE_H

#include <graphics/image.h>

namespace dm
{
class ImageCube : public Descriptor
{
public:
	static std::shared_ptr<ImageCube> Create(const std::string &filename, const std::string fileSuffix, const VkFilter &filter = VK_FILTER_LINEAR, const VkSamplerAddressMode &addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, const bool &anisotropic = true, const bool &mipmap = true);

	explicit ImageCube(std::string filename, std::string fileSuffix = ".png", const VkFilter &filter = VK_FILTER_LINEAR, const VkSamplerAddressMode &addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, const bool &anisotropic = true, const bool &mipmap = true, const bool &load = true);

	ImageCube(const uint32_t &width, const uint32_t &height, std::unique_ptr<uint8_t[]> pixels = nullptr, const VkFormat &format = VK_FORMAT_R8G8B8A8_UNORM, const VkImageLayout &layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, const VkImageUsageFlags &usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, const VkFilter &filter = VK_FILTER_LINEAR, const VkSamplerAddressMode &addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, const VkSampleCountFlagBits &samples = VK_SAMPLE_COUNT_1_BIT, const bool &anisotropic = false, const bool &mipmap = false);

	~ImageCube();

	static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(const uint32_t &binding, const VkDescriptorType &descriptorType, const VkShaderStageFlags &stage, const uint32_t &count);

	WriteDescriptorSet GetWriteDescriptor(const uint32_t& binding, const VkDescriptorType& descriptorType, const std::optional<OffsetSize>& offsetSize) const override;

	void Load();

	std::unique_ptr<uint8_t[]> GetPixels(VkExtent3D &extent, const uint32_t &mipLevel, const uint32_t &arrayLayer) const;

	std::unique_ptr<uint8_t[]> GetPixels(VkExtent3D &extent, const uint32_t &mipLevel = 0) const;

	void SetPixels(const uint8_t *pixels, const uint32_t &layerCount, const uint32_t &baseArrayLayer);
	
	static std::unique_ptr<uint8_t[]> LoadPixels(const std::string &filename, const std::string &fileSuffix, const std::vector<std::string> &fileSides, uint32_t &width, uint32_t &height, uint32_t &components, VkFormat &format);

	const std::string &GetFilename() const { return m_Filename; }

	const std::string &GetFileSuffix() { return m_FileSuffix; };

	const VkFilter &GetFilter() const { return m_Filter; }

	const VkSamplerAddressMode &GetAddressMode() const { return m_AddressMode; }

	const bool &IsAnisotropic() const { return m_Anisotropic; }

	const bool &IsMipmap() const { return m_Mipmap; }

	const VkSampleCountFlagBits &GetSamples() const { return m_Samples; }

	const VkImageLayout &GetLayout() const { return m_Layout; }

	const VkImageUsageFlags &GetUsage() const { return m_Components; }

	const uint32_t &GetComponents() const { return m_Components; }

	const uint32_t &GetWidth() const { return m_Width; }

	const uint32_t &GetHeight() const { return m_Height; }

	const uint32_t &GetMipLevels() const { return m_MipLevels; }

	const VkImage &GetImage() { return m_Image; }

	const VkDeviceMemory &GetMemory() { return m_Memory; }

	const VkSampler &GetSampler() { return m_Sampler; }

	const VkImageView &GetView() { return m_View; }

	const VkFormat &GetFormat() { return m_Format; }
private:
	std::string m_Filename;
	std::string m_FileSuffix;
	std::vector<std::string> m_FileSides;

	VkFilter m_Filter;
	VkSamplerAddressMode m_AddressMode;
	bool m_Anisotropic;
	bool m_Mipmap;
	VkSampleCountFlagBits m_Samples;
	VkImageLayout m_Layout;
	VkImageUsageFlags m_Usage;

	uint32_t m_Components;
	uint32_t m_Width;
	uint32_t m_Height;
	std::unique_ptr<uint8_t[]> m_LoadPixels;
	uint32_t m_MipLevels;

	VkImage m_Image;
	VkDeviceMemory m_Memory;
	VkSampler m_Sampler;
	VkImageView m_View;
	VkFormat m_Format;
};
}

#endif IMAGE_CUBE_H