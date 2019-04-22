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

#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace dm
{
class ImageDepth;
class RenderStage;

class RenderPass
{
public:
	class SubpassDescription
	{
	public:
		SubpassDescription(const VkPipelineBindPoint &bindPoint, std::vector<VkAttachmentReference> colorAttachments, const std::optional<uint32_t> &depthAttachment) :
			m_SubpassDescription({}),
			m_ColorAttachments(std::move(colorAttachments)),
			m_DepthStencilAttachment({})
		{
			m_SubpassDescription.pipelineBindPoint = bindPoint;
			m_SubpassDescription.colorAttachmentCount = static_cast<uint32_t>(m_ColorAttachments.size());
			m_SubpassDescription.pColorAttachments = m_ColorAttachments.data();

			if(depthAttachment)
			{
				m_DepthStencilAttachment.attachment = *depthAttachment;
				m_DepthStencilAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				m_SubpassDescription.pDepthStencilAttachment = &m_DepthStencilAttachment;
			}
		}

		const VkSubpassDescription &GetSubpassDescription() const { return m_SubpassDescription; }
	private:
		VkSubpassDescription m_SubpassDescription;
		std::vector<VkAttachmentReference> m_ColorAttachments;
		VkAttachmentReference m_DepthStencilAttachment;
	};

	RenderPass(const RenderStage &renderStage, const VkFormat &depthFormat, const VkFormat &surfaceFormat, const VkSampleCountFlagBits &samples = VK_SAMPLE_COUNT_1_BIT);

	~RenderPass();

	const VkRenderPass &GetRenderPass() const { return m_RenderPass; }

private:
	VkRenderPass m_RenderPass;
};
}

#endif RENDERPASS_H