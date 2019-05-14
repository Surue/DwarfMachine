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

#include <graphics/mesh_plane.h>

namespace dm
{
std::unique_ptr<MeshPlane> MeshPlane::Create(glm::vec2 extent)
{
	return std::make_unique<MeshPlane>(extent);
}

MeshPlane::MeshPlane(const glm::vec2 extent):
	m_Extent(extent)
{
	MeshPlane::Load();
}

MeshPlane::~MeshPlane() {}

void MeshPlane::Load()
{
	std::vector<VertexMesh> vertices = {
		VertexMesh(glm::vec3(-m_Extent.x, -m_Extent.y, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
		VertexMesh(glm::vec3(m_Extent.x, -m_Extent.y, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
		VertexMesh(glm::vec3(m_Extent.x, m_Extent.y, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
		VertexMesh(glm::vec3(-m_Extent.x, m_Extent.y, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
	};

	std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

	Initialize(vertices, indices);
}
}