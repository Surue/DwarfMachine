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

#define STB_IMAGE_IMPLEMENTATION
#include <utility/stb_image.h>

#include <vector>
#define GLM_FORCE_RADIANS
#define FLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <iostream>
#include <array>

#include <engine/engine.h>


namespace DM
{
const int MAX_FRAMES_IN_FLIGHT = 2;

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescription() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};


const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

struct UniformBufferObject
{
	alignas(16)glm::mat4 model;
	alignas(16)glm::mat4 view;
	alignas(16)glm::mat4 proj;
};

class GraphicManager
{
public:

	GraphicManager();
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

	static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

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
	static bool CheckValidationLayerSupport();

	/**
	 * \brief return required list of extension
	 */
	static std::vector<const char*> GetRequiredExtensions();

	/**
	 * \brief Create a surface used to link vulkan to the GLFW window
	 */
	void CreateSurface();

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
	bool IsDeviceSuitable(VkPhysicalDevice device) const;

	/**
	 * \brief Return queue families that satisfy certain desired properties
	 * \param device
	 */
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;

	/**
	 * \brief Setup logical device to interface with
	 */
	void CreateLogicalDevice();

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
	void CreateCommandPool();

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

	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	VkCommandBuffer BeginSingleTimeCommands() const;

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;

	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

	void CreateTextureImageView();

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;

	void CreateTextureSampler();

	void CreateDepthResources();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	VkFormat FindDepthFormat() const;

	bool HasStencilComponent(VkFormat format) const;

	//WINDOW
	GLFWwindow* m_Window = nullptr;

	const int WIDTH = 800;
	const int HEIGHT = 600;

	//VULKAN
	VkInstance m_VulkanInstance = nullptr;
	VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
	VkSurfaceKHR m_Surface{};

	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice m_Device = nullptr;

	VkQueue m_GraphicsQueue = nullptr;
	VkQueue m_PresentQueue{};

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

	VkCommandPool m_CommandPool{};
	std::vector<VkCommandBuffer> m_CommandBuffers;

	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	size_t m_CurrentFrame = 0;

	bool m_FrameBufferResized = false;

	//vertex buffer
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
	VkImage m_TextureImage{};
	VkDeviceMemory m_TextureImageMemory{};
	VkImageView m_TextureImageView;

	VkSampler m_TextureSampler;

	//Depth
	VkImage m_DepthImage;
	VkDeviceMemory m_DepthImageMemory;
	VkImageView m_DepthImageView;
};
}

#endif GRAPHIC_MANAGER_H