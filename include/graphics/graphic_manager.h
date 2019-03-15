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

#ifndef GRAPHIC_MANAGER_H
#define GRAPHIC_MANAGER_H

#include <vector>

#include <engine/engine.h>

namespace DM
{
class GraphicManager
{
public:

	GraphicManager();
	~GraphicManager();
	/**
	 * \brief Init graphic manager (GLFW and Vulkan)
	 */
	void Init();

	/**
	 * \brief Use to cleanup vulkan's allocation
	 */
	void Destroy();

	/**
	 * \brief Get the game's window
	 * \return 
	 */
	GLFWwindow* GetWindow() const;

private:
	/**
	 * \brief Init a GLFW window
	 */
	void InitWindow();

	/**
	 * \brief Init Vulkan
	 */
	void InitVulkan();

	/**
	 * \brief Create a vulkan's instance
	 */
	void CreateInstanceVulkan();

	/**
	 * \brief Setup the messenger for handling error callback in vulkan
	 */
	void SetupDebugMessenger();

	/**
	 * \brief Check if all requested layers are available
	 */
	bool CheckValidationLayerSupport() const;

	/**
	 * \brief return required list of extension
	 */
	std::vector<const char*> GetRequiredExtensions() const;

	/**
	 * \brief Callback function for vulkan error message
	 * \param messageSeverity Specify the severity of the message
	 * \param messageType Type of the message
	 * \param pCallbackData Contains the details of the message
	 * \param pUserData Allow to pass own data
	 * \return 
	 */
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	/**
	 * \brief Select a GPU
	 */
	void PickPhysicalDevice();

	/**
	 * \brief Evaluate if a device is suitable for our operation
	 * \param device 
	 */
	bool IsDeviceSuitable(VkPhysicalDevice device);

	/**
	 * \brief Return queue families that satisfy certain desired properties
	 * \param device
	 */
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	/**
	 * \brief Setup logical device to interface with
	 */
	void CreateLogicalDevice();

	//WINDOW
	GLFWwindow* m_Window = nullptr;

	const int WIDTH = 800;
	const int HEIGHT = 600;

	//VULKAN
	VkInstance m_VulkanInstance = nullptr;

	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

	const std::vector<const char*> m_ValidationLayer = { "VK_LAYER_LUNARG_standard_validation" };

	VkDevice m_Device = nullptr;

	VkQueue m_GraphicsQueue = nullptr;

	//vulkan error handling
#ifdef NDEBUG
	const bool m_EnableValidationLayers = false;
#else
	const bool m_EnableValidationLayers = true;
#endif

	VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
};
}

#endif GRAPHIC_MANAGER_H