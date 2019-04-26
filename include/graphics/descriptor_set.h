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

#ifndef DESCRIPTOR_SET_H
#define DESCRIPTOR_SET_H

#include <graphics/command_buffer.h>
#include <graphics/pipeline.h>

namespace dm
{
class Descriptor;
class WriteDescriptorSet;

class DescriptorSet
{
public:
	explicit DescriptorSet(const Pipeline &pipeline);

	~DescriptorSet();

	void Update(const std::vector<VkWriteDescriptorSet> &descriptorWrites);

	void BindDescriptor(const CommandBuffer &commandBuffer);

	const VkDescriptorSet &GetDescriptorSet() const { return m_DescriptorSet; }
private:
	VkPipelineLayout m_PipelineLayout;
	VkPipelineBindPoint m_PipelineBindPoint;
	VkDescriptorPool m_DescriptorPool;
	VkDescriptorSet m_DescriptorSet;
};
}

#endif DESCRIPTOR_SET_H