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

#include <graphics/pipeline_graphic.h>

#include <graphics/graphic_manager.h>
#include "engine/file.h"
#include <iostream>

namespace dm
{
const std::vector<VkDynamicState> DYNAMIC_STATES = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH };

PipelineGraphics::PipelineGraphics(Stage stage, std::vector<std::string> shaderStage,
	std::vector<Shader::VertexInput> vertexInputs, std::vector<Shader::Define> defines, const Mode& mode,
	const Depth& depthMode, const VkPrimitiveTopology& topology, const VkPolygonMode& polygonMode,
	const VkCullModeFlags& cullMode, const VkFrontFace& frontFace, const bool& pushDescriptor) :
	m_Stage(std::move(stage)),
	m_ShaderStages(std::move(shaderStage)),
	m_VertexInputs(std::move(vertexInputs)),
	m_Defines(std::move(defines)),
	m_Mode(mode),
	m_Depth(depthMode),
	m_Topology(topology),
	m_PolygonMode(polygonMode),
	m_CullMode(cullMode),
	m_FrontFace(frontFace),
	m_PushDescriptors(pushDescriptor),
	m_Shader(std::make_unique<Shader>(m_ShaderStages.back())),
	m_DynamicStates(std::vector<VkDynamicState>(DYNAMIC_STATES)),
	m_DescriptorSetLayout(VK_NULL_HANDLE),
	m_DescriptorPool(VK_NULL_HANDLE),
	m_Pipeline(VK_NULL_HANDLE),
	m_PipelineLayout(VK_NULL_HANDLE),
	m_PipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS),
	m_VertexInputStateCreateInfo({}),
	m_InputAssemblyStateCreateInfo({}),
	m_RasterizationStateCreateInfo({}),
	m_BlendAttachmentStates({}),
	m_ColorBlendStateCreateInfo({}),
	m_DepthStencilStateCreateInfo({}),
	m_ViewportStateCreateInfo({}),
	m_MultisampleStateCreateInfo({}),
	m_DynamicStateCreateInfo({}),
	m_TessellationStateCreateInfo({})
{
	std::sort(m_VertexInputs.begin(), m_VertexInputs.end());
	CreateShaderProgram();
	CreateDescriptorLayout();
	CreateDescriptorPool();
	CreatePipelineLayout();
	CreateAttributes();

	switch (m_Mode)
	{
	case Mode::POLYGON:
		CreatePipelinePolygon();
		break;
	case Mode::MRT:
		CreatePipelineMrt();
		break;
	default:
		throw std::runtime_error("Unknown pipeline mode");
		break;
	}
}

