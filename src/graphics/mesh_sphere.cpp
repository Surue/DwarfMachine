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

#include <graphics/mesh_sphere.h>
#define _USE_MATH_DEFINES 
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/ext/scalar_constants.inl>

namespace dm
{
std::unique_ptr<MeshSphere> MeshSphere::Create(const float radius)
{
	return std::make_unique<MeshSphere>(radius);
}

MeshSphere::MeshSphere(const float radius, const bool& load) :
	m_Radius(radius)
{
	if(load)
	{
		MeshSphere::Load();
	}
}

void MeshSphere::Load()
{
	if(m_Radius <= 0)
	{
		return;
	}

	uint32_t latitudeBands = 20;
	uint32_t longitudeBands = 20;

	std::vector<VertexMesh> vertices;
	std::vector<uint32_t> indices;

	for(uint32_t i = 0; i < longitudeBands + 1; i++)
	{
		float iDivLong = static_cast<float>(i) / static_cast<float>(longitudeBands);
		float theta = (i == 0 || i == longitudeBands) ? 0.0f : iDivLong * 2.0f * glm::pi<float>();

		for(uint32_t j = 0; j < latitudeBands + 1; j++)
		{
			float jDivLat = static_cast<float>(j) / static_cast<float>(latitudeBands);
			float phi = jDivLat * 2.0f * glm::pi<float>();

			glm::vec3 normal = glm::vec3(std::cos(phi) * std::sin(theta), std::cos(theta), std::sin(phi) * std::sin(theta));
			glm::vec3 position = m_Radius * normal;
			glm::vec2 uvs = glm::vec2(1.0f - jDivLat, 1.0f - iDivLong);
			vertices.emplace_back(VertexMesh(position, uvs, normal));
		}
	}

	for(uint32_t i = 0; i < longitudeBands; i++)
	{
		for (uint32_t j = 0; j < latitudeBands + 1; j++)
		{
			uint32_t first = j + ((latitudeBands + 1) * i);
			uint32_t second = j + ((latitudeBands + 1) * (i + 1));

			indices.emplace_back(second + 1);
			indices.emplace_back(first + 1);
			indices.emplace_back(first);
			indices.emplace_back(second);
			indices.emplace_back(second + 1);
			indices.emplace_back(first);
		}
	}

	Initialize(vertices, indices);
}
}
