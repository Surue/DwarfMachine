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
#include <stdexcept>
#include <iostream>

#include <graphics/graphic_manager.h>

namespace DM
{
GraphicManager::GraphicManager() {}

GraphicManager::~GraphicManager() {}

void GraphicManager::InitWindow()
{
	//Init glfw
	glfwInit();

	//Set context to null (otherwise it would be openGL)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Lock resize function
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void GraphicManager::InitVulkan()
{
	CreateInstanceVulkan();
	SetupDebugMessenger();
	PickPhysicalDevice();
	CreateLogicalDevice();
}

void GraphicManager::Init()
{
	InitWindow();
	InitVulkan();
}

void GraphicManager::Destroy()
{
	vkDestroyDevice(m_Device, nullptr);

	if (m_EnableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
	}

	vkDestroyInstance(m_VulkanInstance, nullptr);

	glfwDestroyWindow(m_Window);

	glfwTerminate();
}

GLFWwindow* GraphicManager::GetWindow() const
{
	return m_Window;
}

void GraphicManager::CreateInstanceVulkan()
{
	if (m_EnableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	//Create application info, use by the driver to do some optimization
	VkApplicationInfo appInfo = {}; //pNext = nullptr
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Dwarf Machine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//Store data for creating an instance
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//Set the layer for the api to be use by the GLFW window
	auto extensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	createInfo.enabledLayerCount = 0;
	if (vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}
}

void GraphicManager::SetupDebugMessenger()
{
	if (!m_EnableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	if (CreateDebugUtilsMessengerEXT(m_VulkanInstance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

bool GraphicManager::CheckValidationLayerSupport() const
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_ValidationLayer)
	{
		auto layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> GraphicManager::GetRequiredExtensions() const
{
	uint32_t glfwExtensionsCount = 0;
	const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

	//If debug mod, add an extension to handle error
	if (m_EnableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VkBool32 GraphicManager::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                       void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";

	return VK_FALSE;
}

void GraphicManager::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			m_PhysicalDevice = device;
			break;
		}
	}

	if (m_PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool GraphicManager::IsDeviceSuitable(const VkPhysicalDevice device)
{
	auto indices = FindQueueFamilies(device);

	return indices.IsComplete();
}

QueueFamilyIndices GraphicManager::FindQueueFamilies(const VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	auto i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

void GraphicManager::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicFamily.value();
	queueCreateInfo.queueCount = 1;

	auto queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;
	if (m_EnableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayer.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayer.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_Device, indices.graphicFamily.value(), 0, &m_GraphicsQueue);
}
}
