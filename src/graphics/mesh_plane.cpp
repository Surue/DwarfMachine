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
std::unique_ptr<MeshPlane> MeshPlane::Create(glm::vec2 extent, int patchSize)
{
	return std::make_unique<MeshPlane>(extent, patchSize);
}

MeshPlane::MeshPlane(const glm::vec2 extent, int patchSize) :
	m_Extent(extent),
	m_PatchSize(patchSize)
{
	MeshPlane::Load();
}

MeshPlane::~MeshPlane() {}

void MeshPlane::Load()
{
	std::vector<VertexMesh> vertices;
	std::vector<uint32_t> indices;

	for (auto x = 0; x <= m_PatchSize; x++)
	{
		for (auto y = 0; y <= m_PatchSize; y++)
		{
			vertices.emplace_back(glm::vec3((static_cast<float>(x) / m_PatchSize) * m_Extent.x - m_Extent.x / 2.0f, 0.0, (static_cast<float>(y) / m_PatchSize) * m_Extent.y - m_Extent.y / 2.0f), glm::vec2((float)x / m_PatchSize, (float)y / m_PatchSize), glm::vec3(0.0f, 1.0f, 0.0f));
		}
	}


	int vert = 0;
	int tris = 0;
	indices.resize(m_PatchSize * m_PatchSize * 6);

	for(int x = 0; x < m_PatchSize; x++)
	{
		for(int y = 0; y < m_PatchSize; y++)
		{
			indices[tris] = (vert);
			indices[tris + 2] = (vert + m_PatchSize + 1);
			indices[tris + 1] = (vert + 1);

			indices[tris + 4] = (vert + 1);
			indices[tris + 3] = (vert + m_PatchSize + 1);
			indices[tris + 5] = (vert + m_PatchSize + 2);

			vert++;

			tris += 6;
		}

		vert++;
	}

	Initialize(vertices, indices);
}
}
