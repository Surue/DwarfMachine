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

#ifndef PIPELINE_COMPUTE_H
#define PIPELINE_COMPUTE_H
#include <graphics/pipelines/pipeline.h>
#include <graphics/command_buffer.h>
#include <glm/vec2.hpp>

namespace dm
{

class PipelineCompute : public Pipeline
{
public:
	explicit PipelineCompute(std::string shaderStage, std::vector<Shader::Define> defines = {}, const bool &pushDescriptor = false);

	~PipelineCompute();

	const std::string &GetShaderStage() const { return m_ShaderStage; }

	const std::vector<Shader::Define> &GetDefines() const { return m_Defines; }

	const bool &IsPushDescriptor() const override { return m_PushDescriptor; }

	void Draw(const CommandBuffer &commandBuffer, const glm::vec2 &extent) const;

	const Shader *GetShader() const override { return m_Shader.get(); }

	const VkDescriptorSetLayout &GetDescriptorSetLayout() const override { return m_DescriptorSetLayout; }

	const VkDescriptorPool& GetDescriptorPool() const override { return m_DescriptorPool; }

	const VkPipeline& GetPipeline() const override { return m_Pipeline; }

	const VkPipelineLayout& GetPipelineLayout() const override { return m_PipelineLayout; }

	const VkPipelineBindPoint& GetPipelineBindPoint() const override { return m_PipelineBindPoint; }

private:
	void CreateShaderProgram();

	void CreateDescriptorLayout();

	void CreateDescriptorPool();

	void CreatePipelineLayout();

	void CreatePipelineCompute();

	std::string m_ShaderStage;
	std::vector<Shader::Define> m_Defines;
	bool m_PushDescriptor;

	std::unique_ptr<Shader> m_Shader;
	VkShaderModule m_ShaderModule;
	VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfo;

	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkDescriptorPool m_DescriptorPool;

	VkPipeline m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
	VkPipelineBindPoint m_PipelineBindPoint;
};
}

#endif