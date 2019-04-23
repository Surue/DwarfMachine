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

#include <graphics/image_cube.h>
#include <graphics/graphic_manager.h>
#include <graphics/buffer.h>

namespace dm
{
std::shared_ptr<ImageCube> ImageCube::Create(const std::string& filename, const std::string fileSuffix,
	const VkFilter& filter, const VkSamplerAddressMode& addressMode, const bool& anisotropic, const bool& mipmap)
{
	return std::make_shared<ImageCube>(filename, fileSuffix, filter, addressMode, anisotropic, mipmap, true);
}

ImageCube::ImageCube(std::string filename, std::string fileSuffix, const VkFilter& filter,
	const VkSamplerAddressMode& addressMode, const bool& anisotropic, const bool& mipmap, const bool& load) :
	m_Filename(std::move(filename)),
	m_FileSuffix(std::move(fileSuffix)),
	m_FileSides(std::vector<std::string>{"Right", "Left", "Top", "Bottom", "Back", "Front"}),
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
		Load();
	}
}

ImageCube::ImageCube(const uint32_t& width, const uint32_t& height, std::unique_ptr<uint8_t[]> pixels,
	const VkFormat& format, const VkImageLayout& layout, const VkImageUsageFlags& usage, const VkFilter& filter,
	const VkSamplerAddressMode& addressMode, const VkSampleCountFlagBits& samples, const bool& anisotropic,
	const bool& mipmap) :
	m_Filename(""),
	m_FileSuffix(""),
	m_Filter(filter),
	m_AddressMode(addressMode),
	m_Anisotropic(anisotropic),
	m_Mipmap(mipmap),
	m_Samples(samples),
	m_Layout(layout),
	m_Usage(usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
	m_Components(0),
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
	Load();
}

ImageCube::~ImageCube()
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	vkDestroyImageView(*logicalDevice, m_View, nullptr);
	vkDestroySampler(*logicalDevice, m_Sampler, nullptr);
	vkFreeMemory(*logicalDevice, m_Memory, nullptr);
	vkDestroyImage(*logicalDevice, m_Image, nullptr);
}

VkDescriptorSetLayoutBinding ImageCube::GetDescriptorSetLayout(const uint32_t& binding,
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

WriteDescriptorSet ImageCube::GetWriteDescriptor(const uint32_t& binding, const VkDescriptorType& descriptorType,
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

void ImageCube::Load()
{
	if (!m_Filename.empty() && m_LoadPixels == nullptr)
	{
		m_LoadPixels = LoadPixels(m_Filename, m_FileSuffix, m_FileSides, m_Width, m_Height, m_Components, m_Format);
	}

	if (m_Width == 0 && m_Height == 0)
	{
		return;
	}

	m_MipLevels = m_Mipmap ? Image::GetMipLevels({ m_Width, m_Height, 1 }) : 1;

	Image::CreateImage(m_Image, m_Memory, { m_Width, m_Height, 1 }, m_Format, m_Samples, VK_IMAGE_TILING_OPTIMAL, m_Usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_MipLevels, 6, VK_IMAGE_TYPE_2D);
	Image::CreateImageSampler(m_Sampler, m_Filter, m_AddressMode, m_Anisotropic, m_MipLevels);
	Image::CreateImageView(m_Image, m_View, VK_IMAGE_VIEW_TYPE_CUBE, m_Format, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels, 0, 6, 0);

	if (m_LoadPixels != nullptr || m_Mipmap)
	{
		Image::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels, 0, 6, 0);
	}

	if (m_LoadPixels != nullptr)
	{
		VkDeviceSize imageSize = m_Width * m_Height * m_Components;

		auto stagingBuffer = Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void *data;
		stagingBuffer.MapMemory(&data);
		std::memcpy(data, m_LoadPixels.get(), stagingBuffer.GetSize());
		stagingBuffer.UnmapMemory();

		Image::CopyBufferToImage(stagingBuffer.GetBuffer(), m_Image, { m_Width, m_Height, 1 }, 6, 0);
	}

	if (m_Mipmap)
	{
		Image::CreateMipmaps(m_Image, { m_Width, m_Height, 1 }, m_Format, m_Layout, m_MipLevels, 0, 6);
	}
	else if (m_LoadPixels != nullptr)
	{
		Image::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_Layout, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels, 0, 6, 0);
	}
	else
	{
		Image::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, m_Layout, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels, 0, 6, 0);
	}

	m_LoadPixels = nullptr;
}

