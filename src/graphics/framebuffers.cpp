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

#include <graphics/framebuffers.h>
#include <graphics/graphic_manager.h>
#include <graphics/render_stage.h>
#include <graphics/renderpass.h>

namespace dm
{
Framebuffers::Framebuffers(const uint32_t& width, const uint32_t& height, const RenderStage& renderStage,
	const RenderPass& renderpass, const Swapchain& swapchain, const ImageDepth& depthStencil,
	const VkSampleCountFlagBits& samples)
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	for (const auto &attachment : renderStage.GetAttachments())
	{
		auto attachmentSamples = attachment.IsMultisampled() ? samples : VK_SAMPLE_COUNT_1_BIT;

		switch (attachment.GetType())
		{
		case Attachment::Type::IMAGE:
			m_ImageAttachments.emplace_back(std::make_unique<Image2d>(width, height, nullptr, attachment.GetFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, attachmentSamples));
			break;
		case Attachment::Type::DEPTH:
			m_ImageAttachments.emplace_back(nullptr);
			break;
		case Attachment::Type::SWAPCHAIN:
			m_ImageAttachments.emplace_back(nullptr);
			break;
		}
	}

	m_Framebuffers.resize(swapchain.GetImageCount());

	for (uint32_t i = 0; i < swapchain.GetImageCount(); i++)
	{
		std::vector<VkImageView> attachments;

		for (const auto &attachment : renderStage.GetAttachments())
		{
			switch (attachment.GetType())
			{
			case Attachment::Type::IMAGE:
				attachments.emplace_back(GetAttachment(attachment.GetBinding())->GetView());
				break;
			case Attachment::Type::DEPTH:
				attachments.emplace_back(depthStencil.GetView());
				break;
			case Attachment::Type::SWAPCHAIN:
				attachments.emplace_back(swapchain.GetImageViews().at(i));
				break;
			}
		}

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderpass.GetRenderPass();
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = width;
		createInfo.height = height;
		createInfo.layers = 1;

		if (vkCreateFramebuffer(*logicalDevice, &createInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

Framebuffers::~Framebuffers()
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	for(const auto &framebuffer : m_Framebuffers)
	{
		vkDestroyFramebuffer(*logicalDevice, framebuffer, nullptr);
	}
}
}
