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

#ifndef RENDERER_DIRECTIONAL_SHADOW
#define RENDERER_DIRECTIONAL_SHADOW
#include <graphics/render_pipeline.h>
#include "pipelines/pipeline_graphic.h"
#include "Mesh.h"

namespace dm
{
class RendererDirectionalShadow : public RenderPipeline
{
public:
	explicit RendererDirectionalShadow(const Pipeline::Stage& stage);

	static Shader::VertexInput GetVertexInput(const uint32_t &binding = 0)
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);

		//vertex input
		bindingDescriptions[0].binding = binding;
		bindingDescriptions[0].stride = sizeof(VertexMesh);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

		//position
		attributeDescriptions[0].binding = binding;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexMesh, position);

		//uv
		attributeDescriptions[1].binding = binding;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexMesh, uv);

		return Shader::VertexInput(binding, bindingDescriptions, attributeDescriptions);
	}

	void Update() override;

	void Draw(const CommandBuffer& commandBuffer) override;

	void RegisterEntity(const Entity entity) override;
private:
	PipelineGraphics m_Pipeline;
};
}

#endif
