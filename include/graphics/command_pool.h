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

#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include <vulkan/vulkan.h>
#include <thread>

namespace dm
{
class LogicalDevice;

class CommandPool
{
public:
	CommandPool(LogicalDevice* logicalDevice, const std::thread::id &threadId = std::this_thread::get_id());

	~CommandPool();

	operator const VkCommandPool &() const { return m_CommandPool; }
	const VkCommandPool &GetCommandPool() const { return m_CommandPool; }
	const std::thread::id &GetThreadId() const { return m_ThreadId; }
private:
	VkCommandPool m_CommandPool;
	std::thread::id m_ThreadId;

	LogicalDevice* m_LogicalDevice;
};
}

#endif COMMAND_POOL_H