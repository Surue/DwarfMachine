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

#include <engine/engine.h>
#include <component/camera.h>
#include <graphics/instance.h>
#include <graphics/surface.h>
#include <graphics/physical_device.h>
#include <graphics/logical_device.h>
#include <graphics/command_buffer.h>

#include <SDL.h>
#include <graphics/swapchain.h>
#include <graphics/render_stage.h>
#include <graphics/render_manager.h>

namespace dm
{
/**
 * \brief Create a vulkan's validation layer. The validation layer is used to catch vulkan's specific error
 * \param instance It's the current vulkan instance
 * \param pCreateInfo Information used to create the validation layer
 * \param pAllocator custom allocator 
 * \param pDebugMessenger ptr that will store new messenger
 * \return if succeeded return a new messenger, otherwise return VK_ERROR_EXTENSION_NOT_PRESENT
 */
inline VkResult CreateDebugUtilsMessengerEXT(const VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
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

/**
 * \brief Destroy a messenger acting as the validation layer
 * \param instance The current Vulkan Instance
 * \param debugMessenger ptr to delete
 * \param pAllocator custom allocator
 */
inline void DestroyDebugUtilsMessengerEXT(const VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

class GraphicManager
{
public:

	GraphicManager(Engine& engine);
	~GraphicManager();
	/**
	 * \brief Init graphic manager (GLFW and Vulkan)
	 */
	void Init();

	/**
	 * \brief Use to cleanup vulkan's allocation
	 */
	void Destroy();

	void Update();

	void SetMainCamera(Camera* camera);

	/**
	 * \brief Get the game's window
	 * \return 
	 */
	Window* GetWindow() const;

	void SetManager(RenderManager* managerRender);

	void SetRenderStages(std::vector<std::unique_ptr<RenderStage>> renderStages);

	const PhysicalDevice* GetPhysicalDevice() const { return m_PhysicalDevice.get(); }

	const LogicalDevice* GetLogicalDevice() const { return m_LogicalDevice.get(); }

	const std::shared_ptr<CommandPool> &GetCommandPool(const std::thread::id &threadId = std::this_thread::get_id());

	const Surface* GetSurface() const { return m_Surface.get(); }

	const Swapchain* GetSwapchain() const { return m_Swapchain.get(); }

	Camera* GetCamera() const { return m_MainCamera; }

	RenderStage *GetRenderStage(const uint32_t &index) const;

	const VkPipelineCache &GetPipelineCache() const { return m_PipelineCache; }

	RendererContainer* GetRendererContainer() const;

	const Descriptor *GetAttachment(const std::string &name) const;

	static std::string StringifyResultVk(const VkResult &result);

	static void CheckVk(const VkResult &result);

private:
	void CreatePipelineCache();
	/**
	 * \brief Init a GLFW window
	 */
	void InitWindow();

	static void FrameBufferResizeCallback(SDL_Window* window, int width, int height);

	void UpdateMainCamera() const;

	void RecreatePass(RenderStage &renderStage); 

	void RecreateAttachmentsMap(); 

	bool StartRenderpass(RenderStage &renderStage); 

	void EndRenderpass(RenderStage &renderStage); 


	//WINDOW
	std::unique_ptr<Window> m_Window;

	//VULKAN
	std::unique_ptr<Instance> m_Instance; 
	std::unique_ptr<Surface> m_Surface;
	std::unique_ptr<PhysicalDevice> m_PhysicalDevice; 
	std::unique_ptr<LogicalDevice> m_LogicalDevice; 

	std::unique_ptr<Swapchain> m_Swapchain;

	std::map<std::thread::id, std::shared_ptr<CommandPool>> m_CommandPools; 
	std::vector< std::unique_ptr<CommandBuffer>> m_CommandBuffers;

	VkPipelineCache m_PipelineCache; 
	std::vector<VkSemaphore> m_PresentCompletesSemaphore; 
	std::vector<VkSemaphore> m_RenderCompletesSemaphore; 
	std::vector<VkFence> m_InFlightFences; 
	size_t m_CurrentFrame; 

	Engine& m_Engine;

	//Camera
	Camera* m_MainCamera = nullptr;

	std::vector<std::unique_ptr<RenderStage>> m_RenderStages; 
	std::map<std::string, const Descriptor *> m_Attachments; 
	std::unique_ptr<RenderManager> m_RenderManager; 
};
}

#endif GRAPHIC_MANAGER_H