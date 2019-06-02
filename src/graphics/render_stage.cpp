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

#include <graphics/render_stage.h>
#include <graphics/graphic_manager.h>
#include <graphics/window.h>

namespace dm
{
RenderStage::RenderStage(std::vector<Attachment> images, std::vector<SubpassType> subpasses, const Viewport& viewport) :
	m_Attachments(std::move(images)),
	m_Subpasses(std::move(subpasses)),
	m_Viewport(viewport),
	m_Renderpass(nullptr),
	m_DepthStencil(nullptr),
	m_Framebuffers(nullptr),
	m_SubpassAttachmentCount(m_Subpasses.size()),
	m_DepthAttachment({}),
	m_SwapchainAttachment({}),
	m_SubpassMultisampled(m_Subpasses.size()),
	m_Size(0, 0),
	m_AspectRatio(1.0f),
	m_OutOfDate(false)
{
	for(const auto &image : m_Attachments)
	{
		VkClearValue clearValue = {};

		switch(image.GetType())
		{
		case Attachment::Type::IMAGE: 
			clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };

			for(const auto &subpass : m_Subpasses)
			{
				auto subpassBinding = subpass.GetAttachmentBindings();

				if(std::find(subpassBinding.begin(), subpassBinding.end(), image.GetBinding()) != subpassBinding.end())
				{
					m_SubpassAttachmentCount[subpass.GetBinding()]++;

					if(image.IsMultisampled())
					{
						m_SubpassMultisampled[subpass.GetBinding()] = true;
					}
				}
			}
			break;
		case Attachment::Type::DEPTH: 
			clearValue.color = { 1.0f, 0.0f };
			m_DepthAttachment = image;
			break;
		case Attachment::Type::SWAPCHAIN: 
			clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			m_SwapchainAttachment = image;
			break;
		default: ;
		}

		m_ClearValues.emplace_back(clearValue);
	}
}

void RenderStage::Update()
{
	glm::vec2 lastSize = m_Size;

	if(m_Viewport.GetSize())
	{
		m_Size = *m_Viewport.GetSize();
	}else
	{
		m_Size = GraphicManager::Get()->GetWindow()->GetSize();
	}

	m_Size = glm::vec2(m_Viewport.GetScale().x * m_Size.x, m_Viewport.GetScale().y * m_Size.y);
	m_AspectRatio = static_cast<float>(m_Size.x) / static_cast<float>(m_Size.y);

	m_OutOfDate = m_Size != lastSize;

}

void RenderStage::Rebuild(const Swapchain& swapchain)
{
	Update();

	const auto physicalDevice = GraphicManager::Get()->GetPhysicalDevice();
	const auto surface = GraphicManager::Get()->GetSurface();

	auto msaaSamples = physicalDevice->GetMsaaSamples();

	if(m_DepthAttachment)
	{
		m_DepthStencil = std::make_unique<ImageDepth>(m_Size.x, m_Size.y, m_DepthAttachment->IsMultisampled() ? msaaSamples : VK_SAMPLE_COUNT_1_BIT);
	}

	if(m_Renderpass == nullptr)
	{
		m_Renderpass = std::make_unique<RenderPass>(*this, m_DepthStencil->GetFormat(), surface->GetFormat().format, msaaSamples);
	}

	m_Framebuffers = std::make_unique<Framebuffers>(m_Size.x, m_Size.y, *this, *m_Renderpass, swapchain, *m_DepthStencil, msaaSamples);
	m_OutOfDate = false;

	m_Descriptors.clear();

	for(const auto &image: m_Attachments)
	{
		if(image.GetType() == Attachment::Type::DEPTH)
		{
			m_Descriptors.emplace(image.GetName(), m_DepthStencil.get());
		}else
		{
			m_Descriptors.emplace(image.GetName(), m_Framebuffers->GetAttachment(image.GetBinding()));
		}
	}
}

std::optional<Attachment> RenderStage::GetAttachment(const std::string& name) const
{
	auto it = std::find_if(m_Attachments.begin(), m_Attachments.end(), [name](const Attachment &a)
	{
		return a.GetName() == name;
	});

	if (it == m_Attachments.end())
	{
		return {};
	}

	return *it;
}

std::optional<Attachment> RenderStage::GetAttachment(const uint32_t& binding) const
{
	auto it = std::find_if(m_Attachments.begin(), m_Attachments.end(), [binding](const Attachment &a)
	{
		return a.GetBinding() == binding;
	});

	if (it == m_Attachments.end())
	{
		return {};
	}

	return *it;
}

const Descriptor* RenderStage::GetDescriptor(const std::string& name) const
{
	auto it = m_Descriptors.find(name);

	if(it != m_Descriptors.end())
	{
		return it->second;
	}

	return nullptr;
}

const VkFramebuffer& RenderStage::GetActiveFramebuffer(const uint32_t& activeSwapchainImage) const
{
	if(activeSwapchainImage > m_Framebuffers->GetFramebuffers().size())
	{
		return m_Framebuffers->GetFramebuffers().at(0);
	}

	return m_Framebuffers->GetFramebuffers().at(activeSwapchainImage);
}
}
