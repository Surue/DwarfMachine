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

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <vulkan/vulkan.h>
#include <memory>

#include <graphics/command_pool.h>

namespace dm
{
class LogicalDevice;

class CommandBuffer {
public:
	CommandBuffer(std::shared_ptr<CommandPool> commandPool ,LogicalDevice* logicalDevice, const VkQueueFlagBits &queueType = VK_QUEUE_GRAPHICS_BIT, const VkCommandBufferLevel &bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	~CommandBuffer();

	void Begin(const VkCommandBufferUsageFlags &usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	void End();

	void SubmitIdle();

	void Submit(const VkSemaphore &waitSemaphore = VK_NULL_HANDLE, const VkSemaphore &signalSemaphore = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);

	const bool &IsRunning() const { return m_Running; }

	operator const VkCommandBuffer &() const { return m_CommandBuffer; }

	const VkCommandBuffer &GetCommandBuffer() const { return m_CommandBuffer; }
private:
	VkQueue GetQueue() const;
	std::shared_ptr<CommandPool> m_CommandPool;

	VkQueueFlagBits m_QueueType;
	VkCommandBuffer m_CommandBuffer;
	bool m_Running;

	LogicalDevice* m_LogicalDevice;
};
}

#endif