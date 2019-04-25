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

#include <graphics/descriptor_handle.h>
#include <graphics/graphic_manager.h>

namespace dm
{
DescriptorHandle::DescriptorHandle() :
	m_Shader(nullptr),
	m_PushDescriptor(false),
	m_DescriptorSet(nullptr),
	m_Changed(false)
{}

DescriptorHandle::DescriptorHandle(const Pipeline& pipeline) :
	m_Shader(pipeline.GetShader()),
	m_PushDescriptor(pipeline.IsPushDescriptor()),
	m_DescriptorSet(std::make_unique<DescriptorSet>(pipeline)),
	m_Changed(true)
{}

void DescriptorHandle::Push(const std::string& descriptorName, UniformHandle& uniformHandle,
	const std::optional<OffsetSize>& offsetSize)
{
	if(m_Shader == nullptr)
	{
		return;
	}

	uniformHandle.Update(m_Shader->GetUniformBlock(descriptorName));
	Push(descriptorName, uniformHandle.GetUniformBuffer(), offsetSize);
}

void DescriptorHandle::Push(const std::string& descriptorName, StorageHandle& storageHandle,
	const std::optional<OffsetSize>& offsetSize)
{
	if (m_Shader == nullptr)
	{
		return;
	}

	storageHandle.Update(m_Shader->GetUniformBlock(descriptorName));
	Push(descriptorName, storageHandle.GetUniformBuffer(), offsetSize);
}

void DescriptorHandle::Push(const std::string& descriptorName, PushHandle& pushHandle,
	const std::optional<OffsetSize>& offsetSize)
{
	if (m_Shader == nullptr)
	{
		return;
	}

	pushHandle.Update(m_Shader->GetUniformBlock(descriptorName));
}

bool DescriptorHandle::Update(const Pipeline& pipeline)
{
	if(m_Shader != pipeline.GetShader())
	{
		m_Shader = pipeline.GetShader();
		m_PushDescriptor = pipeline.IsPushDescriptor();
		m_Descriptor.clear();
		m_WriteDescriptorSets.clear();

		if(!m_PushDescriptor)
		{
			m_DescriptorSet = std::make_unique<DescriptorSet>(pipeline);
		}

		m_Changed = false;
		return false;
	}

	if(m_Changed)
	{
		m_WriteDescriptorSets.clear();
		m_WriteDescriptorSets.resize(m_Descriptor.size());

		for(const auto &[descriptorName, descriptor] : m_Descriptor)
		{
			auto writeDescriptorSet = descriptor.writeDescriptor.GetWriteDescriptorSet();
			writeDescriptorSet.dstSet = VK_NULL_HANDLE;

			if(!m_PushDescriptor)
			{
				writeDescriptorSet.dstSet = m_DescriptorSet->GetDescriptorSet();
			}

			m_WriteDescriptorSets.emplace_back(writeDescriptorSet);
		}

		if(!m_PushDescriptor)
		{
			m_DescriptorSet->Update(m_WriteDescriptorSets);
		}

		m_Changed = false;
	}

	return true;
}

void DescriptorHandle::BindDescriptor(const CommandBuffer& commandBuffer, const Pipeline& pipeline)
{
	if(m_PushDescriptor)
	{
		auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

		Instance::CmdPushDescriptorSetKHR(*logicalDevice, commandBuffer, pipeline.GetPipelineBindPoint(), pipeline.GetPipelineLayout(), 0,
			static_cast<uint32_t>(m_WriteDescriptorSets.size()), m_WriteDescriptorSets.data());
	}
	else
	{
		m_DescriptorSet->BindDescriptor(commandBuffer);
	}
}
}
