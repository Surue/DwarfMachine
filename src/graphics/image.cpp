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
#include <graphics/buffers/buffer.h>
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
	auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

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

std::unique_ptr<uint8_t[]> Image::GetPixels(VkExtent3D& extent, const uint32_t& mipLevel,
	const uint32_t& arrayLayer) const
{
	auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

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
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.flags = arrayLayers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCreateInfo.imageType = type;
	imageCreateInfo.format = format;
	imageCreateInfo.extent = extent;
	imageCreateInfo.mipLevels = mipLevels;
	imageCreateInfo.arrayLayers = arrayLayers;
	imageCreateInfo.samples = samples;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.usage = usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	GraphicManager::CheckVk(vkCreateImage(*logicalDevice, &imageCreateInfo, nullptr, &image));

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(*logicalDevice, image, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = Buffer::FindMemoryType(memoryRequirements.memoryTypeBits, properties);
	GraphicManager::CheckVk(vkAllocateMemory(*logicalDevice, &memoryAllocateInfo, nullptr, &memory));

	GraphicManager::CheckVk(vkBindImageMemory(*logicalDevice, image, memory, 0));
}

void Image::CreateImageSampler(VkSampler& sampler, const VkFilter& filter, const VkSamplerAddressMode& addressMode,
	const bool& anisotropic, const uint32_t& mipLevels)
{
	const auto physicalDevice = GraphicManager::Get()->GetPhysicalDevice();
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = filter;
	samplerCreateInfo.minFilter = filter;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = addressMode;
	samplerCreateInfo.addressModeV = addressMode;
	samplerCreateInfo.addressModeW = addressMode;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = static_cast<VkBool32>(anisotropic);
	samplerCreateInfo.maxAnisotropy =
		(anisotropic && logicalDevice->GetEnabledFeatures().samplerAnisotropy) ? std::min(ANISOTROPY, physicalDevice->GetProperties().limits.maxSamplerAnisotropy) : 1.0f;
	//samplerCreateInfo.compareEnable = VK_FALSE;
	//samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = static_cast<float>(mipLevels);
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	GraphicManager::CheckVk(vkCreateSampler(*logicalDevice, &samplerCreateInfo, nullptr, &sampler));
}

void Image::CreateImageView(const VkImage& image, VkImageView& imageView, const VkImageViewType& type,
	const VkFormat& format, const VkImageAspectFlags& imageAspect, const uint32_t& mipLevels,
	const uint32_t& baseMipLevel, const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = type;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	imageViewCreateInfo.subresourceRange.aspectMask = imageAspect;
	imageViewCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
	imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
	imageViewCreateInfo.subresourceRange.layerCount = layerCount;
	GraphicManager::CheckVk(vkCreateImageView(*logicalDevice, &imageViewCreateInfo, nullptr, &imageView));
}

void Image::CreateMipmaps(const VkImage& image, const VkExtent3D& extent, const VkFormat& format,
	const VkImageLayout& dstImageLayout, const uint32_t& mipLevels, const uint32_t& baseArrayLayer,
	const uint32_t& layerCount)
{
	auto physicalDevice = GraphicManager::Get()->GetPhysicalDevice();

	// Get device properites for the requested texture format.
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(*physicalDevice, format, &formatProperties);

	// Mip-chain generation requires support for blit source and destination
	assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
	assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);

	CommandBuffer commandBuffer = CommandBuffer();

	for (uint32_t i = 1; i < mipLevels; i++)
	{
		VkImageMemoryBarrier barrier0 = {};
		barrier0.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier0.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier0.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier0.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier0.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier0.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier0.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier0.image = image;
		barrier0.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier0.subresourceRange.baseMipLevel = i - 1;
		barrier0.subresourceRange.levelCount = 1;
		barrier0.subresourceRange.baseArrayLayer = baseArrayLayer;
		barrier0.subresourceRange.layerCount = layerCount;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier0);

		VkImageBlit imageBlit = {};
		imageBlit.srcOffsets[1] = { int32_t(extent.width >> (i - 1)), int32_t(extent.height >> (i - 1)), 1 };
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.mipLevel = i - 1;
		imageBlit.srcSubresource.baseArrayLayer = baseArrayLayer;
		imageBlit.srcSubresource.layerCount = layerCount;
		imageBlit.dstOffsets[1] = { int32_t(extent.width >> i), int32_t(extent.height >> i), 1 };
		imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.mipLevel = i;
		imageBlit.dstSubresource.baseArrayLayer = baseArrayLayer;
		imageBlit.dstSubresource.layerCount = layerCount;
		vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);

		VkImageMemoryBarrier barrier1 = {};
		barrier1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier1.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier1.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier1.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier1.newLayout = dstImageLayout;
		barrier1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier1.image = image;
		barrier1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier1.subresourceRange.baseMipLevel = i - 1;
		barrier1.subresourceRange.levelCount = 1;
		barrier1.subresourceRange.baseArrayLayer = baseArrayLayer;
		barrier1.subresourceRange.layerCount = layerCount;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier1);
	}

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
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	commandBuffer.SubmitIdle();
}

