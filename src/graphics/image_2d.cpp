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

#include <graphics/image_2d.h>
#include <engine/engine.h>
#include <graphics/graphic_manager.h>
#include <graphics/buffer.h>

namespace dm
{
std::shared_ptr<Image2d> Image2d::Create(const std::string& filename, const VkFilter& filter,
	const VkSamplerAddressMode& addressMode, const bool& anisotropic, const bool& mipmap)
{
	return std::make_shared<Image2d>(filename, filter, addressMode, anisotropic, mipmap, true);
}

Image2d::Image2d(std::string filename, const VkFilter& filter, const VkSamplerAddressMode& addressMode,
	const bool& anisotropic, const bool& mipmap, const bool& load) :
	m_Filename(std::move(filename)),
	m_Filter(filter),
	m_AddressMode(addressMode),
	m_Anisotropic(anisotropic),
	m_Mipmap(mipmap),
	m_Samples(VK_SAMPLE_COUNT_1_BIT),
	m_Layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
	m_Usage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
	m_Components(0),
	m_Width(0),
	m_Height(0),
	m_LoadPixels(nullptr),
	m_MipLevels(0),
	m_Image(VK_NULL_HANDLE),
	m_Memory(VK_NULL_HANDLE),
	m_Sampler(VK_NULL_HANDLE),
	m_View(VK_NULL_HANDLE),
	m_Format(VK_FORMAT_R8G8B8A8_UNORM)
{
	if(load)
	{
		Image2d::Load();
	}
}

Image2d::Image2d(const uint32_t& width, const uint32_t& height, std::unique_ptr<uint8_t[]> pixels,
	const VkFormat& format, const VkImageLayout& layout, const VkImageUsageFlags& usage, const VkFilter& filter,
	const VkSamplerAddressMode& addressMode, const VkSampleCountFlagBits& samples, const bool& anisotropic,
	const bool& mipmap) :
	m_Filename(""),
	m_Filter(filter),
	m_AddressMode(addressMode),
	m_Anisotropic(anisotropic),
	m_Mipmap(mipmap),
	m_Samples(samples),
	m_Layout(layout),
	m_Usage(usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
	m_Components(4),
	m_Width(width),
	m_Height(height),
	m_LoadPixels(std::move(pixels)),
	m_MipLevels(0),
	m_Image(VK_NULL_HANDLE),
	m_Memory(VK_NULL_HANDLE),
	m_Sampler(VK_NULL_HANDLE),
	m_View(VK_NULL_HANDLE),
	m_Format(format)
{
	//if (usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) { m_Usage = usage; }
	Image2d::Load();
}

Image2d::~Image2d()
{
	auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	vkDestroySampler(*logicalDevice, m_Sampler, nullptr);
	vkDestroyImageView(*logicalDevice, m_View, nullptr);
	vkFreeMemory(*logicalDevice, m_Memory, nullptr);
	vkDestroyImage(*logicalDevice, m_Image, nullptr);
}

VkDescriptorSetLayoutBinding Image2d::GetDescriptorSetLayout(const uint32_t& binding,
	const VkDescriptorType& descriptorType, const VkShaderStageFlags& stage, const uint32_t& count)
{
	VkDescriptorSetLayoutBinding descriptorSet = {};
	descriptorSet.binding = binding;
	descriptorSet.descriptorType = descriptorType;
	descriptorSet.descriptorCount = 1;
	descriptorSet.stageFlags = stage;
	descriptorSet.pImmutableSamplers = nullptr;
	return descriptorSet;
}

WriteDescriptorSet Image2d::GetWriteDescriptor(const uint32_t& binding, const VkDescriptorType& descriptorType,
	const std::optional<OffsetSize>& offsetSize) const
{
	VkDescriptorImageInfo imageInfo = {};
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

void Image2d::Load()
{
	if(!m_Filename.empty() && m_LoadPixels == nullptr)
	{
		m_LoadPixels = Image::LoadPixels(m_Filename, m_Width, m_Height, m_Components, m_Format);
	}

	if(m_Width == 0 && m_Height == 0)
	{
		return;
	}

	m_MipLevels = m_Mipmap ? Image::GetMipLevels({ m_Width, m_Height, 1 }) : 1;

	Image::CreateImage(m_Image, m_Memory, { m_Width, m_Height, 1 }, m_Format, m_Samples, VK_IMAGE_TILING_OPTIMAL, m_Usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_MipLevels, 1, VK_IMAGE_TYPE_2D);
	Image::CreateImageSampler(m_Sampler, m_Filter, m_AddressMode, m_Anisotropic, m_MipLevels);
	Image::CreateImageView(m_Image, m_View, VK_IMAGE_VIEW_TYPE_2D, m_Format, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels, 0, 1, 0);

	if(m_LoadPixels != nullptr || m_Mipmap)
	{
		Image::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels, 0, 1, 0);
	}

	if(m_LoadPixels != nullptr)
	{
		VkDeviceSize imageSize = m_Width * m_Height * m_Components;

		auto stagingBuffer = Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void *data;
		stagingBuffer.MapMemory(&data);
		std::memcpy(data, m_LoadPixels.get(), stagingBuffer.GetSize());
		stagingBuffer.UnmapMemory();

		Image::CopyBufferToImage(stagingBuffer.GetBuffer(), m_Image, { m_Width, m_Height, 1 }, 1, 0);
	}

	if(m_Mipmap)
	{
		Image::CreateMipmaps(m_Image, { m_Width, m_Height, 1 }, m_Format, m_Layout, m_MipLevels, 0, 1);
	}else if(m_LoadPixels != nullptr)
	{
		Image::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_Layout, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels, 0, 1, 0);
	}else
	{
		Image::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, m_Layout, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels, 0, 1, 0);
	}

	m_LoadPixels = nullptr;
}

std::unique_ptr<uint8_t[]> Image2d::GetPixels(VkExtent3D& extent, const uint32_t& mipLevel) const
{
	auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	extent.width = int32_t(m_Width >> mipLevel);
	extent.height = int32_t(m_Height >> mipLevel);
	extent.depth = 1;

	VkImage dstImage;
	VkDeviceMemory dstImageMemory;
	Image::CopyImage(m_Image, dstImage, dstImageMemory, m_Format, extent, m_Layout, mipLevel, 0);

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
void Image2d::SetPixels(const uint8_t* pixels, const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	VkDeviceSize imageSize = m_Width * m_Height * m_Components;
	auto stagingBuffer = Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void *data;
	stagingBuffer.MapMemory(&data);
	std::memcpy(data, pixels, stagingBuffer.GetSize());
	stagingBuffer.UnmapMemory();


	Image::CopyBufferToImage(stagingBuffer.GetBuffer(), m_Image, { m_Width, m_Height, 1 }, layerCount, baseArrayLayer);
}
}
