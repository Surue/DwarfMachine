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
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <set>
#include <algorithm>
#include <fstream>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include <utility/stb_image.h>

#include <graphics/graphic_manager.h>
#include <engine/Input.h>
#include <iostream>
#include <graphics/buffer.h>
#include <graphics/window.h>

namespace dm
{
GraphicManager::GraphicManager(Engine& engine) : m_Engine(engine)
{
	m_Window = std::make_unique<Window>(m_Engine);
}

void GraphicManager::InitWindow()
{
	m_Window->Init();
}

void GraphicManager::FrameBufferResizeCallback(SDL_Window* window, int width, int height)
{
	//TODO Faire en sorte que quand on resize une window cette fonction soit appel�e 
	const auto app = reinterpret_cast<GraphicManager*>(SDL_GetWindowData(window, "Vulkan App"));
	app->m_FrameBufferResized = true; 
}

void GraphicManager::InitVulkan()
{
	m_Instance = std::make_unique<Instance>(m_Window.get()->GetWindow());
	m_PhysicalDevice = std::make_unique<PhysicalDevice>(m_Instance.get());
	m_Surface = std::make_unique<Surface>(m_Instance.get(), m_PhysicalDevice.get(), m_Window.get());
	m_LogicalDevice = std::make_unique<LogicalDevice>(m_Instance.get(), m_PhysicalDevice.get(), m_Surface.get());

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateDescriptorSetLayout();
	CreateGraphicPipeline();

	m_CommandPool = std::make_shared<CommandPool>(m_LogicalDevice.get());

	CreateColorResources();
	CreateDepthResources();
	CreateFrameBuffers();
	CreateTextureImage();
	CreateTextureImageView();
	CreateTextureSampler();
	LoadModel();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
	CreateCommandBuffers();
	CreateSyncObjects();
}

void GraphicManager::Init()
{
	InitWindow();
	InitVulkan();
}

void GraphicManager::Destroy()
{
	vkDeviceWaitIdle(*m_LogicalDevice);

	//Destroy swap chain
	DestroySwapChain();

	//vkDestroySampler(*m_LogicalDevice, m_TextureSampler, nullptr);
	//vkDestroyImageView(*m_LogicalDevice, m_TextureImageView, nullptr);

	//vkDestroyImage(*m_LogicalDevice, m_TextureImage, nullptr);
	//vkFreeMemory(*m_LogicalDevice, m_TextureImageMemory, nullptr);

	vkDestroyDescriptorPool(*m_LogicalDevice, m_DescriptorPool, nullptr);

	//Destroy all bindings
	vkDestroyDescriptorSetLayout(*m_LogicalDevice, m_DescriptorSetLayout, nullptr);
	for(size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		vkDestroyBuffer(*m_LogicalDevice, m_UniformBuffers[i], nullptr);
		vkFreeMemory(*m_LogicalDevice, m_UniformBuffersMemory[i], nullptr);
	}

	//Destroy index buffer and its memory
	vkDestroyBuffer(*m_LogicalDevice, m_IndexBuffer, nullptr);
	vkFreeMemory(*m_LogicalDevice, m_IndexBufferMemory, nullptr);

	//Destroy vertex buffer and its memory
	vkDestroyBuffer(*m_LogicalDevice, m_VertexBuffer, nullptr);
	vkFreeMemory(*m_LogicalDevice, m_VertexBufferMemory, nullptr);

	//Destroy semaphores
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(*m_LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(*m_LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(*m_LogicalDevice, m_InFlightFences[i], nullptr);
	}
}

void GraphicManager::Update()
{
	DrawFrame();
}

void GraphicManager::SetMainCamera(Camera* camera)
{
	m_MainCamera = camera;
}

Window* GraphicManager::GetWindow() const
{
	return m_Window.get();
}

QueueFamilyIndices GraphicManager::FindQueueFamilies(const VkPhysicalDevice device) const
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

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface->GetSurface(), &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

bool GraphicManager::CheckDeviceExtensionSupport(const VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtension(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtension.data());

	std::set<std::string> requiredExtension(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

	for (const auto& extension : availableExtension)
	{
		requiredExtension.erase(extension.extensionName);
	}

	return requiredExtension.empty();
}

void GraphicManager::CreateSwapChain()
{
	const auto swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice->GetPhysicalDevice());

	const auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	const auto presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	const auto extent = ChooseSwapExtent(swapChainSupport.capabilities);

	auto imageCount = swapChainSupport.capabilities.minImageCount + 1;
	//Recommended to ask for +1 to don't wait on the driver

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface->GetSurface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; //Always 1 unless if developing a stereoscopic application 
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	//If want tu use post process use VK_IMAGE_USAGE_TRANSFER_DST_BIT

	auto indices = FindQueueFamilies(m_PhysicalDevice->GetPhysicalDevice());
	uint32_t queueFamilyIndices[] = {indices.graphicFamily.value(), indices.presentFamily.value()};

	if (indices.graphicFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //Best performance
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; //Don't read a pixel if it's hidden by an another window

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_LogicalDevice->GetLogicalDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_LogicalDevice->GetLogicalDevice(), m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_LogicalDevice->GetLogicalDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

SwapChainSupportDetails GraphicManager::QuerySwapChainSupport(const VkPhysicalDevice device) const
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface->GetSurface(), &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface->GetSurface(), &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface->GetSurface(), &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface->GetSurface(), &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface->GetSurface(), &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR GraphicManager::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace ==
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	/**
	 * Instead of choose only one device, it would be possible to rank all of them
	 * and to choose the best one
	 */

	return availableFormats[0];
}

VkPresentModeKHR GraphicManager::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	auto bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicManager::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		m_Window->GetFramebufferSize(&width, &height);

		VkExtent2D actualExtent = {width, height};

		actualExtent.width = std::max(capabilities.minImageExtent.width,
		                              std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height,
		                               std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void GraphicManager::CreateImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		m_SwapChainImageViews[i] = CreateImageView(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void GraphicManager::CreateGraphicPipeline()
{
	const auto vertexShaderCode = ReadFile("shaders/vert.spv");
	const auto fragShaderCode = ReadFile("shaders/frag.spv");

	const auto vertexShaderModule = CreateShaderModule(vertexShaderCode);
	const auto fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = vertexShaderModule;
	vertexShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragShaderStageInfo};

	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescription = Vertex::GetAttributeDescription();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChainExtent.width);
	viewport.height = static_cast<float>(m_SwapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = m_SwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multiSampling = {};
	multiSampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSampling.sampleShadingEnable = VK_TRUE;
	multiSampling.rasterizationSamples = m_PhysicalDevice->GetMsaaSamples();
	multiSampling.minSampleShading = 0.2f;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};
	depthStencil.back = {};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;

	if (vkCreatePipelineLayout(m_LogicalDevice->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multiSampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = m_PipelineLayout;
	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.pDepthStencilState = &depthStencil;

	if (vkCreateGraphicsPipelines(m_LogicalDevice->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) !=
		VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(m_LogicalDevice->GetLogicalDevice(), fragShaderModule, nullptr);
	vkDestroyShaderModule(m_LogicalDevice->GetLogicalDevice(), vertexShaderModule, nullptr);
}

std::vector<char> GraphicManager::ReadFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open shader file : " + filename);
	}

	const auto fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

VkShaderModule GraphicManager::CreateShaderModule(const std::vector<char>& code) const
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_LogicalDevice->GetLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void GraphicManager::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = m_SwapChainImageFormat;
	colorAttachment.samples = m_PhysicalDevice->GetMsaaSamples();
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve = {};
	colorAttachmentResolve.format = m_SwapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = FindDepthFormat();
	depthAttachment.samples = m_PhysicalDevice->GetMsaaSamples();
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentResolveRef = {};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	//Subpass dependencies
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(m_LogicalDevice->GetLogicalDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass");
	}
}

void GraphicManager::CreateFrameBuffers()
{
	m_SwapChainFrameBuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = 
		{
			m_ColorImageView,
			m_DepthImageView,
			m_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_LogicalDevice->GetLogicalDevice(), &framebufferInfo, nullptr, &m_SwapChainFrameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void GraphicManager::CreateCommandBuffers()
{
	m_CommandBuffers.resize(m_SwapChainFrameBuffers.size());

	for (size_t i = 0; i < m_CommandBuffers.size(); i++)
	{
		m_CommandBuffers[i] = std::make_unique<CommandBuffer>(false);
		m_CommandBuffers[i]->Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapChainFrameBuffers[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_SwapChainExtent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(*m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(*m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		VkBuffer vertexBuffers[] = {m_VertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(*m_CommandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(*m_CommandBuffers[i], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(*m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[i], 0, nullptr);
		vkCmdDrawIndexed(*m_CommandBuffers[i], static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(*m_CommandBuffers[i]);

		m_CommandBuffers[i]->End();
	}
}

void GraphicManager::CreateSyncObjects()
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_LogicalDevice->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_LogicalDevice->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_LogicalDevice->GetLogicalDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame");
		}
	}
}

void GraphicManager::DrawFrame()
{
	//Wait end of previous frame
	vkWaitForFences(*m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	//Acquire an image from the swap chain
	uint32_t imageIndex;
	auto result = vkAcquireNextImageKHR(*m_LogicalDevice, m_SwapChain, std::numeric_limits<uint64_t>::max(),
	                                        m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image");
	}

	UpdateUniformBuffer(imageIndex);

	VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};

	VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
	
	m_CommandBuffers[imageIndex]->Submit(waitSemaphores[0], signalSemaphores[0], m_InFlightFences[m_CurrentFrame]);

	//Presentation
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChain[] = {m_SwapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(m_LogicalDevice->GetPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FrameBufferResized)
	{
		m_FrameBufferResized = false;

		auto width = 0, height = 0;
		while (width == 0 || height == 0)
		{
			m_Window->GetFramebufferSize(&width, &height);
			m_Window->WaitEvents();
		}

		m_Engine.GetSettings().windowSize = Vec2i(width, height);

		RecreateSwapChain();
		UpdateMainCamera();

	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void GraphicManager::RecreateSwapChain()
{
	vkDeviceWaitIdle(m_LogicalDevice->GetLogicalDevice());

	DestroySwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicPipeline();
	CreateColorResources();
	CreateDepthResources();
	CreateFrameBuffers();
	CreateCommandBuffers();
}

void GraphicManager::DestroySwapChain()
{
	vkDestroyImageView(*m_LogicalDevice, m_ColorImageView, nullptr);
	vkDestroyImage(*m_LogicalDevice, m_ColorImage, nullptr);
	vkFreeMemory(*m_LogicalDevice, m_ColorImageMemory, nullptr);

	vkDestroyImageView(*m_LogicalDevice, m_DepthImageView, nullptr);
	vkDestroyImage(*m_LogicalDevice, m_DepthImage, nullptr);
	vkFreeMemory(*m_LogicalDevice, m_DepthImageMemory, nullptr);

	for (auto frameBuffer : m_SwapChainFrameBuffers)
	{
		vkDestroyFramebuffer(*m_LogicalDevice, frameBuffer, nullptr);
	}

	vkDestroyPipeline(*m_LogicalDevice, m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(*m_LogicalDevice, m_PipelineLayout, nullptr);
	vkDestroyRenderPass(*m_LogicalDevice, m_RenderPass, nullptr);

	for (auto imageView : m_SwapChainImageViews)
	{
		vkDestroyImageView(*m_LogicalDevice, imageView, nullptr);
	}

	vkDestroySwapchainKHR(*m_LogicalDevice, m_SwapChain, nullptr);
}

void GraphicManager::CreateBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage,
                                  const VkMemoryPropertyFlags properties,
                                  VkBuffer& buffer, VkDeviceMemory& bufferMemory) const
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_LogicalDevice->GetLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vertexx buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_LogicalDevice->GetLogicalDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	/**
	 * Normalement la taille maximum de la m�moire allou� est fix�e par maxMemoryAllocationCount
	 * ce qui peut �tre assez bas (4096). La bonne mani�re d'allou� de la m�moire pour un grand 
	 * nombre d'objet serait d'utiliser un custom allocator qui s�pare l'allocation en plus bout.
	 * 
	 * Il est aussi possible d'utiliser la librairie VulkanMemoryAllocator faite par GPUOpen
	 */
	//TODO modifier l'allocation de la m�moire pour les buffers
	if (vkAllocateMemory(m_LogicalDevice->GetLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory");
	}

	vkBindBufferMemory(m_LogicalDevice->GetLogicalDevice(), buffer, bufferMemory, 0);
}

void GraphicManager::CopyBuffer(const VkBuffer srcBuffer, const VkBuffer dstBuffer, const VkDeviceSize size) const
{
	const auto commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}

void GraphicManager::CreateVertexBuffer()
{
	const auto bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
	             stagingBufferMemory);

	void* data;
	vkMapMemory(*m_LogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(*m_LogicalDevice, stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

	CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

	vkDestroyBuffer(m_LogicalDevice->GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice->GetLogicalDevice(), stagingBufferMemory, nullptr);
}

void GraphicManager::CreateIndexBuffer()
{
	const auto bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
	             stagingBufferMemory);

	void* data;
	vkMapMemory(m_LogicalDevice->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_LogicalDevice->GetLogicalDevice(), stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

	CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

	vkDestroyBuffer(m_LogicalDevice->GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice->GetLogicalDevice(), stagingBufferMemory, nullptr);
}

uint32_t GraphicManager::FindMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice->GetPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void GraphicManager::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(m_LogicalDevice->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void GraphicManager::CreateUniformBuffers()
{
	const auto bufferSize = sizeof(UniformBufferObject);

	m_UniformBuffers.resize(m_SwapChainImages.size());
	m_UniformBuffersMemory.resize(m_SwapChainImages.size());

	for(size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i], m_UniformBuffersMemory[i]);
	}
}

void GraphicManager::UpdateUniformBuffer(const uint32_t currentImage)
{
	UniformBufferObject ubo = {};
	ubo.model = glm::identity<glm::mat4>();
	ubo.view = m_MainCamera->view;
	ubo.proj = m_MainCamera->proj;
	ubo.proj[1][1] *= -1; //Because glm was designed for openGl the y axis is inversed

	void* data;
	vkMapMemory(m_LogicalDevice->GetLogicalDevice(), m_UniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_LogicalDevice->GetLogicalDevice(), m_UniformBuffersMemory[currentImage]);
}

void GraphicManager::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size());

	if (vkCreateDescriptorPool(m_LogicalDevice->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void GraphicManager::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(m_SwapChainImages.size(), m_DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_SwapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	m_DescriptorSets.resize(m_SwapChainImages.size());
	if (vkAllocateDescriptorSets(m_LogicalDevice->GetLogicalDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = m_UniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImage->GetView();
		imageInfo.sampler = textureImage->GetSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_DescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_DescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(m_LogicalDevice->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void GraphicManager::CreateTextureImage()
{
	int texWidth, texHeight, texChannels;
	const auto pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	 VkDeviceSize imageSize = texWidth * texHeight * 4;
	/*
	if(!pixels)
	{
		throw std::runtime_error("failed to load texture image");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_LogicalDevice->GetLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(m_LogicalDevice->GetLogicalDevice(), stagingBufferMemory);

	stbi_image_free(pixels);

	CreateImage(texWidth, texHeight, m_MipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory);*/

	auto buffer = Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, nullptr);
	const VkExtent3D extent{ texWidth, texHeight, 1 };
	textureImage = std::make_unique<Image>(extent, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_NULL_HANDLE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_MipLevels, 1);
	Image::TransitionImageLayout(textureImage->GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, textureImage->GetMipLevels(), 0, 1, 0);
	
	textureImage->SetPixels(pixels, textureImage->GetArrayLevels(), 0);
	
	Image::CopyBufferToImage(buffer.GetBuffer(), textureImage->GetImage(), textureImage->GetExtent(), textureImage->GetArrayLevels(), 0);
	Image::CreateMipmaps(textureImage->GetImage(), textureImage->GetExtent(), textureImage->GetFormat(), textureImage->GetLayout(), textureImage->GetMipLevels(), 0, textureImage->GetArrayLevels());
	
	std::cout << "ici\n";
	/*TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);
	CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	
	GenerateMipmaps(m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, m_MipLevels);

	vkDestroyBuffer(m_LogicalDevice->GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice->GetLogicalDevice(), stagingBufferMemory, nullptr);*/
}

void GraphicManager::CreateImage(const uint32_t width, const uint32_t height, const uint32_t mipLevels, const VkSampleCountFlagBits numSamples,
                                 const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = numSamples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(m_LogicalDevice->GetLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_LogicalDevice->GetLogicalDevice(), image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_LogicalDevice->GetLogicalDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(m_LogicalDevice->GetLogicalDevice(), image, imageMemory, 0);
}

VkCommandBuffer GraphicManager::BeginSingleTimeCommands() const
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = *m_CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_LogicalDevice->GetLogicalDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void GraphicManager::EndSingleTimeCommands(VkCommandBuffer commandBuffer) const
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_LogicalDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_LogicalDevice->GetGraphicsQueue());

	vkFreeCommandBuffers(m_LogicalDevice->GetLogicalDevice(), *m_CommandPool, 1, &commandBuffer);
}

void GraphicManager::TransitionImageLayout(const VkImage image, const VkFormat format, const VkImageLayout oldLayout,
                                           const VkImageLayout newLayout, const uint32_t mipLevels) const
{
	const auto commandBuffer = BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	EndSingleTimeCommands(commandBuffer);
}

void GraphicManager::CopyBufferToImage(const VkBuffer buffer, const VkImage image, uint32_t width, uint32_t height) const
{
	const auto commandBuffer = BeginSingleTimeCommands();

	VkBufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	EndSingleTimeCommands(commandBuffer);
}

void GraphicManager::CreateTextureImageView()
{
	//m_TextureImageView = CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_MipLevels);
	Image::CreateImageView(textureImage->GetImage(), textureImage->GetView(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, textureImage->GetMipLevels(), 0, 1, 0);
}

VkImageView GraphicManager::CreateImageView(const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags, const uint32_t mipLevels) const
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(m_LogicalDevice->GetLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

void GraphicManager::CreateTextureSampler() {
	//VkSamplerCreateInfo samplerInfo = {};
	//samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	//samplerInfo.magFilter = VK_FILTER_LINEAR;
	//samplerInfo.minFilter = VK_FILTER_LINEAR;
	//samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	//samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	//samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	//samplerInfo.anisotropyEnable = VK_TRUE;
	//samplerInfo.maxAnisotropy = 16;
	//samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	//samplerInfo.unnormalizedCoordinates = VK_FALSE;
	//samplerInfo.compareEnable = VK_FALSE;
	//samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	//samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	//samplerInfo.minLod = 0;
	//samplerInfo.maxLod = static_cast<float>(m_MipLevels);
	//samplerInfo.mipLodBias = 0;

	//if (vkCreateSampler(m_LogicalDevice->GetLogicalDevice(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to create texture sampler!");
	//}

	Image::CreateImageSampler(textureImage->GetSampler(), VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, true, textureImage->GetMipLevels());
}

void GraphicManager::CreateDepthResources()
{
	const auto depthFormat = FindDepthFormat();

	CreateImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1, m_PhysicalDevice->GetMsaaSamples(), depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
	m_DepthImageView = CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	TransitionImageLayout(m_DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

VkFormat GraphicManager::FindSupportedFormat(const std::vector<VkFormat>& candidates, const VkImageTiling tiling,
                                             const VkFormatFeatureFlags features) const
{
	for (auto format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice->GetPhysicalDevice(), format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format");
}

VkFormat GraphicManager::FindDepthFormat() const
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool GraphicManager::HasStencilComponent(const VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void GraphicManager::LoadModel()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
				m_Vertices.push_back(vertex);
			}

			m_Indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void GraphicManager::GenerateMipmaps(const VkImage image, const VkFormat imageFormat, const int32_t texWidth, const int32_t texHeight, const uint32_t mipLevels) const
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice->GetPhysicalDevice(), imageFormat, &formatProperties);

	if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		throw std::runtime_error("texture image format does not support linear blitting");
	}

	const auto commandBuffer = BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	auto mipWidth = texWidth;
	auto mipHeight = texHeight;

	for(uint32_t i = 1; i < mipLevels; i++)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1};
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	EndSingleTimeCommands(commandBuffer);
}

void GraphicManager::CreateColorResources()
{
	const auto colorFormat = m_SwapChainImageFormat;

	CreateImage(m_SwapChainExtent.width, m_SwapChainExtent.height, 1, m_PhysicalDevice->GetMsaaSamples(), colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ColorImage, m_ColorImageMemory);
	m_ColorImageView = CreateImageView(m_ColorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	TransitionImageLayout(m_ColorImage, colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
}

void GraphicManager::UpdateMainCamera() const
{
	const auto windowSize = m_Engine.GetSettings().windowSize;
	m_MainCamera->proj = glm::perspective(glm::radians(45.0f), windowSize.x / static_cast<float>(windowSize.y), 0.1f, 100.0f);
}
}
