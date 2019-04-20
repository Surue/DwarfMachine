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

#include <graphics/image.h>
#include <graphics/graphic_manager.h>

namespace dm
{
static const float ANISOTROPY = 16.0f;

Image::Image(const VkExtent3D& extent, const VkImageType& imageType, const VkFormat& format,
	const VkSampleCountFlagBits& samples, const VkImageTiling& tiling, const VkImageUsageFlags& usage,
	const VkMemoryPropertyFlags& properties, const uint32_t& mipLevels, const uint32_t& arrayLayers) :
	m_Extent(extent),
	m_Format(format),
	m_Sample(samples),
	m_Usage(usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
	m_MipLevels(mipLevels),
	m_Image(VK_NULL_HANDLE),
	m_Memory(VK_NULL_HANDLE),
	m_Sampler(VK_NULL_HANDLE),
	m_View(VK_NULL_HANDLE)
{
	CreateImage(m_Image, m_Memory, m_Extent, m_Format, m_Sample, tiling, m_Usage, properties, m_MipLevels, arrayLayers, imageType);
}

Image::~Image()
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	vkDestroyImageView(*logicalDevice, m_View, nullptr);
	vkDestroySampler(*logicalDevice, m_Sampler, nullptr);
	vkFreeMemory(*logicalDevice, m_Memory, nullptr);
	vkDestroyImage(*logicalDevice, m_Image, nullptr);
}

VkDescriptorSetLayoutBinding Image::GetDescriptorSetLayout(const uint32_t& binding, const VkDescriptorType& descriptorType,
	const VkShaderStageFlags& stage, const uint32_t& count)
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding = binding;
	descriptorSetLayoutBinding.descriptorType = descriptorType;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = stage;
	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

	return descriptorSetLayoutBinding;
}

WriteDescriptorSet Image::GetWriteDescriptor(const uint32_t& binding, const VkDescriptorType& descriptorType,
	const std::optional<OffsetSize>& offsetSize) const
{
	VkDescriptorImageInfo imageInfo = { };
	imageInfo.sampler = m_Sampler;
	imageInfo.imageView = m_View;
	imageInfo.imageLayout = m_Layout;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = VK_NULL_HANDLE;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = descriptorType;

	return WriteDescriptorSet(descriptorWrite, imageInfo);
}

std::unique_ptr<uint8_t[]> Image::GetPixels(VkExtent3D& extent, const uint32_t& mipLevel,
	const uint32_t& arrayLayer) const
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	extent.width = int32_t(m_Extent.width >> mipLevel);
	extent.height = int32_t(m_Extent.height >> mipLevel);
	extent.depth = 1;

	VkImage dstImage;
	VkDeviceMemory dstImageMemory;
	CopyImage(m_Image, dstImage, dstImageMemory, m_Format, m_Extent, m_Layout, mipLevel, arrayLayer);

	VkImageSubresource dstImageSubresource = {};
	dstImageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	dstImageSubresource.mipLevel = 0;
	dstImageSubresource.arrayLayer = 0;

	VkSubresourceLayout dstSubresourceLayout;
	vkGetImageSubresourceLayout(*logicalDevice, dstImage, &dstImageSubresource, &dstSubresourceLayout);

	auto pixels = std::make_unique<uint8_t[]>(dstSubresourceLayout.size);

	void *data;
	vkMapMemory(*logicalDevice, dstImageMemory, dstSubresourceLayout.offset, dstSubresourceLayout.size, 0, &data);
	std::memcpy(pixels.get(), data, static_cast<size_t>(dstSubresourceLayout.size));
	vkUnmapMemory(*logicalDevice, dstImageMemory);

	vkFreeMemory(*logicalDevice, dstImageMemory, nullptr);
	vkDestroyImage(*logicalDevice, dstImage, nullptr);

	return pixels;
}
}
