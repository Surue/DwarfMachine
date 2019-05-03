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

#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <cstdint>
#include <engine/matrix_3.h>

namespace dm
{
class Matrix4
{
public:
	explicit Matrix4(const float& diagonal = 1.0f);

	const Vec4f &operator[](const uint32_t &index) const
	{
		return m_Rows[index];
	}

	Vec4f &operator[](const uint32_t &index)
	{
		return m_Rows[index];
	}

	Matrix4 Translate(const Vec3f& other) const;

	Matrix4 Rotate(const float& angle, const Vec3f& axis);

	Matrix4 Scale(const Vec3f& other) const;

	static Matrix4 TransformationMatrix(const Vec3f& translation, const Vec3f& rotation, const Vec3f& scale);

	static Matrix4 ViewMatrix(const Vec3f& position, const Vec3f& rotation);

	static Matrix4 LookAt(const Vec3f& eye, const Vec3f& centre, const Vec3f& up);

	static Matrix4 PerspectiveMatrix(const float &fov, const float &aspectRatio, const float &zNear, const float &zFar);
private:
	union
	{
		struct
		{
			Vec4f m_Rows[4];
		};

		struct
		{
			float m_Linear[16];
		};
	};
};
}

#endif MATRIX_4_H