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

#ifndef GIZMO_TYPE
#define GIZMO_TYPE
#include "graphics/shader.h"
#include <glm/detail/type_mat4x4.hpp>
#include "engine/color.h"
#include <graphics/Mesh.h>
#include <graphics/buffers/uniform_handle.h>
#include <graphics/pipeline_graphic.h>
#include <graphics/descriptor_handle.h>
#include <graphics/buffers/instance_buffer.h>

namespace dm
{
class Gizmo;

class GizmoType
{
public:
	class Instance
	{
	public:
		static Shader::VertexInput GetVertexInput(const uint32_t &baseBinding = 0)
		{
			std::vector<VkVertexInputBindingDescription> bindingDescription = {
				VkVertexInputBindingDescription{ baseBinding, sizeof(Instance), VK_VERTEX_INPUT_RATE_INSTANCE}
			};

			std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
				VkVertexInputAttributeDescription{0, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, m_ModelMatrix) + offsetof(glm::mat4x4, 4 * sizeof(float)) },
				VkVertexInputAttributeDescription{0, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, m_ModelMatrix) + offsetof(glm::mat4x4, 4 * sizeof(float)) },
				VkVertexInputAttributeDescription{0, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, m_ModelMatrix) + offsetof(glm::mat4x4, 4 * sizeof(float)) },
				VkVertexInputAttributeDescription{0, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, m_ModelMatrix) + offsetof(glm::mat4x4, 4 * sizeof(float)) },
				VkVertexInputAttributeDescription{0, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, m_Color) },
			};
			return Shader::VertexInput(0, bindingDescription, attributeDescriptions);
		}

		glm::mat4x4 m_ModelMatrix;
		Color m_Color;
	};

	static GizmoType* Create(Mesh* mesh = nullptr, const float &lineThickness = 1.0f, const Color &color = Color::White);

	explicit GizmoType(Mesh* mesh, const float &lineThickness = 1.0f, const Color &color = Color::White);

	void Update(std::vector<std::unique_ptr<Gizmo>> &gizmos);

	bool CmdRender(const CommandBuffer &commandBuffer, const PipelineGraphics &pipeline, UniformHandle &uniformScene);

	const Mesh &GetMesh() const { return *m_Mesh; }

	void SetMesh(Mesh* mesh) { m_Mesh = mesh; }

	const float &GetLineThickness() const { return m_LineThickness; }

	void SetLineThickness(const float& lineThickness) { m_LineThickness = lineThickness; }

	const Color GetColor() const { return m_Color; }

	void SetColor(const Color& color) { m_Color = color; }

private:
	Mesh* m_Mesh;
	float m_LineThickness;
	Color m_Color;

	uint32_t m_MaxInstances;
	uint32_t m_Instances;

	DescriptorHandle m_DescriptorSet;
	InstanceBuffer m_InstanceBuffer;
};
}

#endif