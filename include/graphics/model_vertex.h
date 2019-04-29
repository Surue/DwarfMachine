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
#include "engine/vector.h"
#include "shader.h"

namespace dm
{
struct VertexModel
{
	VertexModel(const Vec3f &pos, const Vec2f &uv, const Vec3f &normal);

	static Shader::VertexInput GetVertexInput(const uint32_t &binding = 0);

	bool operator==(const VertexModel &other) const
	{
		return position == other.position && uv == other.uv && normal == other.normal;
	}

	bool operator!=(const VertexModel &other) const
	{
		return !(*this == other);
	}

	Vec3f position;
	Vec2f uv;
	Vec3f normal;
};
}

#endif MODEL_VERTEX_H