std::unique_ptr<uint8_t[]> ImageCube::GetPixels(VkExtent3D& extent, const uint32_t& mipLevel,
	const uint32_t& arrayLayer) const
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	extent.width = int32_t(m_Width >> mipLevel);
	extent.height = int32_t(m_Height >> mipLevel);
	extent.depth = 1;

	VkImage dstImage;
	VkDeviceMemory dstImageMemory;
	Image::CopyImage(m_Image, dstImage, dstImageMemory, m_Format, extent, m_Layout, mipLevel, arrayLayer);

	VkImageSubresource dstImageSubresource = {};
	dstImageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	dstImageSubresource.mipLevel = 0;
	dstImageSubresource.arrayLayer = 0;

	VkSubresourceLayout dstSubresourceLayout;
	vkGetImageSubresourceLayout(*logicalDevice, dstImage, &dstImageSubresource, &dstSubresourceLayout);

	auto result = std::make_unique<uint8_t[]>(dstSubresourceLayout.size);

	void *data;
	vkMapMemory(*logicalDevice, dstImageMemory, dstSubresourceLayout.offset, dstSubresourceLayout.size, 0, &data);
	std::memcpy(result.get(), data, static_cast<size_t>(dstSubresourceLayout.size));
	vkUnmapMemory(*logicalDevice, dstImageMemory);

	vkFreeMemory(*logicalDevice, dstImageMemory, nullptr);
	vkDestroyImage(*logicalDevice, dstImage, nullptr);

	return result;
}

std::unique_ptr<uint8_t[]> ImageCube::GetPixels(VkExtent3D& extent, const uint32_t& mipLevel) const
{
	std::unique_ptr<uint8_t[]> pixels = nullptr;
	uint8_t *offset = nullptr;

	for (uint32_t i = 0; i < 6; i++)
	{
		auto resultSide = GetPixels(extent, mipLevel, i);
		int32_t sizeSide = extent.width * extent.height * m_Components;

		if (pixels == nullptr)
		{
			pixels = std::make_unique<uint8_t[]>(sizeSide * 6);
			offset = pixels.get();
		}

		memcpy(offset, resultSide.get(), sizeSide);
		offset += sizeSide;
	}

	extent.height *= 6;
	return pixels;
}

void ImageCube::SetPixels(const uint8_t* pixels, const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	VkDeviceSize imageSize = m_Width * m_Height * m_Components * 6;
	auto stagingBuffer = Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void *data;
	stagingBuffer.MapMemory(&data);
	std::memcpy(data, pixels, stagingBuffer.GetSize());
	stagingBuffer.UnmapMemory();


	Image::CopyBufferToImage(stagingBuffer.GetBuffer(), m_Image, { m_Width, m_Height, 1 }, layerCount, baseArrayLayer);
}

std::unique_ptr<uint8_t[]> ImageCube::LoadPixels(const std::string& filename, const std::string& fileSuffix,
	const std::vector<std::string>& fileSides, uint32_t& width, uint32_t& height, uint32_t& components,
	VkFormat& format)
{
	std::unique_ptr<uint8_t[]> result = nullptr;
	uint8_t *offset = nullptr;

	for (const auto &side : fileSides)
	{
		auto filenameSide = std::string(filename).append("/").append(side).append(fileSuffix);
		auto resultSide = Image::LoadPixels(filenameSide, width, height, components, format);
		const int32_t sizeSide = width * height * components;

		if (result == nullptr)
		{
			result = std::make_unique<uint8_t[]>(sizeSide * fileSides.size());
			offset = result.get();
		}

		memcpy(offset, resultSide.get(), sizeSide);
		offset += sizeSide;
	}

	return result;
}
}
