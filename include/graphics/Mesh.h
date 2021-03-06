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

#ifndef MODEL_H
#define MODEL_H
#include <memory>
#include <graphics/command_buffer.h>
#include <graphics/buffers/buffer.h>
#include "mesh_vertex.h"
#include <glm/glm.hpp>

namespace dm
{
class Mesh
{
public:
	Mesh();

	virtual ~Mesh() = default;

	template<typename T>
	explicit Mesh(const std::vector<T> &vertices, const std::vector<uint32_t> &indices = {}) :
		Mesh()
	{
		Initialize(vertices, indices);
	}

	virtual void Load();

	bool CmdRender(const CommandBuffer &commandBuffer, const uint32_t &instance = 1) const;

	const Buffer *GetVertexBuffer() const { return m_VertexBuffer.get(); }

	const Buffer *GetIndexBuffer() const { return m_IndexBuffer.get(); }

	const uint32_t &GetVertexCount() const { return m_VertexCount; }

	const uint32_t &GetIndexCount() const { return m_IndexCount; }

	const float &GetRadius() const { return m_Radius; }

	static VkIndexType GetIndexType() { return VK_INDEX_TYPE_UINT32; }
protected:
	template<typename T>
	void Initialize(const std::vector<T> &vertices, const std::vector<uint32_t> &indices = {})
	{
		static_assert(std::is_base_of<VertexMesh, T>::value, "T must derive from ModelVertex");

		m_VertexBuffer = nullptr;
		m_IndexBuffer = nullptr;

		if (!vertices.empty())
		{
			const auto vertexStaging = Buffer(sizeof(T) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				vertices.data());
			m_VertexBuffer = std::make_unique<Buffer>(sizeof(T) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			m_VertexCount = static_cast<uint32_t>(vertices.size());

			auto commandBuffer = CommandBuffer();

			VkBufferCopy copyRegion = {};
			copyRegion.size = sizeof(T) * vertices.size();
			vkCmdCopyBuffer(commandBuffer, vertexStaging.GetBuffer(), m_VertexBuffer->GetBuffer(), 1, &copyRegion);

			commandBuffer.SubmitIdle();
		}

		if (!indices.empty())
		{
			auto indexStaging = Buffer(sizeof(uint32_t) * indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());
			m_IndexBuffer = std::make_unique<Buffer>(sizeof(uint32_t) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			m_IndexCount = static_cast<uint32_t>(indices.size());

			CommandBuffer commandBuffer = CommandBuffer();

			VkBufferCopy copyRegion = {};
			copyRegion.size = sizeof(uint32_t) * indices.size();
			vkCmdCopyBuffer(commandBuffer, indexStaging.GetBuffer(), m_IndexBuffer->GetBuffer(), 1, &copyRegion);

			commandBuffer.SubmitIdle();
		}

		m_MinExtents = glm::vec3(std::numeric_limits<float>::max());
		m_MaxExtents = glm::vec3(std::numeric_limits<float>::min());

		for (const auto &vertex : vertices)
		{
			glm::vec3 position = glm::vec3(vertex.position.x, vertex.position.y, vertex.position.z);
			m_MinExtents = glm::vec3(std::min(m_MinExtents.x, position.x), std::min(m_MinExtents.y, position.y), std::min(m_MinExtents.z, position.z));
			m_MaxExtents = glm::vec3(std::max(m_MaxExtents.x, position.x), std::max(m_MaxExtents.y, position.y), std::max(m_MaxExtents.z, position.z));
			//m_MaxExtents = glm::max(m_MaxExtents, position);
		}
		
		m_Radius = std::max(glm::length(m_MinExtents), glm::length(m_MaxExtents));
	}
private:
	std::unique_ptr<Buffer> m_VertexBuffer;
	std::unique_ptr<Buffer> m_IndexBuffer;

	uint32_t m_VertexCount;
	uint32_t m_IndexCount;

	glm::vec3 m_MinExtents;
	glm::vec3 m_MaxExtents;
	float m_Radius;
};
}

#endif MODEL_H