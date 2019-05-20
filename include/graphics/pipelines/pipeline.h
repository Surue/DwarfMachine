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

#ifndef PIPELINE_H
#define PIPELINE_H
#include <utility>
#include <cstdint>
#include <graphics/command_buffer.h>
#include <graphics/pipelines/shader.h>

namespace dm
{
class Pipeline
{
public:
	/**
	 * \brief Represents position in the render structure, first value is the renderpass and the second one is the subpass
	 */
	using Stage = std::pair<uint32_t, uint32_t>;

	Pipeline() = default;

	virtual ~Pipeline() = default;

	void BindPipeline(const CommandBuffer &commandBuffer) const
	{
		vkCmdBindPipeline(commandBuffer, GetPipelineBindPoint(), GetPipeline());
	}

	virtual const Shader *GetShader() const = 0;

	virtual const bool &IsPushDescriptor() const = 0;

	virtual const VkDescriptorSetLayout &GetDescriptorSetLayout() const = 0;

	virtual const VkDescriptorPool &GetDescriptorPool() const = 0;

	virtual const VkPipeline &GetPipeline() const = 0;

	virtual const VkPipelineLayout &GetPipelineLayout() const = 0;

	virtual const VkPipelineBindPoint &GetPipelineBindPoint() const = 0;
};
}

#endif PIPELINE_H