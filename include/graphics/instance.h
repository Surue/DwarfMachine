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

#ifndef INSTANCE_H
#define INSTANCE_H

#include <vector>
#include <vulkan/vulkan.h>
#include <SDL.h>

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

namespace dm
{
/**
 * \brief It represents the connection from the engine to the Vulkan runtime. Only one should exists per application
 */
class Instance final
{
public:
	friend VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	static const std::vector<const char*> ValidationLayers;
	static const std::vector<const char*> InstanceExtensions;
	static const std::vector<const char*> DeviceExtensions;

	Instance(SDL_Window* window);
	~Instance();

	static VkResult CreateDebugUtilsMessengerEXT(
		VkInstance									instance,
		const VkDebugUtilsMessengerCreateInfoEXT*	pCreateInfo,
		const VkAllocationCallbacks*				pAllocator,
		VkDebugUtilsMessengerEXT*					pDebugMessenger);

	static void DestroyDebugUtilsMessengerEXT(
		VkInstance									instance,
		VkDebugUtilsMessengerEXT					debugMessenger,
		const VkAllocationCallbacks*				pAllocator);

	static void CmdPushDescriptorSetKHR(VkDevice device, VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorSet);

	static uint32_t FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties *deviceMemoryProperties, const VkMemoryRequirements *memoryRequirements, const VkMemoryPropertyFlags &requiredProperties);

	operator const VkInstance &() const { return m_Instance; }

	const std::vector<const char*> &GetInstanceLayers() const { return m_InstanceLayer; }
	const std::vector<const char*> &GetInstanceExtensions() const { return m_InstanceExtensions; }
	const std::vector<const char*> &GetDeviceExtensions() const { return m_DeviceExtensions; }
	const VkInstance &GetInstance() const { return m_Instance; }
private:
	void SetupLayers();
	void SetupExtensions(SDL_Window* window);
	void CreateInstance();
	void CreateDebugCallback();

	std::vector<const char*> m_InstanceLayer;
	std::vector<const char*> m_InstanceExtensions;
	std::vector<const char*> m_DeviceExtensions;

	VkDebugUtilsMessengerEXT m_DebugMessenger;
	VkInstance m_Instance;
};
}

#endif