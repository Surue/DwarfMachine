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

#include <graphics/descriptor_set.h>

#include <graphics/graphic_manager.h>

namespace dm
{
DescriptorSet::DescriptorSet(const Pipeline& pipeline) :
	m_PipelineLayout(pipeline.GetPipelineLayout()),
	m_PipelineBindPoint(pipeline.GetPipelineBindPoint()),
	m_DescriptorPool(pipeline.GetDescriptorPool()),
	m_DescriptorSet(VK_NULL_HANDLE)
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	VkDescriptorSetLayout layouts[1] = { pipeline.GetDescriptorSetLayout() };

	VkDescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = m_DescriptorPool;
	allocateInfo.descriptorSetCount = 1;
	allocateInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(*logicalDevice, &allocateInfo, &m_DescriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
}

DescriptorSet::~DescriptorSet()
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	vkFreeDescriptorSets(*logicalDevice, m_DescriptorPool, 1, &m_DescriptorSet);
}

void DescriptorSet::Update(const std::vector<VkWriteDescriptorSet>& descriptorWrites)
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	vkUpdateDescriptorSets(*logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void DescriptorSet::BindDescriptor(const CommandBuffer& commandBuffer)
{
	vkCmdBindDescriptorSets(commandBuffer, m_PipelineBindPoint, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
}
}