PipelineGraphics::~PipelineGraphics()
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	for(const auto &shaderModule : m_Modules)
	{
		vkDestroyShaderModule(*logicalDevice, shaderModule, nullptr);
	}

	vkDestroyDescriptorPool(*logicalDevice, m_DescriptorPool, nullptr);
	vkDestroyPipeline(*logicalDevice, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(*logicalDevice, m_PipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(*logicalDevice, m_DescriptorSetLayout, nullptr);
}

const ImageDepth* PipelineGraphics::GetDepthStencil(const std::optional<uint32_t>& stage) const
{
	return Engine::Get()->GetGraphicManager()->GetRenderStage(stage ? *stage : m_Stage.first)->GetDepthStencil();
}

const Image2d* PipelineGraphics::GetTexture(const uint32_t& index, const std::optional<uint32_t>& stage) const
{
	return Engine::Get()->GetGraphicManager()->GetRenderStage(stage ? *stage : m_Stage.first)->GetFramebuffers()->GetAttachment(index);
}

Vec2i PipelineGraphics::GetSize(const std::optional<uint32_t>& stage) const
{
	return Engine::Get()->GetGraphicManager()->GetRenderStage(stage ? *stage : m_Stage.first)->GetSize();
}

float PipelineGraphics::GetAspectRatio(const std::optional<uint32_t>& stage) const
{
	return Engine::Get()->GetGraphicManager()->GetRenderStage(stage ? *stage : m_Stage.first)->GetAspectRatio();
}

void PipelineGraphics::CreateShaderProgram()
{
	std::stringstream defineBlock;
	defineBlock << "\n";

	for (const auto &define : m_Defines)
	{
		defineBlock << "#define " << define.first << " " << define.second << "\n";
	}

	for (const auto &shaderStage : m_ShaderStages)
	{
		auto fileLoaded = Files::Read(shaderStage);

		if (!fileLoaded)
		{
			std::cout << "Shader Stage could not be loaded: " << shaderStage.c_str() << "\n";
			throw std::runtime_error("Could not create pipeline, missing shader stage");
			return;
		}

		auto shaderCode = Shader::InsertDefineBlock(*fileLoaded, defineBlock.str());
		shaderCode = Shader::ProcessIncludes(shaderCode);

		auto stageFlag = Shader::GetShaderStage(shaderStage);
		auto shaderModule = m_Shader->ProcessShader(shaderCode, stageFlag);

		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {};
		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.stage = stageFlag;
		pipelineShaderStageCreateInfo.module = shaderModule;
		pipelineShaderStageCreateInfo.pName = "main";
		m_Stages.emplace_back(pipelineShaderStageCreateInfo);
		m_Modules.emplace_back(shaderModule);
	}

	m_Shader->ProcessShader();

	std::cout << m_Shader->ToString() << "\n";
}

void PipelineGraphics::CreateDescriptorLayout()
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	auto &descriptorSetLayouts = m_Shader->GetDescriptorSetLayouts();

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.flags = m_PushDescriptors ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();
	GraphicManager::CheckVk(vkCreateDescriptorSetLayout(*logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout));
}

void PipelineGraphics::CreateDescriptorPool()
{
	const auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	auto descriptorPools = m_Shader->GetDescriptorPools();

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = 8192; // 16384;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
	GraphicManager::CheckVk(vkCreateDescriptorPool(*logicalDevice, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));
}

void PipelineGraphics::CreatePipelineLayout()
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	auto pushConstantRanges = m_Shader->GetPushConstantRanges();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
	GraphicManager::CheckVk(vkCreatePipelineLayout(*logicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));
}

void PipelineGraphics::CreateAttributes()
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	auto physicalDevice = Engine::Get()->GetGraphicManager()->GetPhysicalDevice();

	if(m_PolygonMode == VK_POLYGON_MODE_LINE && !logicalDevice->GetEnabledFeatures().fillModeNonSolid)
	{
		throw std::runtime_error("Cannot create graphics pipeline with line polygon mode when logical device does not support non solid fills.");
	}

	m_InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_InputAssemblyStateCreateInfo.topology = m_Topology;
	m_InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	m_RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_RasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	m_RasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	m_RasterizationStateCreateInfo.polygonMode = m_PolygonMode;
	m_RasterizationStateCreateInfo.cullMode = m_CullMode;
	m_RasterizationStateCreateInfo.frontFace = m_FrontFace;
	m_RasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	m_RasterizationStateCreateInfo.lineWidth = 1.0f;

	m_BlendAttachmentStates[0].blendEnable = VK_TRUE;
	m_BlendAttachmentStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	m_BlendAttachmentStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	m_BlendAttachmentStates[0].colorBlendOp = VK_BLEND_OP_ADD;
	m_BlendAttachmentStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	m_BlendAttachmentStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
	m_BlendAttachmentStates[0].alphaBlendOp = VK_BLEND_OP_MAX;
	m_BlendAttachmentStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	m_ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	m_ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	m_ColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(m_BlendAttachmentStates.size());
	m_ColorBlendStateCreateInfo.pAttachments = m_BlendAttachmentStates.data();
	m_ColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	m_ColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	m_ColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	m_ColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	m_DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_DepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	m_DepthStencilStateCreateInfo.front = m_DepthStencilStateCreateInfo.back;
	m_DepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;

	switch(m_Depth)
	{
	case Depth::NONE: 
		m_DepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
		m_DepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
		break;
	case Depth::READ: 
		m_DepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		m_DepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
		break;
	case Depth::WRITE: 
		m_DepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
		m_DepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		break;
	case Depth::READ_WRITE: 
		m_DepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		m_DepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		break;
	default: ;
	}

	m_ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_ViewportStateCreateInfo.viewportCount = 1;
	m_ViewportStateCreateInfo.scissorCount = 1;

	auto renderStage = Engine::Get()->GetGraphicManager()->GetRenderStage(m_Stage.first);
	bool multisampled = renderStage->IsMultisampled(m_Stage.second);

	m_MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_MultisampleStateCreateInfo.rasterizationSamples = multisampled ? physicalDevice->GetMsaaSamples() : VK_SAMPLE_COUNT_1_BIT;
	m_MultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

	m_DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
	m_DynamicStateCreateInfo.pDynamicStates = m_DynamicStates.data();

	m_TessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	m_TessellationStateCreateInfo.patchControlPoints = 3;
}

