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

#ifndef POST_FILTER_H
#define POST_FILTER_H

#include <graphics/render_pipeline.h>
#include <graphics/descriptor_handle.h>
#include <graphics/pipeline_graphic.h>

namespace dm
{
class PostFilter : public RenderPipeline
{
public:
	PostFilter(const Pipeline::Stage &pipelineStage, const std::vector<std::string> &shaderStage, const std::vector<Shader::Define> &defines = {});

	virtual ~PostFilter();

	const DescriptorHandle &GetDescriptorSet() const { return m_DescriptorSet; }

	const PipelineGraphics &GetPipeline() const { return m_Pipeline; }

	const Descriptor *GetAttachment(const std::string &descriptorName, const Descriptor *descriptor) const;

	const Descriptor *GetAttachment(const std::string &descriptorName, const std::string &rendererAttachment) const;

	void SetAttachment(const std::string &descriptorName, const Descriptor* descriptor);

	bool RemoveAttachment(const std::string &descriptorName);

protected:
	static uint32_t GlobalSwitching;

	void PushConditional(const std::string &descriptorName1, const std::string &descriptorName2, const std::string &rendererAttachment1, const std::string &rendererAttachment2);

	DescriptorHandle m_DescriptorSet;
	PipelineGraphics m_Pipeline;

	std::map<std::string, const Descriptor*> m_Attachments;
};
}

#endif