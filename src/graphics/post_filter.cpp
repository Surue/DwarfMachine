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

#include <graphics/post_filter.h>
#include <graphics/graphic_manager.h>

namespace dm
{
uint32_t PostFilter::GlobalSwitching = 0;

PostFilter::PostFilter(const Pipeline::Stage& pipelineStage, const std::vector<std::string>& shaderStage,
	const std::vector<Shader::Define>& defines) :
	RenderPipeline(pipelineStage),
	m_Pipeline(pipelineStage, shaderStage, {}, defines, PipelineGraphics::Mode::POLYGON, PipelineGraphics::Depth::NONE)
{
}

const Descriptor* PostFilter::GetAttachment(const std::string& descriptorName, const Descriptor* descriptor) const
{
	const auto it = m_Attachments.find(descriptorName);

	if(it == m_Attachments.end())
	{
		return descriptor;
	}

	return it->second;
}

const Descriptor* PostFilter::GetAttachment(const std::string& descriptorName,
	const std::string& rendererAttachment) const
{
	const auto it = m_Attachments.find(descriptorName);

	if (it == m_Attachments.end())
	{

		return Engine::Get()->GetGraphicManager()->GetAttachment(descriptorName);
	}

	return it->second;
}

void PostFilter::SetAttachment(const std::string& descriptorName, const Descriptor* descriptor)
{
	const auto it = m_Attachments.find(descriptorName);

	if (it == m_Attachments.end())
	{
		m_Attachments.emplace(descriptorName, descriptor);
		return;
	}

	it->second = descriptor;
}

bool PostFilter::RemoveAttachment(const std::string& descriptorName)
{
	const auto it = m_Attachments.find(descriptorName);

	if (it != m_Attachments.end())
	{
		m_Attachments.erase(it);
		return true;
	}

	return false;
}

void PostFilter::PushConditional(const std::string& descriptorName1, const std::string& descriptorName2,
	const std::string& rendererAttachment1, const std::string& rendererAttachment2)
{
	const auto it1 = m_Attachments.find(descriptorName1);
	const auto it2 = m_Attachments.find(descriptorName2);

	if(it1 != m_Attachments.end() || it2 != m_Attachments.end())
	{
		m_DescriptorSet.Push(descriptorName1, GetAttachment(descriptorName1, rendererAttachment1));
		m_DescriptorSet.Push(descriptorName2, GetAttachment(descriptorName2, rendererAttachment1));
		return;
	}

	if(it1 == m_Attachments.end() && it2 != m_Attachments.end())
	{
		m_DescriptorSet.Push(descriptorName1, Engine::Get()->GetGraphicManager()->GetAttachment(GlobalSwitching % 2 == 1 ? rendererAttachment1 : rendererAttachment2));
		m_DescriptorSet.Push(descriptorName2, GetAttachment(descriptorName2, rendererAttachment1));
		return;
	}

	if (it1 != m_Attachments.end() && it2 == m_Attachments.end())
	{
		m_DescriptorSet.Push(descriptorName1, GetAttachment(descriptorName1, rendererAttachment1));
		m_DescriptorSet.Push(descriptorName2, Engine::Get()->GetGraphicManager()->GetAttachment(GlobalSwitching % 2 == 1 ? rendererAttachment1 : rendererAttachment2));
		return;
	}

	if(GlobalSwitching % 2 == 1)
	{
		m_DescriptorSet.Push(descriptorName1, Engine::Get()->GetGraphicManager()->GetAttachment(rendererAttachment1));
		m_DescriptorSet.Push(descriptorName2, Engine::Get()->GetGraphicManager()->GetAttachment(rendererAttachment2));
	}else
	{
		m_DescriptorSet.Push(descriptorName1, Engine::Get()->GetGraphicManager()->GetAttachment(rendererAttachment2));
		m_DescriptorSet.Push(descriptorName2, Engine::Get()->GetGraphicManager()->GetAttachment(rendererAttachment1));
	}

	GlobalSwitching++;
}
}
