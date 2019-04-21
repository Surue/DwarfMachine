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

#include <graphics/image_depth.h>
#include <engine/engine.h>
#include <graphics/graphic_manager.h>

namespace dm
{
static const std::vector<VkFormat> TRY_FORMATS = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT,
	VK_FORMAT_D16_UNORM };

ImageDepth::ImageDepth(const uint32_t& width, const uint32_t& height, const VkSampleCountFlagBits& samples) :
	m_Width(width),
	m_Height(height),
	m_Image(VK_NULL_HANDLE),
	m_Memory(VK_NULL_HANDLE),
	m_Sampler(VK_NULL_HANDLE),
	m_View(VK_NULL_HANDLE),
	m_Format(VK_FORMAT_UNDEFINED)
{
	const auto physicalDevice = Engine::Get()->GetGraphicManager()->GetPhysicalDevice();

	for(const auto &format : TRY_FORMATS)
	{
		VkFormatProperties formatProperties = {};
		vkGetPhysicalDeviceFormatProperties(*physicalDevice, format, &formatProperties);

		if(formatProperties.optimalTilingFeatures &VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			m_Format = format;
			break;
		}
	}

	if(m_Format == VK_FORMAT_UNDEFINED)
	{
		throw std::runtime_error("No depth stencil format could be selected");
	}

	VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	if(Image::HasStencil(m_Format))
	{
		aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	Image::CreateImage(m_Image, m_Memory, { m_Width, m_Height, 1 }, m_Format, samples, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, 1, VK_IMAGE_TYPE_2D);
	Image::CreateImageSampler(m_Sampler, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, false, 1);
	Image::CreateImageView(m_Image, m_View, VK_IMAGE_VIEW_TYPE_2D, m_Format, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 0, 1, 0);
	Image::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, aspectMask, 1, 0, 1, 0);
}

ImageDepth::~ImageDepth()
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	vkDestroyImageView(*logicalDevice, m_View, nullptr);
	vkDestroySampler(*logicalDevice, m_Sampler, nullptr);
	vkFreeMemory(*logicalDevice, m_Memory, nullptr);
	vkDestroyImage(*logicalDevice, m_Image, nullptr);
}

VkDescriptorSetLayoutBinding ImageDepth::GetDescriptorSetLayout(const uint32_t& binding,
	const VkDescriptorType& descriptorType, const VkShaderStageFlags& stage)
{
	VkDescriptorSetLayoutBinding descriptorSetLayout = {};
	descriptorSetLayout.binding = binding;
	descriptorSetLayout.descriptorType = descriptorType;
	descriptorSetLayout.descriptorCount = 1;
	descriptorSetLayout.stageFlags = stage;
	descriptorSetLayout.pImmutableSamplers = nullptr;
	return descriptorSetLayout;
}

WriteDescriptorSet ImageDepth::GetWriteDescriptor(const uint32_t& binding, const VkDescriptorType& descriptorType,
	const std::optional<OffsetSize>& offsetSize) const
{
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.sampler = m_Sampler;
	imageInfo.imageView = m_View;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = VK_NULL_HANDLE;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = descriptorType;
	return WriteDescriptorSet(descriptorWrite, imageInfo);
}
}
