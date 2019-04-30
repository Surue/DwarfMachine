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

#include <engine/matrix_4.h>

namespace dm
{
Matrix4::Matrix4(const float& diagonal)
{
	memset(m_rows, 0, 4 * sizeof(Vec4f));
	m_rows[0].x = diagonal;
	m_rows[1].y = diagonal;
	m_rows[2].z = diagonal;
	m_rows[3].w = diagonal;
}

Matrix4 Matrix4::Translate(const Vec3f& other) const
{
	auto result = Matrix4(*this);

	for (auto col = 0; col < 4; col++)
	{
		result[3][col] += m_rows[0][col] * other.x + m_rows[1][col] * other.y + m_rows[2][col] * other.z;
	}

	return result;
}

Matrix4 Matrix4::Rotate(const float& angle, const Vec3f& axis)
{
	auto result = Matrix4(*this);

	const auto c = std::cos(angle);
	const auto s = std::sin(angle);
	const auto o = 1.0f - c;
	const auto xy = axis.x * axis.y;
	const auto yz = axis.y * axis.z;
	const auto xz = axis.x * axis.z;
	const auto xs = axis.x * s;
	const auto ys = axis.y * s;
	const auto zs = axis.z * s;

	auto f = Matrix3();
	f[0][0] = axis.x * axis.x * o + c;
	f[0][1] = xy * o + zs;
	f[0][2] = xz * o - ys;
	f[1][0] = xy * o - zs;
	f[1][1] = axis.y * axis.y * o + c;
	f[1][2] = yz * o + xs;
	f[2][0] = xz * o + ys;
	f[2][1] = yz * o - xs;
	f[2][2] = axis.z * axis.z * o + c;

	for (auto row = 0; row < 3; row++)
	{
		for (auto col = 0; col < 4; col++)
		{
			result[row][col] = m_rows[0][col] * f[row][0] + m_rows[1][col] * f[row][1] + m_rows[2][col] * f[row][2];
		}
	}

	return result;
}

Matrix4 Matrix4::Scale(const Vec3f& other) const
{
	auto result = Matrix4(*this);

	for (auto row = 0; row < 3; row++)
	{
		for (auto col = 0; col < 4; col++)
		{
			result[row][col] *= other[row];
		}
	}

	return result;
}
}
