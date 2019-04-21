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
#include <graphics/buffer.h>
#include <graphics/graphic_manager.h>
#include <engine/file.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Utility/stb_image.h>
#include <iostream>

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
	m_ArrayLayers(arrayLayers),
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

void Image::SetPixels(const uint8_t* pixels, const uint32_t& layerCount, const uint32_t& baseArrayLayer) const
{
	VkDeviceSize imageSize = m_Extent.width * m_Extent.height * 4;
	auto stagingBuffer = Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void *data;
	stagingBuffer.MapMemory(&data);
	std::memcpy(data, pixels, stagingBuffer.GetSize());
	stagingBuffer.UnmapMemory();


	CopyBufferToImage(stagingBuffer.GetBuffer(), m_Image, m_Extent, layerCount, baseArrayLayer);
}

std::unique_ptr<uint8_t[]> Image::LoadPixels(const std::string& filename, uint32_t& width, uint32_t& height,
	uint32_t& components, VkFormat& format)
{
	auto file = Files::Read(filename);

	if(!file)
	{
		throw std::runtime_error("image could not be loaded");
	}

	std::unique_ptr<uint8_t[]> pixels(stbi_load_from_memory(reinterpret_cast<uint8_t*>(file->data()), static_cast<uint32_t>(file->size()), reinterpret_cast<int32_t*>(&width), reinterpret_cast<int32_t*>(&height), reinterpret_cast<int32_t*>(&components), STBI_rgb_alpha));

	components = 4;
	format = VK_FORMAT_R8G8B8A8_UNORM;

	if(pixels == nullptr)
	{
		std::cout << "Unable to load image\n";
	}

	return pixels;
}

void Image::WritePixels(const std::string& filename, const uint8_t* pixels, const int32_t& width,
	const uint32_t& height, const int32_t& components)
{
	throw std::runtime_error("Methode Write Pixels in Image.cpp not implemented yet");
}

uint32_t Image::GetMipLevels(const VkExtent3D& extent)
{
	return static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, std::max(extent.height, extent.depth)))) + 1);
}

bool Image::HasDepth(const VkFormat& format)
{
	static const std::vector<VkFormat> DEPTH_FORMAT = { VK_FORMAT_D16_UNORM, VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT };

	return std::find(DEPTH_FORMAT.begin(), DEPTH_FORMAT.end(), format) != std::end(DEPTH_FORMAT);
}

bool Image::HasStencil(const VkFormat& format)
{
	static const std::vector<VkFormat> STENCIL_FORMATS = { VK_FORMAT_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT };
	return std::find(STENCIL_FORMATS.begin(), STENCIL_FORMATS.end(), format) != std::end(STENCIL_FORMATS);
}

