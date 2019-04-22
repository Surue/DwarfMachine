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

#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H

#include <graphics/image_2d.h>
#include <graphics/swapchain.h>

namespace dm
{
class ImageDepth;
class Renderpass;
class RenderStage;

class Framebuffers {
public:
	Framebuffers(const uint32_t &width, const uint32_t &height, const RenderStage &renderStage, const Renderpass &renderpass, const Swapchain &sawpchain, const ImageDepth &depthSstencil, const VkSampleCountFlagBits &samples = VK_SAMPLE_COUNT_1_BIT);

	~Framebuffers();

	const std::vector<std::unique_ptr<Image2d>> &GetImageAttachments() const { return m_ImageAttachments; }

	Image2d *GetAttachment(const uint32_t &index) const { return m_ImageAttachments[index].get(); }

	const std::vector<VkFramebuffer> &GetFramebuffers() const { return m_Framebuffers; }
private:
	std::vector<std::unique_ptr<Image2d>> m_ImageAttachments;
	std::vector<VkFramebuffer> m_Framebuffers;
};
}

#endif FRAMEBUFFERS_H