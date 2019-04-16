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

private:
	/**
	 * \brief Init a GLFW window
	 */
	void InitWindow();

	static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

	/**
	 * \brief Init Vulkan
	 */
	void InitVulkan();

	/**
	 * \brief Return queue families that satisfy certain desired properties
	 * \param device
	 */
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;

	/**
	 * \brief Check if the given device support extensions
	 * \param device to check
	 * \return true if support extensions
	 */
	static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

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
	 * \param availableFormats 
	 * \return 
	 */
	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

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
	 * \brief 
	 */
	void CreateImageViews();

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

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	void CreateDescriptorSetLayout();

	void CreateUniformBuffers();

	void UpdateUniformBuffer(uint32_t currentImage);

	void CreateDescriptorPool();

	void CreateDescriptorSets();

	void CreateTextureImage();

	void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;

	VkCommandBuffer BeginSingleTimeCommands() const;

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) const;

	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

	void CreateTextureImageView();

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) const;

	void CreateTextureSampler();

	void CreateDepthResources();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	VkFormat FindDepthFormat() const;

	static bool HasStencilComponent(VkFormat format);

	void LoadModel();

	void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) const;

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

	VkSwapchainKHR m_SwapChain{};

	std::vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent{};

	std::vector<VkImageView> m_SwapChainImageViews;

	VkRenderPass m_RenderPass{};
	VkDescriptorSetLayout m_DescriptorSetLayout{};
	VkPipelineLayout m_PipelineLayout{};

	VkPipeline m_GraphicsPipeline{};

	std::vector<VkFramebuffer> m_SwapChainFrameBuffers;

	std::shared_ptr<CommandPool> m_CommandPool = nullptr;
	//std::vector<VkCommandBuffer> m_OldCommandBuffers;
	std::vector< std::unique_ptr<CommandBuffer>> m_CommandBuffers;

	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	size_t m_CurrentFrame = 0;

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

	//Textures
	uint32_t m_MipLevels{};
	VkImage m_TextureImage{};
	VkDeviceMemory m_TextureImageMemory{};
	VkImageView m_TextureImageView{};

	VkSampler m_TextureSampler{};

	//Depth
	VkImage m_DepthImage{};
	VkDeviceMemory m_DepthImageMemory{};
	VkImageView m_DepthImageView{};

	//Multisample Anti Aliasing 
	VkImage m_ColorImage{};
	VkDeviceMemory m_ColorImageMemory{};
	VkImageView m_ColorImageView{};

	Engine& m_Engine;

	//Camera
	Camera* m_MainCamera = nullptr;
};
}

#endif GRAPHIC_MANAGER_H