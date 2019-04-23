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

#ifndef RENDER_STAGE_H
#define RENDER_STAGE_H

#include <vulkan/vulkan.h>
#include <engine/vector.h>
#include <graphics/image_depth.h>
#include <graphics/framebuffers.h>
#include <graphics/renderpass.h>
#include <graphics/swapchain.h>
#include <map>

namespace dm
{
class Attachment
{
public:
	enum class Type
	{
		IMAGE,
		DEPTH,
		SWAPCHAIN
	};

	Attachment(const uint32_t &binding, std::string name, const Type &type, const bool &multisampled = false, const VkFormat &format = VK_FORMAT_R8G8B8A8_UNORM) :
		m_Binding(binding),
		m_Name(std::move(name)),
		m_Type(type),
		m_Multisampled(multisampled),
		m_Format(format)
	{}

	const uint32_t &GetBinding() const { return m_Binding; }

	const std::string &GetName() const { return m_Name; }

	const Type &GetType() const { return m_Type; }

	const bool &IsMultisampled() const { return m_Multisampled; }

	const VkFormat &GetFormat() const { return m_Format; }
private:
	uint32_t m_Binding;
	std::string m_Name;
	Type m_Type;
	bool m_Multisampled;
	VkFormat m_Format;
};

class SubpassType
{
public:
	SubpassType(const uint32_t &binding, std::vector<uint32_t> attachmentBindings):
		m_Binding(binding),
		m_AttachmentBindings(std::move(attachmentBindings))
	{}

	const uint32_t &GetBinding() const { return m_Binding; }

	const std::vector<uint32_t> &GetAttachmentBindings() const { return m_AttachmentBindings; }
private:
	uint32_t m_Binding;
	std::vector<uint32_t> m_AttachmentBindings;
};

class Viewport
{
public:
	Viewport() :
		m_Scale(1.0f, 1.0f),
		m_Offset(0, 0)
	{}

	explicit Viewport(const std::optional<Vec2i> &size) :
		m_Scale(1.0f, 1.0f),
		m_Size(size),
		m_Offset(0, 0)
	{}

	const Vec2f &GetScale() const { return m_Scale; }

	void SetScale(const Vec2f &scale) { m_Scale = scale; }

	const std::optional<Vec2i> &GetSize() const { return m_Size; }

	void SetSize(const std::optional<Vec2i> &size) { m_Size = size; }

	const Vec2i &GetOffset() const { return m_Offset; }

	void SetOffset(const Vec2i &offset) { m_Offset = offset; }
private:
	Vec2f m_Scale;
	std::optional<Vec2i> m_Size;
	Vec2i m_Offset;
};

class RenderStage
{
public:
	explicit RenderStage(std::vector<Attachment> images = {}, std::vector<SubpassType> subpasses = {}, const Viewport &viewport = Viewport());

	void Update();

	void Rebuild(const Swapchain &swapchain);

	std::optional<Attachment> GetAttachment(const std::string &name) const;

	std::optional<Attachment> GetAttachment(const uint32_t &binding) const;

	const std::vector<Attachment> &GetAttachments() const { return m_Attachments; }

	const std::vector<SubpassType> &GetSubpasses() const { return m_Subpasses; }

	Viewport &GetViewport() { return m_Viewport; }

	void SetViewPort(const Viewport &viewport) { m_Viewport = viewport; }

	const Vec2i &GetSize() const { return m_Size; }

	const float &getAspectRatio() const { return m_AspectRatio; }

	const bool &ISOUtOfDate() const { return m_OutOfDate; }

	const RenderPass *GetRenderPass() const { return m_Renderpass.get(); }

	const ImageDepth *GetDepthStencil() const { return m_DepthStencil.get(); }

	const Framebuffers *GetFramebuffers() const { return m_Framebuffers.get(); }

	const Descriptor *GetDescriptor(const std::string &name) const;

	const VkFramebuffer &GetActiveFramebuffer(const uint32_t &activeSwapchainImage) const;

	const std::vector<VkClearValue> &GetClearValues() const { return m_ClearValues; }

	const uint32_t &GetAttachmentCount(const uint32_t &subpass) const { return m_SubpassAttachmentCount[subpass]; }

	bool HasDepth() const { return m_DepthAttachment.has_value(); }

	bool HasSwapchain() const { return m_SwapchainAttachment.has_value(); }

	bool IsMultisampled(const uint32_t &subpass) const { return m_SubpassMultisampled[subpass]; }

private:
	std::vector<Attachment> m_Attachments;
	std::vector<SubpassType> m_Subpasses;

	Viewport m_Viewport;

	std::unique_ptr<RenderPass> m_Renderpass;
	std::unique_ptr<ImageDepth> m_DepthStencil;
	std::unique_ptr<Framebuffers> m_Framebuffers;

	std::map<std::string, const Descriptor*> m_Descriptors;

	std::vector<VkClearValue> m_ClearValues;
	std::vector<uint32_t> m_SubpassAttachmentCount;
	std::optional<Attachment> m_DepthAttachment;
	std::optional<Attachment> m_SwapchainAttachment;
	std::vector<bool> m_SubpassMultisampled;

	Vec2i m_Size;
	float m_AspectRatio;
	bool m_OutOfDate;
};
}

#endif RENDER_STAGE_H