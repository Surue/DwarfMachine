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

#include <graphics/model_cube.h>
#include "graphics/model_vertex.h"

namespace dm
{
std::shared_ptr<ModelCube> ModelCube::Create(const Vec3f& extents)
{
	return std::make_shared<ModelCube>(extents);
}

ModelCube::ModelCube(const Vec3f& extents, const bool& load) :
	m_Extents(extents)
{
	if(load)
	{
		ModelCube::Load();
	}
}

void ModelCube::Load()
{
	if(m_Extents.x <= 0 || m_Extents.y <= 0|| m_Extents.z <= 0)
	{
		return;
	}

	static std::vector<VertexModel> vertices  = {
		VertexModel(Vec3f(-0.5f, -0.5f, 0.5f), Vec2f(0.375f, 1.0f), Vec3f(-1.0f, 0.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, 0.5f, 0.5f), Vec2f(0.625f, 1.0f), Vec3f(-1.0f, 0.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, -0.5f, -0.5f), Vec2f(0.375f, 0.75f), Vec3f(-1.0f, 0.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, 0.5f, -0.5f), Vec2f(0.625f, 0.75f), Vec3f(0.0f, 0.0f, -1.0f)),
		VertexModel(Vec3f(0.5f, -0.5f, 0.5f), Vec2f(0.375f, 0.25f), Vec3f(1.0f, 0.0f, 0.0f)),
		VertexModel(Vec3f(0.5f, 0.5f, 0.5f), Vec2f(0.625f, 0.25f), Vec3f(0.0f, 0.0f, 1.0f)),
		VertexModel(Vec3f(0.5f, -0.5f, -0.5f), Vec2f(0.375f, 0.5f), Vec3f(0.0f, 0.0f, -1.0f)),
		VertexModel(Vec3f(0.5f, 0.5f, -0.5f), Vec2f(0.625f, 0.5f), Vec3f(1.0f, 0.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, -0.5f, -0.5f), Vec2f(0.375f, 0.75f), Vec3f(0.0f, 0.0f, -1.0f)),
		VertexModel(Vec3f(0.5f, -0.5f, -0.5f), Vec2f(0.375f, 0.5f), Vec3f(1.0f, 0.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, -0.5f, 0.5f), Vec2f(0.375f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f)),
		VertexModel(Vec3f(0.5f, -0.5f, 0.5f), Vec2f(0.375f, 0.25f), Vec3f(0.0f, 0.0f, 1.0f)),
		VertexModel(Vec3f(0.5f, -0.5f, -0.5f), Vec2f(0.375f, 0.5f), Vec3f(0.0f, -1.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, -0.5f, 0.5f), Vec2f(0.125f, 0.25f), Vec3f(0.0f, -1.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, -0.5f, -0.5f), Vec2f(0.125f, 0.5f), Vec3f(0.0f, -1.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, 0.5f, -0.5f), Vec2f(0.875f, 0.5f), Vec3f(0.0f, 1.0f, 0.0f)),
		VertexModel(Vec3f(0.5f, 0.5f, 0.5f), Vec2f(0.625f, 0.25f), Vec3f(0.0f, 1.0f, 0.0f)),
		VertexModel(Vec3f(0.5f, 0.5f, -0.5f), Vec2f(0.625f, 0.5f), Vec3f(0.0f, 1.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, 0.5f, -0.5f), Vec2f(0.625f, 0.75f), Vec3f(-1.0f, 0.0f, 0.0f)),
		VertexModel(Vec3f(0.5f, 0.5f, -0.5f), Vec2f(0.625f, 0.5f), Vec3f(0.0f, 0.0f, -1.0f)),
		VertexModel(Vec3f(0.5f, 0.5f, 0.5f), Vec2f(0.625f, 0.25f), Vec3f(1.0f, 0.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, 0.5f, 0.5f), Vec2f(0.625f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f)),
		VertexModel(Vec3f(0.5f, -0.5f, 0.5f), Vec2f(0.375f, 0.25f), Vec3f(0.0f, -1.0f, 0.0f)),
		VertexModel(Vec3f(-0.5f, 0.5f, 0.5f), Vec2f(0.875f, 0.25f), Vec3f(0.0f, 1.0f, 0.0f)),
	};

	static std::vector<uint32_t> indices = {
		1, 2, 0, // Front
		3, 6, 8,
		7, 4, 9, // Back
		5, 10, 11,
		12, 13, 14, // Top
		15, 16, 17,
		1, 18, 2, // Bottom
		3, 19, 6,
		7, 20, 4, // Left
		5, 21, 10,
		12, 22, 13, // Right
		15, 23, 16,
	};

	for (auto &vertex : vertices)
	{
		vertex.position *= m_Extents;
	}

	Initialize(vertices, indices);
}
}
