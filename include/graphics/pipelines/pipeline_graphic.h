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

#ifndef PIPELINE_GRAPHIC_H
#define PIPELINE_GRAPHIC_H

#include <vector>
#include <vulkan/vulkan.h>
#include <graphics/pipelines/pipeline.h>
#include <glm/vec2.hpp>

namespace dm
{
class ImageDepth;
class Image2d;

class PipelineGraphics : public Pipeline
{
public:
	enum class Mode
	{
		POLYGON,
		MRT
	};

	enum class Depth
	{
		NONE = 0,
		READ = 1,
		WRITE = 2,
		READ_WRITE = READ | WRITE
	};

	PipelineGraphics(Stage stage, std::vector<std::string> shaderStages, std::vector<Shader::VertexInput> vertexInputs, std::vector<Shader::Define> defines = {},
		const Mode &mode = Mode::POLYGON, const Depth &depthMode = Depth::READ_WRITE, const VkPrimitiveTopology &topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		const VkPolygonMode &polygonMode = VK_POLYGON_MODE_FILL, const VkCullModeFlags &cullMode = VK_CULL_MODE_BACK_BIT, const VkFrontFace &frontFace = VK_FRONT_FACE_CLOCKWISE,
		const bool &pushDescriptors = false);

	~PipelineGraphics();

	const ImageDepth *GetDepthStencil(const std::optional<uint32_t> &stage = {}) const;

	const Image2d *GetTexture(const uint32_t &index, const std::optional<uint32_t> &stage = {}) const;

	glm::vec2 GetSize(const std::optional<uint32_t> &stage = {}) const;

	float GetAspectRatio(const std::optional<uint32_t> &stage = {}) const;

	const Stage &GetStage() const { return m_Stage; }

	const std::vector<std::string> &GetShaderStages() const { return m_ShaderStages; }

	const std::vector<Shader::VertexInput> &GetVertexInputs() const { return m_VertexInputs; }

	const std::vector<Shader::Define> &GetDefines() const { return m_Defines; }

	const Mode &GetMode() const { return m_Mode; }

	const Depth &GetDepth() const { return m_Depth; }

	const VkPrimitiveTopology &GetTopology() const { return m_Topology; }

	const VkPolygonMode &GetPolygonMode() const { return m_PolygonMode; }

	const VkCullModeFlags &GetCullMode() const { return m_CullMode; }

	const VkFrontFace &GetFrontFace() const { return m_FrontFace; }

	const bool &IsPushDescriptor() const override { return m_PushDescriptors; }

	const Shader *GetShader() const override { return m_Shader.get(); }

	const VkDescriptorSetLayout &GetDescriptorSetLayout() const override { return m_DescriptorSetLayout; }

	const VkDescriptorPool &GetDescriptorPool() const override { return m_DescriptorPool; }

	const VkPipeline &GetPipeline() const override { return m_Pipeline; }

	const VkPipelineLayout &GetPipelineLayout() const override { return m_PipelineLayout; }

	const VkPipelineBindPoint &GetPipelineBindPoint() const override { return m_PipelineBindPoint; }

private:
	void CreateShaderProgram();

	void CreateDescriptorLayout();

	void CreateDescriptorPool();

	void CreatePipelineLayout();

	void CreateAttributes();

	void CreatePipeline();

	void CreatePipelinePolygon();

	void CreatePipelineMrt();

	Stage m_Stage;
	std::vector<std::string> m_ShaderStages;
	std::vector<Shader::VertexInput> m_VertexInputs;
	std::vector<Shader::Define> m_Defines;
	Mode m_Mode;
	Depth m_Depth;
	VkPrimitiveTopology m_Topology;
	VkPolygonMode m_PolygonMode;
	VkCullModeFlags m_CullMode;
	VkFrontFace m_FrontFace;
	bool m_PushDescriptors;

	std::unique_ptr<Shader> m_Shader;

	std::vector<VkDynamicState> m_DynamicStates;

	std::vector<VkShaderModule> m_Modules;
	std::vector<VkPipelineShaderStageCreateInfo> m_Stages;

	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkDescriptorPool m_DescriptorPool;

	VkPipeline m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
	VkPipelineBindPoint m_PipelineBindPoint;

