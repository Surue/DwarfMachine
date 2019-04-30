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

#define TINYOBJLOADER_IMPLEMENTATION
#include <utility/tiny_obj_loader.h>

#include <engine/engine.h>
#include <graphics/vulkan_utilities.h>
#include <component/camera.h>
#include <graphics/instance.h>
#include <graphics/surface.h>
#include <graphics/physical_device.h>
#include <graphics/logical_device.h>
#include <graphics/command_buffer.h>

#include <SDL.h>
#include <graphics/image_2d.h>
#include <graphics/image_depth.h>
#include <graphics/swapchain.h>
#include "render_stage.h"
#include "uniform_handle.h"

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
	~GraphicManager() = default;
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

	const PhysicalDevice* GetPhysicalDevice() const { return m_PhysicalDevice.get(); }

	const LogicalDevice* GetLogicalDevice() const { return m_LogicalDevice.get(); }

	std::shared_ptr<CommandPool> GetCommandPool() const { return m_CommandPool; }

	const Surface* GetSurface() const { return m_Surface.get(); }

	const Swapchain* GetSwapchain() const { return m_Swapchain.get(); }

	Camera* GetCamera() const { return m_MainCamera; }

	RenderStage *GetRenderStage(const uint32_t &index) const;

	const VkPipelineCache &GetPipelineCache() const { return m_PipelineCache; }

private:
	void CreatePipelineCache();
	/**
	 * \brief Init a GLFW window
	 */
	void InitWindow();

	static void FrameBufferResizeCallback(SDL_Window* window, int width, int height);

	/**
	 * \brief Init Vulkan
	 */
	void InitVulkan();

	/**
	 * \brief Create a swapchain (Act as an image buffer)
	 */
	void CreateSwapChain();

	/**
	 * \brief Create a swap chain struct
	 * \param device 
	 * \return 
	 */
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;

	/**
	 * \brief 
	 * \param availablePresentModes 
	 * \return 
	 */
	static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	/**
	 * \brief 
	 * \param capabilities 
	 * \return 
	 */
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

	/**
	 * \brief Create graphic pipeline
	 */
	void CreateGraphicPipeline();

	/**
	 * \brief Read a SPIR_V shader file
	 * \param filename SPIR-V shader
	 * \return 
	 */
	static std::vector<char> ReadFile(const std::string& filename);

	/**
	 * \brief Create a shader module
	 * \param code  buffer with bytecode
	 * \return 
	 */
	VkShaderModule CreateShaderModule(const std::vector<char>& code) const;

	/**
	 * \brief 
	 */
	void CreateRenderPass();

	/**
	 * \brief 
	 */
	void CreateFrameBuffers();

	/**
	 * \brief
	 */
	void CreateCommandBuffers();

	/**
	 * \brief Create semaphore and fences
	 */
	void CreateSyncObjects();

	/**
	 * \brief Draw the current frame
	 */
	void DrawFrame();

	/**
	 * \brief Handle resizing of swap chain if it's not compatible anymore with the window
	 */
	void RecreateSwapChain();

	void DestroySwapChain();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

	/**
	 * \brief Create a vertex buffer that store all vertices send to the GPU
	 */
	void CreateVertexBuffer();

	/**
	 * \brief Create an index buffer to point to different vertex
	 */
	void CreateIndexBuffer();

	void CreateDescriptorSetLayout();

	void CreateUniformBuffers();

	void UpdateUniformBuffer(uint32_t currentImage);

	void CreateDescriptorPool();

	void CreateDescriptorSets();

	void CreateTextureImage();

	VkCommandBuffer BeginSingleTimeCommands() const;

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	void CreateDepthResources();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	VkFormat FindDepthFormat() const;

	void LoadModel();

	void CreateColorResources();

	void UpdateMainCamera() const;

	//WINDOW
	std::unique_ptr<Window> m_Window = nullptr;

	const std::string MODEL_PATH = "ressources/models/Tentacle_lp.obj";
	const std::string TEXTURE_PATH = "ressources/textures/Tentacle_lp_defaultMat_BaseColor.png";

	//VULKAN
	std::unique_ptr<Instance> m_Instance = nullptr;
	std::unique_ptr<Surface> m_Surface = nullptr;
	std::unique_ptr<PhysicalDevice> m_PhysicalDevice = nullptr;
	std::unique_ptr<LogicalDevice> m_LogicalDevice = nullptr;

	std::unique_ptr<Swapchain> m_Swapchain = nullptr;

	std::vector<VkFence> m_InFlightFences;
	size_t m_CurrentFrame = 0;

	VkRenderPass m_RenderPass{};
	VkDescriptorSetLayout m_DescriptorSetLayout{};
	VkPipelineLayout m_PipelineLayout{};

	VkPipeline m_GraphicsPipeline{};

	std::vector<VkFramebuffer> m_SwapChainFrameBuffers;

	std::shared_ptr<CommandPool> m_CommandPool = nullptr;
	std::vector< std::unique_ptr<CommandBuffer>> m_CommandBuffers;

	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;

	bool m_FrameBufferResized = false;

	//vertex buffer
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};

	//Index buffer
	VkBuffer m_IndexBuffer{};
	VkDeviceMemory m_IndexBufferMemory{};

	//Uniform buffers
	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBuffersMemory;

	VkDescriptorPool m_DescriptorPool{};
	std::vector<VkDescriptorSet> m_DescriptorSets;

	//Images
	std::shared_ptr<Image2d> m_TextureImage = nullptr; //Textures tentacules
	std::shared_ptr<ImageDepth> m_DepthImage = nullptr; //Depth
	std::shared_ptr<Image2d> m_ColorImage = nullptr; //Multisample Anti Aliasing 

	Engine& m_Engine;

	//Camera
	Camera* m_MainCamera = nullptr;

	VkPipelineCache m_PipelineCache;
	std::vector<std::unique_ptr<RenderStage>> m_RenderStages;
};
}

#endif GRAPHIC_MANAGER_H