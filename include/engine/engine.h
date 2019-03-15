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

#ifndef ENGINE_H
#define ENGINE_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <optional>

namespace DM
{
class GraphicManager;

inline VkResult CreateDebugUtilsMessengerEXT(const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(
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

inline void DestroyDebugUtilsMessengerEXT(const VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	const auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)(vkGetInstanceProcAddr(
		instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicFamily;

	bool IsComplete() const
	{
		return graphicFamily.has_value();
	}
};

class Engine
{
public:
	void Run();
private:
	/**
	 * \brief Main loop of the game
	 */
	void MainLoop();

	/**
	 * \brief Use to cleanup vulkan's allocation
	 */
	void Destroy();

	GLFWwindow* m_Window = nullptr;

	GraphicManager* m_GraphicManager;
};
}

#endif ENGINE_H