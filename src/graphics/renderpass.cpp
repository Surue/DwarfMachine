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

#include <graphics/renderpass.h>
#include <graphics/render_stage.h>
#include <engine/engine.h>
#include <graphics/graphic_manager.h>
#include <iostream>

namespace dm
{
RenderPass::RenderPass(const RenderStage& renderStage, const VkFormat& depthFormat, const VkFormat& surfaceFormat,
	const VkSampleCountFlagBits& samples) :
	m_RenderPass(VK_NULL_HANDLE)
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	std::vector<VkAttachmentDescription> attachmentDescriptions;

	for (const auto &attachment : renderStage.GetAttachments())
	{
		auto attachementSamples = attachment.IsMultisampled() ? samples : VK_SAMPLE_COUNT_1_BIT;
		VkAttachmentDescription attachmentDescription = {};
		attachmentDescription.samples = attachementSamples;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		switch(attachment.GetType())
		{
		case Attachment::Type::IMAGE: 
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachmentDescription.format = attachment.GetFormat();
			break;
		case Attachment::Type::DEPTH: 
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachmentDescription.format = depthFormat;
			break;
		case Attachment::Type::SWAPCHAIN: 
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			attachmentDescription.format = surfaceFormat;
			break;
		default: ;
		}

		attachmentDescriptions.emplace_back(attachmentDescription);
	}

	std::vector<std::unique_ptr<SubpassDescription>> subpasses;
	std::vector<VkSubpassDependency> dependencies;

	for (const auto subpassType : renderStage.GetSubpasses())
	{
		std::vector<VkAttachmentReference> subpassColorAttachments;

		std::optional<uint32_t> depthAttachment;

		for(const auto &attachmentBinding : subpassType.GetAttachmentBindings())
		{
			auto attachment = renderStage.GetAttachment(attachmentBinding);

			if(!attachment)
			{
				std::cout << "Failed to find a renderpass attachment bound to " << attachmentBinding << "\n";
				continue;
			}

			if(attachment->GetType() == Attachment::Type::DEPTH)
			{
				depthAttachment = attachment->GetBinding();
				continue;
			}

			VkAttachmentReference attachmentReference = {};
			attachmentReference.attachment = attachment->GetBinding();
			attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			subpassColorAttachments.emplace_back(attachmentReference);
		}

		subpasses.emplace_back(std::make_unique<SubpassDescription>(VK_PIPELINE_BIND_POINT_GRAPHICS, subpassColorAttachments, depthAttachment));

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		if(subpassType.GetBinding() == renderStage.GetSubpasses().size())
		{
			subpassDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		}else
		{
			subpassDependency.dstSubpass = subpassType.GetBinding();
		}

		if(subpassType.GetBinding() == 0)
		{
			subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}else
		{
			subpassDependency.srcSubpass = subpassType.GetBinding() - 1;
		}

		dependencies.emplace_back(subpassDependency);
	}

	std::vector<VkSubpassDescription> subpassDescription;
	subpassDescription.reserve(subpasses.size());

	for(const auto &subpass : subpasses)
	{
		subpassDescription.emplace_back(subpass->GetSubpassDescription());
	}

	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	createInfo.pAttachments = attachmentDescriptions.data();
	createInfo.subpassCount = static_cast<uint32_t>(subpassDescription.size());
	createInfo.pSubpasses = subpassDescription.data();
	createInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	createInfo.pDependencies = dependencies.data();


	if (vkCreateRenderPass(*logicalDevice, &createInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass");
	}
}

RenderPass::~RenderPass()
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	vkDestroyRenderPass(*logicalDevice, m_RenderPass, nullptr);
}
}
