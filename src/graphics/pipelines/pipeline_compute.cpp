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

#include <graphics/pipelines/pipeline_compute.h>
#include <engine/engine.h>
#include <graphics/graphic_manager.h>
#include <engine/file.h>

namespace dm
{
PipelineCompute::PipelineCompute(std::string shaderStage, std::vector<Shader::Define> defines,
	const bool& pushDescriptor) :
	m_ShaderStage(std::move(shaderStage)),
	m_Defines(std::move(defines)),
	m_PushDescriptor(pushDescriptor),
	m_Shader(std::make_unique<Shader>(m_ShaderStage)),
	m_ShaderModule(VK_NULL_HANDLE),
	m_ShaderStageCreateInfo({}),
	m_DescriptorSetLayout(VK_NULL_HANDLE),
	m_DescriptorPool(VK_NULL_HANDLE),
	m_Pipeline(VK_NULL_HANDLE),
	m_PipelineLayout(VK_NULL_HANDLE),
	m_PipelineBindPoint(VK_PIPELINE_BIND_POINT_COMPUTE)
{
	CreateShaderProgram();
	CreateDescriptorLayout();
	CreateDescriptorPool();
	CreatePipelineLayout();
	CreatePipelineCompute();
}

PipelineCompute::~PipelineCompute()
{
	auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	vkDestroyShaderModule(*logicalDevice, m_ShaderModule, nullptr);

	vkDestroyDescriptorSetLayout(*logicalDevice, m_DescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(*logicalDevice, m_DescriptorPool, nullptr);
	vkDestroyPipeline(*logicalDevice, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(*logicalDevice, m_PipelineLayout, nullptr);
}

void PipelineCompute::Draw(const CommandBuffer& commandBuffer, const glm::vec2& extent) const
{
	const auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(extent.x) / static_cast<float>(*m_Shader->GetLocalSizes()[0])));
	const auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(extent.y) / static_cast<float>(*m_Shader->GetLocalSizes()[1])));
	
	vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);
}

void PipelineCompute::CreateShaderProgram()
{
	std::stringstream defineBlock;

	defineBlock << "\n";

	for(const auto &define : m_Defines)
	{
		defineBlock << "#define " << define.first << " " << define.second << "\n";
	}

	auto fileLoaded = Files::Read(m_ShaderStage);

	if(!fileLoaded)
	{
		std::cout << "Shader stage could not be loaded : " << m_ShaderStage.c_str() << "\n";
		throw std::runtime_error("Could not create compute pipeline\n");
	}

	auto shaderCode = Shader::InsertDefineBlock(*fileLoaded, defineBlock.str());
	shaderCode = Shader::ProcessIncludes(shaderCode);

	const auto stageFlag = Shader::GetShaderStage(m_ShaderStage);
	m_ShaderModule = m_Shader->ProcessShader(shaderCode, stageFlag);

	m_ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_ShaderStageCreateInfo.stage = stageFlag;
	m_ShaderStageCreateInfo.module = m_ShaderModule;
	m_ShaderStageCreateInfo.pName = "main";

	m_Shader->ProcessShader();
}

void PipelineCompute::CreateDescriptorLayout()
{
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	auto descriptorSetLayouts = m_Shader->GetDescriptorSetLayouts();

	VkDescriptorSetLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.flags = m_PushDescriptor ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
	createInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	createInfo.pBindings = descriptorSetLayouts.data();
	GraphicManager::CheckVk(vkCreateDescriptorSetLayout(*logicalDevice, &createInfo, nullptr, &m_DescriptorSetLayout));
}

void PipelineCompute::CreateDescriptorPool()
{
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	auto descriptorPool = m_Shader->GetDescriptorPools();

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	createInfo.maxSets = 8192;
	createInfo.poolSizeCount = static_cast<uint32_t>(descriptorPool.size());
	createInfo.pPoolSizes = descriptorPool.data();

	GraphicManager::CheckVk(vkCreateDescriptorPool(*logicalDevice, &createInfo, nullptr, &m_DescriptorPool));
}

void PipelineCompute::CreatePipelineLayout()
{
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();

	auto pushConstantRanges = m_Shader->GetPushConstantRanges();

	VkPipelineLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = 1;
	createInfo.pSetLayouts = &m_DescriptorSetLayout;
	createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	createInfo.pPushConstantRanges = pushConstantRanges.data();

	GraphicManager::CheckVk(vkCreatePipelineLayout(*logicalDevice, &createInfo, nullptr, &m_PipelineLayout));
}

void PipelineCompute::CreatePipelineCompute()
{
	const auto logicalDevice = GraphicManager::Get()->GetLogicalDevice();
	const auto pipelineCache = GraphicManager::Get()->GetPipelineCache();

	VkComputePipelineCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.stage = m_ShaderStageCreateInfo;
	createInfo.layout = m_PipelineLayout;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;

	GraphicManager::CheckVk(vkCreateComputePipelines(*logicalDevice, pipelineCache, 1, &createInfo, nullptr, &m_Pipeline));
}
}
