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

#include <graphics/logical_device.h>
#include <vector>
#include <graphics/physical_device.h>
#include <graphics/surface.h>
#include <iostream>
#include "graphics/instance.h"

namespace dm
{
LogicalDevice::LogicalDevice(const Instance* instance, const PhysicalDevice* physicalDevice, const Surface* surface) :
	m_Instance(instance),
	m_PhysicalDevice(physicalDevice),
	m_Surface(surface),
	m_LogicalDevice(VK_NULL_HANDLE),
	m_SupportedQueues(0),
	m_GraphicsFamily(0),
	m_PresentFamily(0),
	m_ComputeFamily(0),
	m_TransferFamily(0),
	m_GraphicsQueue(VK_NULL_HANDLE),
	m_PresentQueue(VK_NULL_HANDLE),
	m_ComputeQueue(VK_NULL_HANDLE),
	m_TransferQueue(VK_NULL_HANDLE)
{
	CreateQueueIndices();
	CreateLogicalDevice();
}

LogicalDevice::~LogicalDevice()
{
	vkDeviceWaitIdle(m_LogicalDevice);

	vkDestroyDevice(m_LogicalDevice, nullptr);
}

void LogicalDevice::CreateQueueIndices()
{
	int32_t graphicsFamily = -1;
	int32_t presentFamily = -1;
	int32_t computeFamily = -1;
	int32_t transferFamily = -1;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*m_PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	auto i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsFamily = i;
			m_SupportedQueues |= VK_QUEUE_GRAPHICS_BIT;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*m_PhysicalDevice, i, m_Surface->GetSurface(), &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			presentFamily = i;
		}

		if(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			computeFamily = i;
			m_SupportedQueues |= VK_QUEUE_COMPUTE_BIT;
		}

		if(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			transferFamily = i;
			m_SupportedQueues |= VK_QUEUE_TRANSFER_BIT;
		}

		if (graphicsFamily != -1 && presentFamily != -1 && computeFamily != -1 && transferFamily != -1)
		{
			break;
		}

		i++;
	}

	if(graphicsFamily == -1)
	{
		throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");	
	}

	m_GraphicsFamily = graphicsFamily;
	m_PresentFamily = presentFamily;
	m_ComputeFamily = computeFamily;
	m_TransferFamily = transferFamily;
}

void LogicalDevice::CreateLogicalDevice()
{
	auto physicalDeviceFeature = m_PhysicalDevice->GetFeatures();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriorities[] = { 0.0f };

	if(m_SupportedQueues & VK_QUEUE_GRAPHICS_BIT)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = m_GraphicsFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = queuePriorities;

		queueCreateInfos.push_back(queueCreateInfo);
	}else
	{
		m_GraphicsFamily = VK_NULL_HANDLE;
	}

	if (m_SupportedQueues & VK_QUEUE_COMPUTE_BIT && m_ComputeFamily != m_GraphicsFamily)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = m_ComputeFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = queuePriorities;

		queueCreateInfos.push_back(queueCreateInfo);
	}
	else
	{
		m_ComputeFamily = m_GraphicsFamily;
	}

	if (m_SupportedQueues & VK_QUEUE_TRANSFER_BIT && m_TransferFamily != m_GraphicsFamily && m_TransferFamily != m_ComputeFamily)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = m_TransferFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = queuePriorities;

		queueCreateInfos.push_back(queueCreateInfo);
	}
	else
	{
		m_ComputeFamily = m_GraphicsFamily;
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	if(physicalDeviceFeature.sampleRateShading)
	{
		deviceFeatures.sampleRateShading = VK_TRUE;
	}

	if(physicalDeviceFeature.fillModeNonSolid)
	{
		deviceFeatures.fillModeNonSolid = VK_TRUE;

		if(physicalDeviceFeature.wideLines)
		{
			deviceFeatures.wideLines = VK_TRUE;
		}
	}else
	{
		std::cout << "Selected GPU does not support wireframe pipelines\n";
	}

	if(physicalDeviceFeature.samplerAnisotropy)
	{
		deviceFeatures.samplerAnisotropy = VK_TRUE;
	}else
	{
		std::cout << "Selected GPU does not support sampler anisotropy\n";
	}

	if (physicalDeviceFeature.textureCompressionBC)
	{
		deviceFeatures.textureCompressionBC = VK_TRUE;
	}
	else if (physicalDeviceFeature.textureCompressionASTC_LDR)
	{
		deviceFeatures.textureCompressionASTC_LDR = VK_TRUE;
	}
	else if (physicalDeviceFeature.textureCompressionETC2)
	{
		deviceFeatures.textureCompressionETC2 = VK_TRUE;
	}

	if (physicalDeviceFeature.vertexPipelineStoresAndAtomics)
	{
		deviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support vertex pipeline stores and atomics\n";
	}

	if (physicalDeviceFeature.fragmentStoresAndAtomics)
	{
		deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support fragment stores and atomics\n";
	}

	if (physicalDeviceFeature.shaderStorageImageExtendedFormats)
	{
		deviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support shader storage extended formats\n";
	}

	if (physicalDeviceFeature.shaderStorageImageWriteWithoutFormat)
	{
		deviceFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support shader storage write without format\n";
	}

	if (physicalDeviceFeature.geometryShader)
	{
		deviceFeatures.geometryShader = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support geometry shaders\n";
	}

	if (physicalDeviceFeature.tessellationShader)
	{
		deviceFeatures.tessellationShader = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support tessellation shaders\n";
	}

	if (physicalDeviceFeature.multiViewport)
	{
		deviceFeatures.multiViewport = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support multi viewports\n";
	}

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_Instance->GetDeviceExtensions().size());
	createInfo.ppEnabledExtensionNames = m_Instance->GetDeviceExtensions().data();

	if (ENABLE_VALIDATION_LAYERS)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_Instance->GetInstanceLayers().size());
		createInfo.ppEnabledLayerNames = m_Instance->GetInstanceLayers().data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_PhysicalDevice->GetPhysicalDevice(), &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_LogicalDevice, m_GraphicsFamily, 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_LogicalDevice, m_PresentFamily, 0, &m_PresentQueue);
	vkGetDeviceQueue(m_LogicalDevice, m_ComputeFamily, 0, &m_ComputeQueue);
	vkGetDeviceQueue(m_LogicalDevice, m_TransferFamily, 0, &m_TransferQueue);
}
}