	VkPipelineVertexInputStateCreateInfo m_VertexInputStateCreateInfo;
	VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyStateCreateInfo;
	VkPipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo;
	std::array<VkPipelineColorBlendAttachmentState, 1> m_BlendAttachmentStates;
	VkPipelineColorBlendStateCreateInfo m_ColorBlendStateCreateInfo;
	VkPipelineDepthStencilStateCreateInfo m_DepthStencilStateCreateInfo;
	VkPipelineViewportStateCreateInfo m_ViewportStateCreateInfo;
	VkPipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo;
	VkPipelineDynamicStateCreateInfo m_DynamicStateCreateInfo;
	VkPipelineTessellationStateCreateInfo m_TessellationStateCreateInfo;
};

class PipelineGraphicsCreate
{
public:
	explicit PipelineGraphicsCreate(std::vector<std::string> shaderStages = {}, std::vector<Shader::VertexInput> vertexInputs = {}, std::vector<Shader::Define> defines = {},
		const PipelineGraphics::Mode &mode = PipelineGraphics::Mode::POLYGON, const PipelineGraphics::Depth &depth = PipelineGraphics::Depth::READ_WRITE,
		const VkPrimitiveTopology &topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, const VkPolygonMode &polygonMode = VK_POLYGON_MODE_FILL,
		const VkCullModeFlags &cullMode = VK_CULL_MODE_BACK_BIT, const VkFrontFace &frontFace = VK_FRONT_FACE_CLOCKWISE, const bool &pushDescriptors = false) :
		m_ShaderStages(std::move(shaderStages)),
		m_VertexInputs(std::move(vertexInputs)),
		m_Defines(std::move(defines)),
		m_Mode(mode),
		m_Depth(depth),
		m_Topology(topology),
		m_PolygonMode(polygonMode),
		m_CullMode(cullMode),
		m_FrontFace(frontFace),
		m_PushDescriptor(pushDescriptors)
	{}

	PipelineGraphics *Create(const Pipeline::Stage &pipelineStage) const
	{
		return new PipelineGraphics(pipelineStage, m_ShaderStages, m_VertexInputs, m_Defines, m_Mode, m_Depth, m_Topology, m_PolygonMode, m_CullMode, m_FrontFace, m_PushDescriptor);
	}

	const std::vector<std::string> &GetShaderStages() const { return m_ShaderStages; }

	const std::vector<Shader::VertexInput> &GetVertexInputs() const { return m_VertexInputs; }

	const std::vector<Shader::Define> &GetDefines() const { return m_Defines; }

	const PipelineGraphics::Mode &GetMode() const { return m_Mode; }

	const PipelineGraphics::Depth &GetDepth() const { return m_Depth; }

	const VkPrimitiveTopology &GetTopology() const { return m_Topology; }

	const VkPolygonMode &GetPolygonMode() const { return m_PolygonMode; }

	const VkCullModeFlags &GetCullMode() const { return m_CullMode; }

	const VkFrontFace &GetFrontFace() const { return m_FrontFace; }

	const bool &IsPushDescriptor() const { return m_PushDescriptor; }

	bool operator==(const PipelineGraphicsCreate &other) const
	{
		return m_ShaderStages == other.m_ShaderStages &&
			m_Defines == other.m_Defines &&
			m_Mode == other.m_Mode &&
			m_Depth == other.m_Depth &&
			m_Topology == other.m_Topology &&
			m_PolygonMode == other.m_PolygonMode &&
			m_CullMode == other.m_CullMode &&
			m_FrontFace == other.m_FrontFace &&
			m_PushDescriptor == other.m_PushDescriptor;
	}

	bool operator!=(const PipelineGraphicsCreate &other) const
	{
		return !(*this == other);
	}

private:
	std::vector<std::string> m_ShaderStages;
	std::vector<Shader::VertexInput> m_VertexInputs;
	std::vector<Shader::Define> m_Defines;

	PipelineGraphics::Mode m_Mode;
	PipelineGraphics::Depth m_Depth;
	VkPrimitiveTopology m_Topology;
	VkPolygonMode m_PolygonMode;
	VkCullModeFlags m_CullMode;
	VkFrontFace m_FrontFace;
	bool m_PushDescriptor;
};
}

#endif PIPELINE_GRAPHIC_H