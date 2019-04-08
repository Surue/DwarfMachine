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

#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>
#include <vector>

namespace dm
{

class Instance;

class PhysicalDevice
{
public:
	PhysicalDevice(const Instance *instance);

	operator const VkPhysicalDevice &() const { return m_PhysicalDevice; }

	const VkPhysicalDevice &GetPhysicalDevice() const { return m_PhysicalDevice; }
	const VkPhysicalDeviceProperties &GetProperties() const { return m_Properties; }
	const VkPhysicalDeviceFeatures &GetFeatures() const { return m_Features; }
	const VkPhysicalDeviceMemoryProperties &GetMemoryProperties() const { return m_MemoryProperties; }
	const VkSampleCountFlagBits &GetMsaaSamples() const { return m_MsaaSamples; }
private:
	VkPhysicalDevice ChoosePhysicalDevice(const std::vector<VkPhysicalDevice> &devices);

	int32_t ScorePhysicalDevice(const VkPhysicalDevice &device);

	VkSampleCountFlagBits GetMaxUsableSampleCount() const;

	const Instance *m_Instance;

	VkPhysicalDevice m_PhysicalDevice;
	VkPhysicalDeviceProperties m_Properties;
	VkPhysicalDeviceFeatures m_Features;
	VkPhysicalDeviceMemoryProperties m_MemoryProperties;
	VkSampleCountFlagBits m_MsaaSamples;
};
}

#endif PHYSICAL_DEVICE_H