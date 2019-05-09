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
//Matrix4::Matrix4(const float& diagonal)
//{
//	std::memset(m_Rows, 0, 4 * sizeof(Vec4f));
//	m_Rows[0][0] = diagonal;
//	m_Rows[1][1] = diagonal;
//	m_Rows[2][2] = diagonal;
//	m_Rows[3][3] = diagonal;
//}
//
//Matrix4 Matrix4::Translate(const Vec3f& other) const
//{
//	Matrix4 result = Matrix4(*this);
//
//	for (int32_t col = 0; col < 4; col++)
//	{
//		result[3][col] += m_Rows[0][col] * other.x + m_Rows[1][col] * other.y + m_Rows[2][col] * other.z;
//	}
//
//	return result;
//}
//
//Matrix4 Matrix4::Rotate(const float& angle, const Vec3f& axis)
//{
//	Matrix4 result = Matrix4(*this);
//
//	float c = std::cos(angle);
//	float s = std::sin(angle);
//	float o = 1.0f - c;
//	float xy = axis.x * axis.y;
//	float yz = axis.y * axis.z;
//	float xz = axis.x * axis.z;
//	float xs = axis.x * s;
//	float ys = axis.y * s;
//	float zs = axis.z * s;
//
//	Matrix3 f = Matrix3();
//	f[0][0] = axis.x * axis.x * o + c;
//	f[0][1] = xy * o + zs;
//	f[0][2] = xz * o - ys;
//	f[1][0] = xy * o - zs;
//	f[1][1] = axis.y * axis.y * o + c;
//	f[1][2] = yz * o + xs;
//	f[2][0] = xz * o + ys;
//	f[2][1] = yz * o - xs;
//	f[2][2] = axis.z * axis.z * o + c;
//
//	for (int32_t row = 0; row < 3; row++)
//	{
//		for (int32_t col = 0; col < 4; col++)
//		{
//			result[row][col] = m_Rows[0][col] * f[row][0] + m_Rows[1][col] * f[row][1] + m_Rows[2][col] * f[row][2];
//		}
//	}
//
//	return result;
//}
//
//Matrix4 Matrix4::Scale(const Vec3f& other) const
//{
//	auto result = Matrix4(*this);
//
//	for (auto row = 0; row < 3; row++)
//	{
//		for (auto col = 0; col < 4; col++)
//		{
//			result[row][col] *= other[row];
//		}
//	}
//
//	return result;
//}
//
//Matrix4 Matrix4::TransformationMatrix(const Vec3f& translation, const Vec3f& rotation, const Vec3f& scale)
//{
//	Matrix4 result = Matrix4();
//	result = result.Translate(translation);
//	result = result.Rotate(rotation.x, Vec3f::Right);
//	result = result.Rotate(rotation.y, Vec3f::Up);
//	result = result.Rotate(rotation.z, Vec3f::Front);
//	result = result.Scale(scale);
//	return result;
//}
//
//Matrix4 Matrix4::ViewMatrix(const Vec3f& position, const Vec3f& rotation)
//{
//	auto result = Matrix4();
//	result = result.Rotate(rotation.x, Vec3f::Right);
//	result = result.Rotate(rotation.y, Vec3f::Up);
//	result = result.Rotate(rotation.z, Vec3f::Front);
//	result = result.Translate(position.Negate());
//	return result;
//}
//
//Matrix4 Matrix4::LookAt(const Vec3f& eye, const Vec3f& centre, const Vec3f& up)
//{
//	const auto f = (centre - eye).Normalized();
//	const auto s = Vec3f::Cross(f, up).Normalized();
//	const auto u = Vec3f::Cross(s, f);
//
//	auto result = Matrix4();
//	result[0][0] = s.x;
//	result[1][0] = s.y;
//	result[2][0] = s.z;
//	result[0][1] = u.x;
//	result[1][1] = u.y;
//	result[2][1] = u.z;
//	result[0][2] = -f.x;
//	result[1][2] = -f.y;
//	result[2][2] = -f.z;
//	result[3][0] = -Vec3f::Dot(s, eye);
//	result[3][1] = -Vec3f::Dot(u, eye);
//	result[3][2] = Vec3f::Dot(f, eye);
//	return result;
//}
//
//Matrix4 Matrix4::PerspectiveMatrix(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar)
//{
//	auto result = Matrix4(0.0f);
//
//	const auto f = std::tan(0.5f * fov);
//
//	result[0][0] = 1.0f / (aspectRatio * f);
//	result[1][1] = 1.0f / f;
//	result[2][2] = zFar / (zNear - zFar);
//	result[2][3] = -1.0f;
//	result[3][2] = -(zFar * zNear) / (zFar - zNear);
//	return result;
//}
}
