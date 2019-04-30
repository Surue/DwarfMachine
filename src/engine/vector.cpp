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

#include <engine/vector.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <SDL_stdinc.h>

namespace dm
{
Vec2i::Vec2i() : x(0), y (0) {}

Vec2i::Vec2i(const int x, const int y) : x(x), y(y) {}

bool Vec2i::operator==(const Vec2i& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

bool Vec2i::operator!=(const Vec2i& rhs) const
{
	return x != rhs.x || y != rhs.y;
}

Vec2i Vec2i::operator+(const Vec2i& rhs) const
{
	return Vec2i(x + rhs.x, y + rhs.y);
}

Vec2i& Vec2i::operator+=(const Vec2i& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

Vec2i Vec2i::operator-(const Vec2i& rhs) const
{
	return Vec2i(x - rhs.x, y - rhs.y);
}

Vec2i& Vec2i::operator-=(const Vec2i& rhs)
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

Vec2i Vec2i::operator*(const int rhs) const
{
	return Vec2i(x * rhs, y * rhs);
}

Vec2i& Vec2i::operator*=(const int rhs)
{
	x *= rhs;
	y *= rhs;

	return *this;
}
Vec2i Vec2i::operator/(const float rhs) const
{
	return Vec2i(static_cast<int>(x / rhs), static_cast<int>(y / rhs));
}

Vec2i& Vec2i::operator/=(const float rhs)
{
	x = static_cast<int>(x / rhs);
	y = static_cast<int>(y / rhs);

	return *this;
}

float Vec2i::GetMagnitude() const
{
	return sqrtf(x * x + y * y);
}

Vec2i Vec2i::Lerp(const Vec2i& v1, const Vec2i& v2, const float t)
{
	return v1 + (v2 - v1)*t;
}

float Vec2i::AngleBetween(const Vec2i& v1, const Vec2i& v2)
{
	const auto dot = Dot(v1, v2);
	const float angle = acosf(dot) / M_PI * 180.0f;
	return (dot < 0.0f ? -1.0f : 1.0f) * angle;
}

float Vec2i::Dot(const Vec2i& v1, const Vec2i& v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

Vec2f::Vec2f(): x(0), y(0){}

Vec2f::Vec2f(const float x, const float y) : x(x), y (y){}

Vec2f::Vec2f(const Vec2i v)
{
	x = v.x;
	y = v.y;
}

bool Vec2f::operator==(const Vec2f& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

bool Vec2f::operator!=(const Vec2f& rhs) const
{
	return x != rhs.x || y != rhs.y;
}

bool Vec2f::operator==(const Vec2i& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

bool Vec2f::operator!=(const Vec2i& rhs) const
{
	return x != rhs.x || y != rhs.y;
}

Vec2f Vec2f::operator+(const Vec2f& rhs) const
{
	return Vec2f(x + rhs.x, y + rhs.y);
}

Vec2f& Vec2f::operator+=(const Vec2f& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

Vec2f Vec2f::operator+(const Vec2i& rhs) const
{
	return Vec2f(x + rhs.x, y + rhs.y);
}

Vec2f& Vec2f::operator+=(const Vec2i& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

Vec2f Vec2f::operator-(const Vec2f& rhs) const
{
	return Vec2f(x - rhs.x, y - rhs.y);
}

Vec2f& Vec2f::operator-=(const Vec2f& rhs)
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

Vec2f Vec2f::operator-(const Vec2i& rhs) const
{
	return Vec2f(x - rhs.x, y - rhs.y);
}

Vec2f& Vec2f::operator-=(const Vec2i& rhs)
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

Vec2f Vec2f::operator*(const int rhs) const
{
	return Vec2f(x * rhs, y * rhs);
}

Vec2f& Vec2f::operator*=(const int rhs)
{
	x *= rhs;
	y *= rhs;

	return *this;
}

Vec2f Vec2f::operator*(const float rhs) const
{
	return Vec2f(x * rhs, y * rhs);
}

Vec2f& Vec2f::operator*=(const float rhs)
{
	x *= rhs;
	y *= rhs;

	return *this;
}

Vec2f Vec2f::operator/(const float rhs) const
{
	return Vec2f(x / rhs, y / rhs);
}

Vec2f& Vec2f::operator/=(const float rhs)
{
	x /= rhs;
	y /= rhs;

	return *this;
}

float Vec2f::GetMagnitude() const
{
	return sqrt(x * x + y * y);
}

Vec2f Vec2f::Normalized() const
{
	const auto magnitude = GetMagnitude();

	if (magnitude != 0) {
		return Vec2f(x / magnitude, y / magnitude);
	}else
	{
		return Vec2f(x, y);
	}
}

Vec2f Vec2f::Lerp(const Vec2f& v1, const Vec2f& v2, const float t)
{
	return v1 + (v2 - v1)*t;
}

float Vec2f::AngleBetween(const Vec2f& v1, const Vec2f& v2)
{
	const auto dot = Dot(v1, v2);
	const float angle = acosf(dot) / M_PI * 180.0f;
	return (dot < 0.0f ? -1.0f : 1.0f) * angle;
}

float Vec2f::Dot(const Vec2f& v1, const Vec2f& v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

const Vec3f Vec3f::Zero = Vec3f(0.0f, 0.0f, 0.0f);
const Vec3f Vec3f::One = Vec3f(1.0f, 1.0f, 1.0f);
const Vec3f Vec3f::Left = Vec3f(-1.0f, 0.0f, 0.0f);
const Vec3f Vec3f::Right = Vec3f(1.0f, 0.0f, 0.0f);
const Vec3f Vec3f::Up = Vec3f(0.0f, 1.0f, 0.0f);
const Vec3f Vec3f::Down = Vec3f(0.0f, -1.0f, 0.0f);
const Vec3f Vec3f::Front = Vec3f(0.0f, 0.0f, 1.0f);
const Vec3f Vec3f::Back = Vec3f(0.0f, 0.0f, -1.0f);

Vec3f::Vec3f() : x(0), y(0), z(0) {}

Vec3f::Vec3f(const float x, const float y, const float z) : x(x), y(y), z(z){}

Vec3f::Vec3f(const Vec2i v)
{
	x = v.x;
	y = v.y;
	z = 0;
}

Vec3f::Vec3f(const Vec2f v)
{
	x = v.x;
	y = v.y;
	z = 0;
}

bool Vec3f::operator==(const Vec3f& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vec3f::operator!=(const Vec3f& rhs) const
{
	return x != rhs.x || y != rhs.y || z != rhs.z;
}

Vec3f Vec3f::operator+(const Vec3f& rhs) const
{
	return Vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3f& Vec3f::operator+=(const Vec3f& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

Vec3f Vec3f::operator-(const Vec3f& rhs) const
{
	return Vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3f& Vec3f::operator-=(const Vec3f& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

Vec3f Vec3f::operator*(const int rhs) const
{
	return Vec3f(x * rhs, y * rhs, z * rhs);
}

Vec3f& Vec3f::operator*=(const int rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;

	return *this;
}

Vec3f Vec3f::operator*(const float rhs) const
{
	return Vec3f(x * rhs, y * rhs, z * rhs);
}

Vec3f& Vec3f::operator*=(const float rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;

	return *this;
}

Vec3f Vec3f::operator/(const float rhs) const
{
	return Vec3f(x / rhs, y / rhs, z / rhs);
}

Vec3f& Vec3f::operator/=(const float rhs)
{
	x /= rhs;
	y /= rhs;
	z /= rhs;

	return *this;
}

float Vec3f::GetMagnitude() const
{
	return sqrt(x * x + y * y + z * z);
}

Vec3f Vec3f::Normalized() const
{
	return *this / GetMagnitude();
}

Vec3f Vec3f::Lerp(const Vec3f& v1, const Vec3f& v2, float t)
{
	return v1 + (v2 - v1)*t;
}

float Vec3f::AngleBetween(const Vec3f& v1, const Vec3f& v2)
{
	const auto dot = Dot(v1, v2);
	const float angle = acosf(dot) / M_PI * 180.0f;
	return (dot < 0.0f ? -1.0f : 1.0f) * angle;
}

float Vec3f::Dot(const Vec3f& v1, const Vec3f& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
}

Vec3f Vec3f::Cross(const Vec3f& v1, const Vec3f& v2)
{
	return Vec3f(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

Vec4f::Vec4f(): x(0), y(0), z(0), w(0) {}

Vec4f::Vec4f(const float x, const float y, const float z, const float w):
	x(x),
	y(y),
	z(z),
	w(w) { }

const float& Vec4f::operator[](const uint32_t& index) const
{
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		throw std::runtime_error("Vector4 index out of bounds!");
	}
}

float& Vec4f::operator[](const uint32_t& index)
{
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		throw std::runtime_error("Vector4 index out of bounds!");
	}
}
}
