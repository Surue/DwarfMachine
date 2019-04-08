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

#ifndef LOGICAL_DEVICE_H
#define LOGICAL_DEVICE_H

#include <vulkan/vulkan.h>
#include <optional>

namespace dm
{
class Instance;
class PhysicalDevice;
class Surface;

class LogicalDevice
{
public:
	LogicalDevice(const Instance* instance, const PhysicalDevice* physicalDevice, const Surface* surface);

	~LogicalDevice();

	operator const VkDevice &() const { return m_LogicalDevice; }

	const VkDevice &GetLogicalDevice() const { return m_LogicalDevice; }
	const VkPhysicalDeviceFeatures &GetEnabledFeatures() const { return m_EnabledFeatures; }
	const VkQueue &GetGraphicsQueue() const { return m_GraphicsQueue; }
	const VkQueue &GetPresentQueue() const { return m_PresentQueue; }
	const VkQueue &GetComputeQueue() const { return m_ComputeQueue; }
	const VkQueue &GetTransferQueue() const { return m_TransferQueue; }
	const uint32_t &GetGraphicsFamily() const { return m_GraphicsFamily; }
	const uint32_t &GetPresentFamily() const { return m_PresentFamily; }
	const uint32_t &GetComputeFamily() const { return m_ComputeFamily; }
	const uint32_t &GetTransferFamily() const { return m_TransferFamily; }
private:
	void CreateQueueIndices();
	void CreateLogicalDevice();

	const Instance* m_Instance;
	const PhysicalDevice* m_PhysicalDevice;
	const Surface* m_Surface;

	VkDevice m_LogicalDevice;
	VkPhysicalDeviceFeatures m_EnabledFeatures;

	VkQueueFlags m_SupportedQueues;
	uint32_t m_GraphicsFamily;
	uint32_t m_PresentFamily;
	uint32_t m_ComputeFamily;
	uint32_t m_TransferFamily;

	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;
	VkQueue m_ComputeQueue;
	VkQueue m_TransferQueue;
};
}

#endif