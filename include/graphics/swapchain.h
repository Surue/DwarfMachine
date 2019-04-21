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

#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

namespace dm
{
class Swapchain
{
public:
	explicit Swapchain(const VkExtent2D &extent);

	~Swapchain();

	VkResult AcquireNextImage(const VkSemaphore &presentCompleteSemaphore = VK_NULL_HANDLE);

	VkResult QueuePresent(const VkQueue &presentQueue, const VkSemaphore &waitSemaphore = VK_NULL_HANDLE) const;

	const VkExtent2D &GetExtend() const { return m_Extent; }

	const uint32_t &GetImageCount() const { return m_ImageCount; }

	const VkSurfaceTransformFlagsKHR & GetPreTransform() const { return m_PreTransform; }

	const VkCompositeAlphaFlagBitsKHR &GetCompositeAlpha() const { return m_CompositeAlpha; }

	const std::vector<VkImage> &GetImages() const { return m_Images; }

	const VkImage &GetActiveImage() const { return m_Images[m_ActiveImageIndex]; }

	const std::vector<VkImageView> &GetImageViews() const { return m_ImageViews; }

	const VkSwapchainKHR &GetSwapchain() const { return m_Swapchain; }

	const uint32_t &GetActiveImageIndex() const { return m_ActiveImageIndex; }

	bool IsSameExtent(const VkExtent2D &extent2D) const { return m_Extent.width == extent2D.width && m_Extent.height == extent2D.height; }
private:
	VkExtent2D m_Extent;
	VkPresentModeKHR m_PresentMode;

	uint32_t m_ImageCount;
	VkSurfaceTransformFlagsKHR m_PreTransform;
	VkCompositeAlphaFlagBitsKHR m_CompositeAlpha;
	std::vector<VkImage> m_Images;
	std::vector<VkImageView> m_ImageViews;
	VkSwapchainKHR m_Swapchain;

	VkFence m_FenceImage;
	uint32_t m_ActiveImageIndex;
};
}

#endif SWAPCHAIN_H