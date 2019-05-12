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

#ifndef MODEL_VERTEX_H
#define MODEL_VERTEX_H
#include <engine/vector.h>
#include <graphics/shader.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace dm
{
struct VertexMesh
{
	VertexMesh(const Vec3f &pos, const Vec2f &uv, const Vec3f &normal);

	VertexMesh(const glm::vec3 &pos, const glm::vec2 &uv, const glm::vec3 &normal);

	static Shader::VertexInput GetVertexInput(const uint32_t &binding = 0);

	bool operator==(const VertexMesh &other) const
	{
		return position == other.position && uv == other.uv && normal == other.normal;
	}

	bool operator!=(const VertexMesh &other) const
	{
		return !(*this == other);
	}

	Vec3f position;
	Vec2f uv;
	Vec3f normal;
};
}

namespace std
{
	template<>
	struct hash<dm::VertexMesh>
	{
		size_t operator()(const dm::VertexMesh &vertex) const noexcept
		{
			size_t seed = 0;
			HashCombine(seed, vertex.position);
			HashCombine(seed, vertex.uv);
			HashCombine(seed, vertex.normal);
			return seed;
		}
	};
}
#endif MODEL_VERTEX_H