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

#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan.h>
#include <graphics/command_buffer.h>
#include <graphics/descriptor.h>

namespace dm
{
class Image : public Descriptor
{
public:
	Image(const VkExtent3D& extent, const VkImageType& imageType, const VkFormat& format,
	      const VkSampleCountFlagBits& samples, const VkImageTiling& tiling, const VkImageUsageFlags& usage,
	      const VkMemoryPropertyFlags& properties, const uint32_t& mipLevels, const uint32_t& arrayLayers);

	~Image();

	static VkDescriptorSet GetDescriptorSetLayout(const uint32_t &binding, const VkDescriptorType &descriptorType, const VkShaderStageFlags &stage, const uint32_t &count);

	WriteDescriptorSet GetWriteDescriptor(const uint32_t &binding, const VkDescriptorType &descriptorType, const std::optional<OffsetSize> &offsetSize) const override;

	std::unique_ptr<uint8_t[]> GetPixels(VkExtent3D &extent, const uint32_t &mipLevel = 0, const uint32_t &arrayLayer = 0) const;

	void SetPixels(const uint8_t *pixels, const uint32_t &layerCount, const uint32_t &baseArrayLayer);

	const VkExtent3D &GetExtent() const { return m_Extent; }

	const VkFormat &GetFormat() const { return m_Format; }

	const VkSampleCountFlagBits &GetSamples() const { return m_Sample; }

	const VkImageUsageFlags &GetUsage() const { return m_Usage; }

	const uint32_t &GetMipLevels() const { return m_MipLevels; }

	const uint32_t &GetArrayLevels() const { return m_ArrayLayers; }

	const VkFilter &GetFilter() const { return m_Filter; }

	const VkSamplerAddressMode &GetAddressMode() const { return m_AddressMode; }

	const bool &IsAnisotropic() const { return m_Anisotropic; }

	const VkImageLayout &GetLayout() const { return m_Layout; }

	const VkImage &GetImage() const { return m_Image; }

	static std::unique_ptr<uint8_t[]> LoadPixels(const std::string &filename, uint32_t &width, uint32_t &height, uint32_t &components, VkFormat &format);

	static void WritePixels(const std::string &filename, const uint8_t *pixels, const int32_t &width, const uint32_t &height, const int32_t &components = 4);

	static uint32_t GetMipLevels(const VkExtent3D &extent);

	static bool HasDepth(const VkFormat &format);

	static bool HasStencil(const VkFormat &format);

	static void CreateImage(VkImage &image, VkDeviceMemory &memory, const VkExtent3D &extent, const VkFormat &format, const VkSampleCountFlagBits &samples, const VkImageTiling &tiling, const VkImageUsageFlags &usage, const VkMemoryPropertyFlags &properties, const uint32_t &mipLevels, const uint32_t &arrayLayers, const VkImageType &type);

	static void CreateImageSampler(VkSampler &sampler, const VkFilter &filter, const VkSamplerAddressMode &addressMode, const bool &anisotropic, const uint32_t &mipLevels);

	static void CreateImageView(const VkImage &image, VkImageView &imageView, const VkImageViewType &type, const VkFormat &format, const VkImageAspectFlags &imageAspect, const uint32_t &mipLevels, const uint32_t &baseMipLevel, const uint32_t &layerCount, const uint32_t &baseArrayLayer);

	static void CreateMipmaps(const VkImage &image, const VkExtent3D &extent, const VkFormat &format, const VkImageLayout &dstImageLayout, const uint32_t &mipLevels, const uint32_t &baseArrayLayer, const uint32_t &layerCount);

	static void TransitionImageLayout(const VkImage &image, const VkFormat &format, const VkImageLayout &srcImageLayout, const VkImageLayout &dstImageLayout, const VkImageAspectFlags &imageAspect, const uint32_t &mipLevels, const uint32_t &baseMipLevel, const uint32_t &layerCount, const uint32_t &baseArrayLayer);

	static void InsertImageMemoryBarrier(const CommandBuffer &commandBuffer, const VkImage &image, const VkAccessFlags &srcAccessMask, const VkAccessFlags &dstAccessMask, const VkImageLayout &oldImageLayout, const VkImageLayout &newImageLayout, const VkPipelineStageFlags &srcStageMask, const VkPipelineStageFlags &dstStageMask, const VkImageAspectFlags &imageAspect, const uint32_t &mipLevels, const uint32_t &baseMipLevel, const uint32_t &layerCount, const uint32_t &baseArrayLayer);

	static void CopyBufferToImage(const VkBuffer &buffer, const VkImage &image, const VkExtent3D &extent, const uint32_t &layerCount, const uint32_t &baseArrayLayer);

	static bool CopyImage(const VkImage &srcImage, VkImage &dstImage, VkDeviceMemory &dstImageMemory, const VkFormat &srcFormat, const VkExtent3D &extent, const VkImageLayout &srcImageLayout, const uint32_t &mipLevel, const uint32_t &arrayLayer);
private:
	VkExtent3D m_Extent;
	VkFormat m_Format;
	VkSampleCountFlagBits m_Sample;
	VkImageUsageFlags m_Usage;
	uint32_t m_MipLevels;
	uint32_t m_ArrayLayers;

	VkFilter m_Filter;
	VkSamplerAddressMode m_AddressMode;
	bool m_Anisotropic;

	VkImageLayout m_Layout;

	VkImage m_Image;
	VkDeviceMemory m_Memory;
	VkSampler m_Sampler;
	VkImageView m_View;
};
}

#endif IMAGE_H
