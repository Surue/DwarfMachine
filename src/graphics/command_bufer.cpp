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

#include <graphics/command_buffer.h>
#include <graphics/logical_device.h>

namespace dm
{
CommandBuffer::CommandBuffer(const std::shared_ptr<CommandPool> commandPool, LogicalDevice* logicalDevice, const VkQueueFlagBits& queueType,
		const VkCommandBufferLevel& bufferLevel):
	m_CommandPool(commandPool),
	m_QueueType(queueType), 
	m_CommandBuffer(nullptr), 
	m_Running(false),
	m_LogicalDevice(logicalDevice)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = *m_CommandPool;
	allocInfo.level = bufferLevel;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(*m_LogicalDevice, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

CommandBuffer::~CommandBuffer()
{
	vkFreeCommandBuffers(*m_LogicalDevice, m_CommandPool->GetCommandPool(), 1, &m_CommandBuffer);
}

void CommandBuffer::Begin(const VkCommandBufferUsageFlags& usage)
{
	if(m_Running)
	{
		return;
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = usage;

	if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	m_Running = true;
}

void CommandBuffer::End()
{
	if(!m_Running)
	{
		return;
	}

	if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}

	m_Running = false;
}

void CommandBuffer::SubmitIdle()
{
	auto queueSelected = GetQueue();

	if(m_Running)
	{
		End();
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffer;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkFence fence;
	vkCreateFence(*m_LogicalDevice, &fenceCreateInfo, nullptr, &fence);
	vkResetFences(*m_LogicalDevice, 1, &fence);

	if (vkQueueSubmit(m_LogicalDevice->GetGraphicsQueue(), 1, &submitInfo, fence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer");
	}

	vkWaitForFences(*m_LogicalDevice, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());

	vkDestroyFence(*m_LogicalDevice, fence, nullptr);
}

void CommandBuffer::Submit(const VkSemaphore& waitSemaphore, const VkSemaphore& signalSemaphore, VkFence fence)
{
	const auto queueSelected = GetQueue();

	if(m_Running)
	{
		End();
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffer;

	if (waitSemaphore != VK_NULL_HANDLE)
	{
		// Pipeline stages used to wait at for graphics queue submissions.
		static VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		submitInfo.pWaitDstStageMask = &submitPipelineStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphore;
	}

	if (signalSemaphore != VK_NULL_HANDLE)
	{
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;
	}

	if (fence != VK_NULL_HANDLE)
	{
		vkResetFences(*m_LogicalDevice, 1, &fence);
	}

	vkQueueSubmit(queueSelected, 1, &submitInfo, fence);
}

VkQueue CommandBuffer::GetQueue() const
{
	switch(m_QueueType)
	{
	case VK_QUEUE_GRAPHICS_BIT:
		return m_LogicalDevice->GetGraphicsQueue();
	case VK_QUEUE_COMPUTE_BIT:
		return m_LogicalDevice->GetComputeQueue();
	default: 
		return nullptr;
	}
}
}