void Image::TransitionImageLayout(const VkImage& image, const VkFormat& format, const VkImageLayout& srcImageLayout,
	const VkImageLayout& dstImageLayout, const VkImageAspectFlags& imageAspect, const uint32_t& mipLevels,
	const uint32_t& baseMipLevel, const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	CommandBuffer commandBuffer = CommandBuffer();

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout = srcImageLayout;
	imageMemoryBarrier.newLayout = dstImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = imageAspect;
	imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
	imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	imageMemoryBarrier.subresourceRange.layerCount = layerCount;

	// Source access mask controls actions that have to be finished on the old layout before it will be transitioned to the new layout.
	switch (srcImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		imageMemoryBarrier.srcAccessMask = 0;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		//throw std::runtime_error("Unsupported image layout transition source");
		break;
	}

	// Destination access mask controls the dependency for the new image layout.
	switch (dstImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		if (imageMemoryBarrier.srcAccessMask == 0)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}

		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		//throw std::runtime_error("Unsupported image layout transition destination");
		break;
	}

	VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	commandBuffer.SubmitIdle();
}

void Image::InsertImageMemoryBarrier(const CommandBuffer& commandBuffer, const VkImage& image,
	const VkAccessFlags& srcAccessMask, const VkAccessFlags& dstAccessMask, const VkImageLayout& oldImageLayout,
	const VkImageLayout& newImageLayout, const VkPipelineStageFlags& srcStageMask,
	const VkPipelineStageFlags& dstStageMask, const VkImageAspectFlags& imageAspect, const uint32_t& mipLevels,
	const uint32_t& baseMipLevel, const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstAccessMask = dstAccessMask;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = imageAspect;
	imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
	imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	imageMemoryBarrier.subresourceRange.layerCount = layerCount;
	vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void Image::CopyBufferToImage(const VkBuffer& buffer, const VkImage& image, const VkExtent3D& extent,
	const uint32_t& layerCount, const uint32_t& baseArrayLayer)
{
	CommandBuffer commandBuffer = CommandBuffer();

	VkBufferImageCopy region = {};
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
	auto physicalDevice = GraphicManager::Get()->GetPhysicalDevice();
	auto surface = GraphicManager::Get()->GetSurface();

	// Checks blit swapchain support.
	bool supportsBlit = true;
	VkFormatProperties formatProperties;

	// Check if the device supports blitting from optimal images (the swapchain images are in optimal format).
	vkGetPhysicalDeviceFormatProperties(*physicalDevice, surface->GetFormat().format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
	{
		std::cout << "Device does not support blitting from optimal tiled images, using copy instead of blit!\n";
		supportsBlit = false;
	}

	// Check if the device supports blitting to linear images.
	vkGetPhysicalDeviceFormatProperties(*physicalDevice, srcFormat, &formatProperties);

	if (!(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
	{
		std::cout << "Device does not support blitting to linear tiled images, using copy instead of blit!\n";
		supportsBlit = false;
	}

	CreateImage(dstImage, dstImageMemory, extent, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 1, 1, VK_IMAGE_TYPE_2D);

	// Do the actual blit from the swapchain image to our host visible destination image.
	CommandBuffer commandBuffer = CommandBuffer();

	// Transition destination image to transfer destination layout.
	InsertImageMemoryBarrier(commandBuffer, dstImage, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);

	// Transition image from previous usage to transfer source layout
	InsertImageMemoryBarrier(commandBuffer, srcImage, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT, srcImageLayout,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevel, 1, arrayLayer);

	// If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB).
	if (supportsBlit)
	{
		// Define the region to blit (we will blit the whole swapchain image).
		VkOffset3D blitSize = { static_cast<int32_t>(extent.width), static_cast<int32_t>(extent.height), static_cast<int32_t>(extent.depth) };
		VkImageBlit imageBlitRegion = {};
		imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlitRegion.srcSubresource.mipLevel = mipLevel;
		imageBlitRegion.srcSubresource.baseArrayLayer = arrayLayer;
		imageBlitRegion.srcSubresource.layerCount = 1;
		imageBlitRegion.srcOffsets[1] = blitSize;
		imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlitRegion.dstSubresource.mipLevel = 0;
		imageBlitRegion.dstSubresource.baseArrayLayer = 0;
		imageBlitRegion.dstSubresource.layerCount = 1;
		imageBlitRegion.dstOffsets[1] = blitSize;
		vkCmdBlitImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlitRegion,
			VK_FILTER_NEAREST);
	}
	else
	{
		// Otherwise use image copy (requires us to manually flip components).
		VkImageCopy imageCopyRegion = {};
		imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopyRegion.srcSubresource.mipLevel = mipLevel;
		imageCopyRegion.srcSubresource.baseArrayLayer = arrayLayer;
		imageCopyRegion.srcSubresource.layerCount = 1;
		imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopyRegion.dstSubresource.mipLevel = 0;
		imageCopyRegion.dstSubresource.baseArrayLayer = 0;
		imageCopyRegion.dstSubresource.layerCount = 1;
		imageCopyRegion.extent = extent;
		vkCmdCopyImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
	}

	// Transition destination image to general layout, which is the required layout for mapping the image memory later on.
	InsertImageMemoryBarrier(commandBuffer, dstImage, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);

	// Transition back the image after the blit is done.
	InsertImageMemoryBarrier(commandBuffer, srcImage, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		srcImageLayout, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevel, 1, arrayLayer);

	commandBuffer.SubmitIdle();

	return supportsBlit;
}
}
