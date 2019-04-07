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

#include <GLFW/glfw3.h>

#include <graphics/instance.h>
#include <iostream>

namespace dm
{
const std::vector<const char *> Instance::ValidationLayers = { "VK_LAYER_LUNARG_standard_validation" };
const std::vector<const char *> Instance::InstanceExtensions = { };
const std::vector<const char *> Instance::DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; // VK_AMD_SHADER_IMAGE_LOAD_STORE_LOD_EXTENSION_NAME,
																								  // VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME

VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";

	return VK_FALSE;
}

Instance::Instance() : m_DebugMessenger(VK_NULL_HANDLE), m_Instance(VK_NULL_HANDLE)
{
	if (ENABLE_VALIDATION_LAYERS && !SetupLayers())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
	SetupExtensions();
	CreateInstance();
	CreateDebugCallback();
}

Instance::~Instance()
{
	DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
}

VkResult Instance::CreateDebugUtilsMessengerEXT(const VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		instance, "vkCreateDebugUtilsMessengerEXT"));
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Instance::DestroyDebugUtilsMessengerEXT(const VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

void Instance::CmdPushDescriptorSetKHR(VkDevice device, VkCommandBuffer commandBuffer,
	VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount,
	const VkWriteDescriptorSet* pDescriptorSet)
{
	auto func = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR"));

	if(func != nullptr)
	{
		func(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorSet);
	}
}

uint32_t Instance::FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties* deviceMemoryProperties,
	const VkMemoryRequirements* memoryRequirements, const VkMemoryPropertyFlags& requiredProperties)
{
	for (uint32_t i = 0; i < deviceMemoryProperties->memoryTypeCount; ++i)
	{
		if (memoryRequirements->memoryTypeBits & (1 << i))
		{
			if ((deviceMemoryProperties->memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties)
			{
				return i;
			}
		}
	}

	throw std::runtime_error("Couldn't find a proper memory type");
}

bool Instance::SetupLayers()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (auto layerName : ValidationLayers)
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

		m_InstanceLayer.emplace_back(layerName);
	}

	for (const auto &layerName : DeviceExtensions)
	{
		m_DeviceExtensions.emplace_back(layerName);
	}

	return true;
}

void Instance::SetupExtensions()
{
	uint32_t glfwExtensionsCount = 0;
	const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	m_InstanceExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionsCount);

	for (const auto &instanceExtension : InstanceExtensions)
	{
		m_InstanceExtensions.emplace_back(instanceExtension);
	}
}

void Instance::CreateInstance()
{
	//Create application info, use by the driver to do some optimization
	VkApplicationInfo appInfo = {}; //pNext = nullptr
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Dwarf machine application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Dwarf Machine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//Store data for creating an instance
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_InstanceExtensions.data();

	if (ENABLE_VALIDATION_LAYERS)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_InstanceLayer.size());
		createInfo.ppEnabledLayerNames = m_InstanceLayer.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}
}

void Instance::CreateDebugCallback()
{
	if constexpr (!ENABLE_VALIDATION_LAYERS) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}
}
