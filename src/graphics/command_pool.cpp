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

#include <graphics/command_pool.h>
#include <graphics/logical_device.h>
#include <graphics/graphic_manager.h>

namespace dm
{
CommandPool::CommandPool(const std::thread::id& threadId) :
	m_CommandPool(VK_NULL_HANDLE),
	m_ThreadId(threadId)
{
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();
	const auto graphicsFamily = logicalDevice->GetGraphicsFamily();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = graphicsFamily;

	GraphicManager::CheckVk(vkCreateCommandPool(*logicalDevice, &poolInfo, nullptr, &m_CommandPool));
}

CommandPool::~CommandPool()
{
	std::cout << "~CommandPool\n";
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();
	vkDestroyCommandPool(*logicalDevice, m_CommandPool, nullptr);
}
}
