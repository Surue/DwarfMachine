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
#include <graphics/graphic_manager.h>

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
	GraphicManager::CheckVk(vkDeviceWaitIdle(m_LogicalDevice));

	vkDestroyDevice(m_LogicalDevice, nullptr);
}

void LogicalDevice::CreateQueueIndices()
{
	uint32_t deviceQueueFamilyPropertyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(*m_PhysicalDevice, &deviceQueueFamilyPropertyCount, nullptr);
	std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(deviceQueueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*m_PhysicalDevice, &deviceQueueFamilyPropertyCount, deviceQueueFamilyProperties.data());

	int32_t graphicsFamily = -1;
	int32_t presentFamily = -1;
	int32_t computeFamily = -1;
	int32_t transferFamily = -1;

	for (uint32_t i = 0; i < deviceQueueFamilyPropertyCount; i++)
	{
		// Check for graphics support.
		if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsFamily = i;
			m_GraphicsFamily = i;
			m_SupportedQueues |= VK_QUEUE_GRAPHICS_BIT;
		}

		// Check for presentation support.
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(*m_PhysicalDevice, i, *m_Surface, &presentSupport);

		if (deviceQueueFamilyProperties[i].queueCount > 0 && presentSupport)
		{
			presentFamily = i;
			m_PresentFamily = i;
		}

		// Check for compute support.
		if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			computeFamily = i;
			m_ComputeFamily = i;
			m_SupportedQueues |= VK_QUEUE_COMPUTE_BIT;
		}

		// Check for transfer support.
		if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			transferFamily = i;
			m_TransferFamily = i;
			m_SupportedQueues |= VK_QUEUE_TRANSFER_BIT;
		}

		if (graphicsFamily != -1 && presentFamily != -1 && computeFamily != -1 && transferFamily != -1)
		{
			break;
		}
	}

	if (graphicsFamily == -1)
	{
		throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");
	}
}

void LogicalDevice::CreateLogicalDevice()
{
	auto physicalDeviceFeatures = m_PhysicalDevice->GetFeatures();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriorities[] = { 0.0f };

	if (m_SupportedQueues & VK_QUEUE_GRAPHICS_BIT)
	{
		VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = m_GraphicsFamily;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(graphicsQueueCreateInfo);
	}
	else
	{
		m_GraphicsFamily = VK_NULL_HANDLE;
	}

	if (m_SupportedQueues & VK_QUEUE_COMPUTE_BIT && m_ComputeFamily != m_GraphicsFamily)
	{
		VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
		computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		computeQueueCreateInfo.queueFamilyIndex = m_ComputeFamily;
		computeQueueCreateInfo.queueCount = 1;
		computeQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(computeQueueCreateInfo);
	}
	else
	{
		m_ComputeFamily = m_GraphicsFamily;
	}

	if (m_SupportedQueues & VK_QUEUE_TRANSFER_BIT && m_TransferFamily != m_GraphicsFamily && m_TransferFamily != m_ComputeFamily)
	{
		VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
		transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transferQueueCreateInfo.queueFamilyIndex = m_TransferFamily;
		transferQueueCreateInfo.queueCount = 1;
		transferQueueCreateInfo.pQueuePriorities = queuePriorities;
		queueCreateInfos.emplace_back(transferQueueCreateInfo);
	}
	else
	{
		m_TransferFamily = m_GraphicsFamily;
	}

	VkPhysicalDeviceFeatures enabledFeatures = {};

	// Enable sample rate shading filtering if supported.
	if (physicalDeviceFeatures.sampleRateShading)
	{
		enabledFeatures.sampleRateShading = VK_TRUE;
	}

	// Fill mode non solid is required for wireframe display.
	if (physicalDeviceFeatures.fillModeNonSolid)
	{
		enabledFeatures.fillModeNonSolid = VK_TRUE;

		// Wide lines must be present for line width > 1.0f.
		if (physicalDeviceFeatures.wideLines)
		{
			enabledFeatures.wideLines = VK_TRUE;
		}
	}
	else
	{
		std::cout << "Selected GPU does not support wireframe pipelines!\n";
	}

	if (physicalDeviceFeatures.samplerAnisotropy)
	{
		enabledFeatures.samplerAnisotropy = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support sampler anisotropy!\n";
	}

	if (physicalDeviceFeatures.textureCompressionBC)
	{
		enabledFeatures.textureCompressionBC = VK_TRUE;
	}
	else if (physicalDeviceFeatures.textureCompressionASTC_LDR)
	{
		enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;
	}
	else if (physicalDeviceFeatures.textureCompressionETC2)
	{
		enabledFeatures.textureCompressionETC2 = VK_TRUE;
	}

	if (physicalDeviceFeatures.vertexPipelineStoresAndAtomics)
	{
		enabledFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
	}
	else
	{
		std::cout<<"Selected GPU does not support vertex pipeline stores and atomics!\n";
	}

	if (physicalDeviceFeatures.fragmentStoresAndAtomics)
	{
		enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support fragment stores and atomics!\n";
	}

	if (physicalDeviceFeatures.shaderStorageImageExtendedFormats)
	{
		enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support shader storage extended formats!\n";
	}

	if (physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat)
	{
		enabledFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support shader storage write without format!\n";
	}

	//enabledFeatures.shaderClipDistance = VK_TRUE;
	//enabledFeatures.shaderCullDistance = VK_TRUE;

	if (physicalDeviceFeatures.geometryShader)
	{
		enabledFeatures.geometryShader = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support geometry shaders!\n";
	}

	if (physicalDeviceFeatures.tessellationShader)
	{
		enabledFeatures.tessellationShader = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support tessellation shaders!\n";
	}

	if (physicalDeviceFeatures.multiViewport)
	{
		enabledFeatures.multiViewport = VK_TRUE;
	}
	else
	{
		std::cout << "Selected GPU does not support multi viewports!\n";
	}

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_Instance->GetInstanceLayers().size());
	deviceCreateInfo.ppEnabledLayerNames = m_Instance->GetInstanceLayers().data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_Instance->GetDeviceExtensions().size());
	deviceCreateInfo.ppEnabledExtensionNames = m_Instance->GetDeviceExtensions().data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
	GraphicManager::CheckVk(vkCreateDevice(*m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice));

	vkGetDeviceQueue(m_LogicalDevice, m_GraphicsFamily, 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_LogicalDevice, m_PresentFamily, 0, &m_PresentQueue);
	vkGetDeviceQueue(m_LogicalDevice, m_ComputeFamily, 0, &m_ComputeQueue);
	vkGetDeviceQueue(m_LogicalDevice, m_TransferFamily, 0, &m_TransferQueue);
}
}
