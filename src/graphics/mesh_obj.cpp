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

#include <graphics/mesh_obj.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <utility/tiny_obj_loader.h>

namespace dm
{
std::unique_ptr<MeshObj> MeshObj::Create(const std::string& filename)
{
	return std::make_unique<MeshObj>(filename);
}

MeshObj::MeshObj(const std::string& filename): 
m_Filename(filename) {
	MeshObj::Load();
}

MeshObj::~MeshObj() {}

void MeshObj::Load()
{
	if (m_Filename.empty())
	{
		return;
	}

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, m_Filename.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	std::vector<VertexMesh> vertices;
	std::vector<uint32_t> indices;
	std::unordered_map<VertexMesh, size_t> uniqueVertices;

	for (const auto &shape : shapes)
	{
		for (const auto &index : shape.mesh.indices)
		{
			//Position
			glm::vec3 position = glm::vec3(
				attrib.vertices[3 * index.vertex_index + 0], 
				attrib.vertices[3 * index.vertex_index + 1], 
				attrib.vertices[3 * index.vertex_index + 2]);

			//UV
			glm::vec2 uv = glm::vec2(
				attrib.texcoords[2 * index.texcoord_index + 0], 
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);

			//Normal
			glm::vec3 normal;
			if (index.normal_index > 0) {
				normal = glm::vec3(
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]);
			}

			VertexMesh vertex = VertexMesh(position, uv, normal);

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = vertices.size();
				vertices.push_back(vertex);
			}

			indices.push_back(static_cast<uint32_t>(uniqueVertices[vertex]));
		}
	}

	Initialize(vertices, indices);
}
}
