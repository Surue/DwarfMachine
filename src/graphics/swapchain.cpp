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

#include <graphics/swapchain.h>
#include <graphics/graphic_manager.h>

namespace dm
{
	static const std::vector<VkCompositeAlphaFlagBitsKHR> COMPOSITE_ALPHA_FLAGS = { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR, };

Swapchain::Swapchain(const VkExtent2D& extent) :
	m_Extent(extent),
	m_PresentMode(VK_PRESENT_MODE_FIFO_KHR),
	m_ImageCount(0),
	m_PreTransform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
	m_CompositeAlpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR),
	m_Swapchain(VK_NULL_HANDLE),
	m_FenceImage(VK_NULL_HANDLE),
	m_ActiveImageIndex(std::numeric_limits<uint32_t>::max())
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	const auto physicalDevice = Engine::Get()->GetGraphicManager()->GetPhysicalDevice();
	const auto surface = Engine::Get()->GetGraphicManager()->GetSurface();

	const auto surfaceFormat = surface->GetFormat();
	const auto surfaceCapabilities = surface->GetCapabilities();
	const auto graphicsFamily = logicalDevice->GetGraphicsFamily();
	const auto presentFamily = logicalDevice->GetPresentFamily();

	uint32_t physicalPresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *surface, &physicalPresentModeCount, nullptr);
	std::vector<VkPresentModeKHR> physicalPresentModes(physicalPresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *surface, &physicalPresentModeCount, physicalPresentModes.data());

	for (const auto &presentMode : physicalPresentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			m_PresentMode = presentMode;
			break;
		}

		if (m_PresentMode != VK_PRESENT_MODE_MAILBOX_KHR && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			m_PresentMode = presentMode;
		}
	}

	uint32_t desiredImageCount = surfaceCapabilities.minImageCount + 1;

	if (surfaceCapabilities.maxImageCount > 0 && desiredImageCount > surfaceCapabilities.maxImageCount)
	{
		desiredImageCount = surfaceCapabilities.maxImageCount;
	}

	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		m_PreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		m_PreTransform = surfaceCapabilities.currentTransform;
	}

	for(const auto &compositeAlphaFlag : COMPOSITE_ALPHA_FLAGS)
	{
		if(surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
		{
			m_CompositeAlpha = compositeAlphaFlag;
			break;
		}
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *surface;
	createInfo.minImageCount = desiredImageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = m_Extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(m_PreTransform);
	createInfo.compositeAlpha = m_CompositeAlpha;
	createInfo.presentMode = m_PresentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	if (Engine::Get()->GetGraphicManager()->GetSwapchain() != nullptr)
	{
		createInfo.oldSwapchain = Engine::Get()->GetGraphicManager()->GetSwapchain()->GetSwapchain();
	}

	if (graphicsFamily != presentFamily)
	{
		std::array<uint32_t, 2> queueFamily = { graphicsFamily, presentFamily };
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
		createInfo.pQueueFamilyIndices = queueFamily.data();
	}

	if (vkCreateSwapchainKHR(*logicalDevice, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(*logicalDevice, m_Swapchain, &m_ImageCount, nullptr);
	m_Images.resize(m_ImageCount);
	m_ImageViews.resize(m_ImageCount);
	vkGetSwapchainImagesKHR(*logicalDevice, m_Swapchain, &m_ImageCount, m_Images.data());

	for(uint32_t i = 0; i < m_ImageCount; i++)
	{
		Image::CreateImageView(m_Images.at(i), m_ImageViews.at(i), VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);
	}

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(*logicalDevice, &fenceCreateInfo, nullptr, &m_FenceImage);
}

Swapchain::~Swapchain()
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	vkDestroySwapchainKHR(*logicalDevice, m_Swapchain, nullptr);

	for(const auto &imageView : m_ImageViews)
	{
		vkDestroyImageView(*logicalDevice, imageView, nullptr);
	}

	vkDestroyFence(*logicalDevice, m_FenceImage, nullptr);
}

VkResult Swapchain::AcquireNextImage(const VkSemaphore& presentCompleteSemaphore)
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	VkResult acquireResult = vkAcquireNextImageKHR(*logicalDevice, m_Swapchain, std::numeric_limits<uint64_t>::max(), presentCompleteSemaphore, VK_NULL_HANDLE, &m_ActiveImageIndex);

	if(acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image");
		return acquireResult;
	}

	//vkWaitForFences(*logicalDevice, 1, &m_FenceImage, VK_TRUE, std::numeric_limits<uint64_t>::max());

	//vkResetFences(*logicalDevice, 1, &m_FenceImage);

	return VK_SUCCESS;
}

VkResult Swapchain::QueuePresent(const VkQueue& presentQueue, const VkSemaphore& waitSemaphore) const
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &waitSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_Swapchain;
	presentInfo.pImageIndices = &m_ActiveImageIndex;
	return vkQueuePresentKHR(presentQueue, &presentInfo);
}
}