void PipelineGraphics::CreatePipeline()
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();
	auto pipelineCache = Engine::Get()->GetGraphicManager()->GetPipelineCache();
	auto renderStage = Engine::Get()->GetGraphicManager()->GetRenderStage(m_Stage.first);

	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	uint32_t lastAttribute = 0;

	for(const auto &vertexInput : m_VertexInputs)
	{
		for(const auto &binding : vertexInput.GetBindingDescriptions())
		{
			bindingDescriptions.emplace_back(binding);
		}

		for(const auto &attribute : vertexInput.GetAttributeDescriptions())
		{
			auto &newAttribute = attributeDescriptions.emplace_back(attribute);
			newAttribute.location += lastAttribute;
		}

		if(!vertexInput.GetAttributeDescriptions().empty())
		{
			lastAttribute = attributeDescriptions.back().location + 1;
		}
	}

	m_VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_VertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	m_VertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	m_VertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	m_VertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(m_Stages.size());
	pipelineCreateInfo.pStages = m_Stages.data();

	pipelineCreateInfo.pVertexInputState = &m_VertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &m_InputAssemblyStateCreateInfo;
	pipelineCreateInfo.pTessellationState = &m_TessellationStateCreateInfo;
	pipelineCreateInfo.pViewportState = &m_ViewportStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &m_RasterizationStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = &m_MultisampleStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &m_DepthStencilStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = &m_ColorBlendStateCreateInfo;
	pipelineCreateInfo.pDynamicState = &m_DynamicStateCreateInfo;

	pipelineCreateInfo.layout = m_PipelineLayout;
	pipelineCreateInfo.renderPass = renderStage->GetRenderPass()->GetRenderPass();
	pipelineCreateInfo.subpass = m_Stage.second;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;
	//TODO le m_ColorBlend est remis à 0, il doit passer par la mauvaise fonction
	GraphicManager::CheckVk(vkCreateGraphicsPipelines(*logicalDevice, pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline));

}

void PipelineGraphics::CreatePipelinePolygon()
{
	CreatePipeline();
}

void PipelineGraphics::CreatePipelineMrt()
{
	auto renderStage = Engine::Get()->GetGraphicManager()->GetRenderStage(m_Stage.first);
	uint32_t attachmentCount = renderStage->GetAttachmentCount(m_Stage.second);

	std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStates;
	blendAttachmentStates.reserve(attachmentCount);

	for (uint32_t i = 0; i < attachmentCount; i++)
	{
		VkPipelineColorBlendAttachmentState blendAttachmentState = {};
		blendAttachmentState.blendEnable = VK_TRUE;
		blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachmentStates.emplace_back(blendAttachmentState);
	}

	m_ColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(blendAttachmentStates.size());
	m_ColorBlendStateCreateInfo.pAttachments = blendAttachmentStates.data();

	CreatePipeline();
}
}
