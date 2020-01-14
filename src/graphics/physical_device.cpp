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

#include <graphics/physical_device.h>
#include <graphics/instance.h>
#include <map>
#include <algorithm>
#include <stdexcept>

namespace  dm
{
static const std::vector<VkSampleCountFlagBits> STAGE_FLAG_BITS = { VK_SAMPLE_COUNT_64_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_16_BIT, VK_SAMPLE_COUNT_8_BIT,
VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_2_BIT };

PhysicalDevice::PhysicalDevice(const Instance* instance) :
	m_Instance(instance),
	m_PhysicalDevice(VK_NULL_HANDLE),
	m_Properties({}),
	m_Features({}),
	m_MemoryProperties({}),
	m_MsaaSamples(VK_SAMPLE_COUNT_1_BIT)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(*m_Instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(*m_Instance, &deviceCount, devices.data());

	m_PhysicalDevice = ChoosePhysicalDevice(devices);

	if (m_PhysicalDevice == nullptr)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);
	m_MsaaSamples = GetMaxUsableSampleCount();
}

VkPhysicalDevice PhysicalDevice::ChoosePhysicalDevice(const std::vector<VkPhysicalDevice>& devices)
{
	std::multimap<int32_t, VkPhysicalDevice> rankedDevices;

	for(const auto &device : devices)
	{
		int32_t score = ScorePhysicalDevice(device);
		rankedDevices.emplace(score, device);
	}

	if(rankedDevices.rbegin()->first > 0)
	{
		return rankedDevices.rbegin()->second;
	}

	return nullptr;
}

int32_t PhysicalDevice::ScorePhysicalDevice(const VkPhysicalDevice& device)
{
	int32_t score = 0;

	uint32_t extensionPropertyCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, nullptr);
	std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, extensionProperties.data());

	for(const char *currentExtension : Instance::DeviceExtensions)
	{
		bool extensionFound = false;

		for(const auto &extension : extensionProperties)
		{
			if(strcmp(currentExtension, extension.extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}

		if(!extensionFound)
		{
			return 0;
		}
	}

	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
	vkGetPhysicalDeviceFeatures(device, &physicalDeviceFeatures);

	if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}

	score += physicalDeviceProperties.limits.maxImageDimension2D;

	return score;
}

VkSampleCountFlagBits PhysicalDevice::GetMaxUsableSampleCount() const
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

	const auto counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts,
	                             physicalDeviceProperties.limits.framebufferDepthSampleCounts);

	for(const auto &sampleFlag : STAGE_FLAG_BITS)
	{
		if(counts & sampleFlag)
		{
			return sampleFlag;
		}
	}
	return VK_SAMPLE_COUNT_1_BIT;
}
}