void Image::CreateImage(VkImage& image, VkDeviceMemory& memory, const VkExtent3D& extent, const VkFormat& format,
	const VkSampleCountFlagBits& samples, const VkImageTiling& tiling, const VkImageUsageFlags& usage,
	const VkMemoryPropertyFlags& properties, const uint32_t& mipLevels, const uint32_t& arrayLayers,
	const VkImageType& type)
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = arrayLayers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageInfo.imageType = type;
	imageInfo.extent = extent;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = arrayLayers;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = samples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(*logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(*logicalDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = Buffer::FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(*logicalDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(*logicalDevice, image, memory, 0);
}

void Image::CreateImageSampler(VkSampler& sampler, const VkFilter& filter, const VkSamplerAddressMode& addressMode,
	const bool& anisotropic, const uint32_t& mipLevels)
{
	const auto physicalDevice = Engine::Get()->GetGraphicManager()->GetPhysicalDevice();
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	VkSamplerCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.magFilter = filter;
	createInfo.minFilter = filter;
	createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	createInfo.addressModeU = addressMode;
	createInfo.addressModeV = addressMode;
	createInfo.addressModeW = addressMode;
	createInfo.mipLodBias = 0.0f;
	createInfo.anisotropyEnable = static_cast<VkBool32>(anisotropic);
	createInfo.maxAnisotropy = (anisotropic & logicalDevice->GetEnabledFeatures().samplerAnisotropy) ? std::min(ANISOTROPY, physicalDevice->GetProperties().limits.maxSamplerAnisotropy) : 1.0f;
	createInfo.minLod = 0.0f;
	createInfo.maxLod = static_cast<float>(mipLevels);
	createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	createInfo.unnormalizedCoordinates = VK_FALSE;

	if (vkCreateSampler(*logicalDevice, &createInfo, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create sampler image");
	}
}

void Image::CreateImageView(const VkImage& image, VkImageView& imageView, const VkImageViewType& type,
	const VkFormat& format, const VkImageAspectFlags& imageAspect, const uint32_t& mipLevels,
	const uint32_t& baseMipLevel, const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = type;
	viewInfo.format = format;
	viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	viewInfo.subresourceRange.aspectMask = imageAspect;
	viewInfo.subresourceRange.baseMipLevel = baseMipLevel;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
	viewInfo.subresourceRange.layerCount = layerCount;

	if (vkCreateImageView(*logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
}

void Image::CreateMipmaps(const VkImage& image, const VkExtent3D& extent, const VkFormat& format,
	const VkImageLayout& dstImageLayout, const uint32_t& mipLevels, const uint32_t& baseArrayLayer,
	const uint32_t& layerCount)
{
	auto physicalDevice = Engine::Get()->GetGraphicManager()->GetPhysicalDevice();

	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(*physicalDevice, format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		throw std::runtime_error("texture image format does not support linear blitting");
	}

	auto commandBuffer = CommandBuffer();

	for (uint32_t i = 1; i < mipLevels; i++)
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
		barrier.subresourceRange.layerCount = layerCount;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
		
		VkImageBlit blit = {};
		blit.srcOffsets[1] = { int32_t(extent.width >> (i - 1)), int32_t(extent.height >> (i - 1)), 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = baseArrayLayer;
		blit.srcSubresource.layerCount = layerCount;
		blit.dstOffsets[1] = { int32_t(extent.width >> i), int32_t(extent.height >> i), 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = baseArrayLayer;
		blit.dstSubresource.layerCount = layerCount;

		vkCmdBlitImage(commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = dstImageLayout;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = dstImageLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
		barrier.subresourceRange.layerCount = layerCount;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		commandBuffer.SubmitIdle();
	}
}

void Image::TransitionImageLayout(const VkImage& image, const VkFormat& format, const VkImageLayout& srcImageLayout,
	const VkImageLayout& dstImageLayout, const VkImageAspectFlags& imageAspect, const uint32_t& mipLevels,
	const uint32_t& baseMipLevel, const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	auto commandBuffer = CommandBuffer();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = srcImageLayout;
	barrier.newLayout = dstImageLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = imageAspect;
	barrier.subresourceRange.baseMipLevel = baseMipLevel;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	barrier.subresourceRange.layerCount = layerCount;

	switch (srcImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		barrier.srcAccessMask = 0;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		throw std::runtime_error("Unsupported image layout transition source");
		break;
	}

	// Destination access mask controls the dependency for the new image layout.
	switch (dstImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		if (barrier.srcAccessMask == 0)
		{
			barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}

		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		throw std::runtime_error("Unsupported image layout transition destination");
		break;
	}

	const VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	const VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	commandBuffer.SubmitIdle();
}

void Image::InsertImageMemoryBarrier(const CommandBuffer& commandBuffer, const VkImage& image,
	const VkAccessFlags& srcAccessMask, const VkAccessFlags& dstAccessMask, const VkImageLayout& oldImageLayout,
	const VkImageLayout& newImageLayout, const VkPipelineStageFlags& srcStageMask,
	const VkPipelineStageFlags& dstStageMask, const VkImageAspectFlags& imageAspect, const uint32_t& mipLevels,
	const uint32_t& baseMipLevel, const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;
	barrier.oldLayout = oldImageLayout;
	barrier.newLayout = newImageLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = imageAspect;
	barrier.subresourceRange.baseMipLevel = baseMipLevel;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	barrier.subresourceRange.layerCount = layerCount;
	vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Image::CopyBufferToImage(const VkBuffer& buffer, const VkImage& image, const VkExtent3D& extent,
	const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	auto commandBuffer = CommandBuffer();

	VkBufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = baseArrayLayer;
	region.imageSubresource.layerCount = layerCount;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = extent;

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandBuffer.SubmitIdle();
}

bool Image::CopyImage(const VkImage& srcImage, VkImage& dstImage, VkDeviceMemory& dstImageMemory,
	const VkFormat& srcFormat, const VkExtent3D& extent, const VkImageLayout& srcImageLayout, const uint32_t& mipLevel,
	const uint32_t& arrayLayer)
{
	auto physicalDevice = Engine::Get()->GetGraphicManager()->GetPhysicalDevice();
	auto surface = Engine::Get()->GetGraphicManager()->GetSurface();

	bool supportBlit = true;
	VkFormatProperties formatProperties;

	vkGetPhysicalDeviceFormatProperties(*physicalDevice, surface->GetFormat().format, &formatProperties);

	if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
	{
		std::cout << "device does not support blitting from optimal tiled image, using copy instead of blit\n";
		supportBlit = false;
	}

	vkGetPhysicalDeviceFormatProperties(*physicalDevice, srcFormat, &formatProperties);

	if(!(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
	{
		std::cout << "device does not support blitting to linear tiled images, using copy instead of blit\n";
		supportBlit = false;
	}

	CreateImage(dstImage, dstImageMemory, extent, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 1, 1, VK_IMAGE_TYPE_2D);

	CommandBuffer commandBuffer = CommandBuffer();

	InsertImageMemoryBarrier(commandBuffer, dstImage, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevel, 1, arrayLayer);

	if(supportBlit)
	{
		VkOffset3D blitSize = { static_cast<int32_t>(extent.width), static_cast<int32_t>(extent.height), static_cast<int32_t>(extent.depth) };
		VkImageBlit blitRegion = { };
		blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.srcSubresource.mipLevel = mipLevel;
		blitRegion.srcSubresource.baseArrayLayer = arrayLayer;
		blitRegion.srcSubresource.layerCount = 1;
		blitRegion.srcOffsets[1] = blitSize;
		blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.dstSubresource.mipLevel = 0;
		blitRegion.dstSubresource.baseArrayLayer = 0;
		blitRegion.dstSubresource.layerCount = 1;
		blitRegion.dstOffsets[1] = blitSize;
		vkCmdBlitImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_NEAREST);
	}else
	{
		VkImageCopy copyRegion = {};
		copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.srcSubresource.mipLevel = mipLevel;
		copyRegion.srcSubresource.baseArrayLayer = arrayLayer;
		copyRegion.srcSubresource.layerCount = 1;
		copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.dstSubresource.mipLevel = 0;
		copyRegion.dstSubresource.baseArrayLayer = 0;
		copyRegion.dstSubresource.layerCount = 1;
		copyRegion.extent = extent;
		vkCmdCopyImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
	}

	InsertImageMemoryBarrier(commandBuffer, dstImage, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);

	InsertImageMemoryBarrier(commandBuffer, srcImage, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcImageLayout, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevel, 1, arrayLayer);

	commandBuffer.SubmitIdle();

	return supportBlit;
}
}
