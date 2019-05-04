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

#ifndef VECTOR_H
#define VECTOR_H
#include <cstdint>
#include <stdexcept>

namespace dm
{
class Vec2i final
{
public:
	int x;
	int y;

	Vec2i();
	Vec2i(int x, int y);

	~Vec2i() = default;

	bool operator==(const Vec2i &rhs) const;
	bool operator!=(const Vec2i &rhs) const;

	Vec2i operator+(const Vec2i &rhs) const;
	Vec2i& operator+=(const Vec2i &rhs);

	Vec2i operator-(const Vec2i &rhs) const;
	Vec2i& operator-=(const Vec2i &rhs);

	Vec2i operator*(int rhs) const;
	Vec2i& operator*=(int rhs);

	Vec2i operator/(float rhs) const;
	Vec2i& operator/=(float rhs);

	float GetMagnitude() const;

	static Vec2i Lerp(const Vec2i& v1, const Vec2i& v2, float t);
	static float AngleBetween(const Vec2i& v1, const Vec2i& v2);
	static float Dot(const Vec2i& v1, const Vec2i& v2);
};

class Vec2f final
{
public:
	float x;
	float y;

	Vec2f();
	Vec2f(float x, float y);
	explicit Vec2f(Vec2i v);

	~Vec2f() = default;

	bool operator==(const Vec2f &rhs) const;
	bool operator!=(const Vec2f &rhs) const;
	bool operator==(const Vec2i &rhs) const;
	bool operator!=(const Vec2i &rhs) const;

	Vec2f operator+(const Vec2f &rhs) const;
	Vec2f& operator+=(const Vec2f &rhs);
	Vec2f operator+(const Vec2i &rhs) const;
	Vec2f& operator+=(const Vec2i &rhs);

	Vec2f operator-(const Vec2f &rhs) const;
	Vec2f& operator-=(const Vec2f &rhs);
	Vec2f operator-(const Vec2i &rhs) const;
	Vec2f& operator-=(const Vec2i &rhs);

	Vec2f operator*(int rhs) const;
	Vec2f& operator*=(int rhs);
	Vec2f operator*(float rhs) const;
	Vec2f& operator*=(float rhs);

	Vec2f operator/(float rhs) const;
	Vec2f& operator/=(float rhs);

	float GetMagnitude() const;
	Vec2f Normalized() const;

	static Vec2f Lerp(const Vec2f& v1, const Vec2f& v2, float t);
	static float AngleBetween(const Vec2f& v1, const Vec2f& v2);
	static float Dot(const Vec2f& v1, const Vec2f& v2);
};

class Vec3f final
{
public:
	float x;
	float y;
	float z;

	Vec3f();
	Vec3f(float x, float y, float z);
	explicit Vec3f(Vec2i v);
	explicit Vec3f(Vec2f v);

	~Vec3f() = default;

	bool operator==(const Vec3f &rhs) const;
	bool operator!=(const Vec3f &rhs) const;

	Vec3f operator+(const Vec3f &rhs) const;
	Vec3f& operator+=(const Vec3f &rhs);

	Vec3f operator-(const Vec3f &rhs) const;
	Vec3f& operator-=(const Vec3f &rhs);

	Vec3f operator*(int rhs) const;
	Vec3f& operator*=(int rhs);
	Vec3f operator*(float rhs) const;
	Vec3f& operator*=(float rhs);

	Vec3f operator*(Vec3f other) const;
	Vec3f& operator*=(Vec3f other);

	Vec3f operator/(float rhs) const;
	Vec3f& operator/=(float rhs);

	float GetMagnitude() const;
	Vec3f Normalized() const;

	Vec3f Negate() const;

	static Vec3f Lerp(const Vec3f& v1, const Vec3f& v2, float t);
	static float AngleBetween(const Vec3f& v1, const Vec3f& v2);
	static float Dot(const Vec3f& v1, const Vec3f& v2);
	static Vec3f Cross(const Vec3f& v1, const Vec3f& v2);

	const float &operator[](const uint32_t &index) const
	{
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw std::runtime_error("Vector3 index out of bounds!");
		}
	}

	float &operator[](const uint32_t &index)
	{
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw std::runtime_error("Vector3 index out of bounds!");
		}
	}

	static const Vec3f Zero;
	static const Vec3f One;
	static const Vec3f Left;
	static const Vec3f Right;
	static const Vec3f Up;
	static const Vec3f Down;
	static const Vec3f Front;
	static const Vec3f Back;
};

inline auto operator*(const float &left, const Vec3f &right)
{
	return right * left;
}

class Vec4f final
{
public:
	float x;
	float y;
	float z;
	float w;

	Vec4f();
	Vec4f(float x, float y, float z, float w);

	~Vec4f() = default;

	const float& operator[](const uint32_t& index) const;

	float& operator[](const uint32_t& index);

	//bool operator==(const Vec4f &rhs) const;
	//bool operator!=(const Vec4f &rhs) const;

	//Vec4f operator+(const Vec4f &rhs) const;
	//Vec4f& operator+=(const Vec4f &rhs);

	//Vec4f operator-(const Vec4f &rhs) const;
	//Vec4f& operator-=(const Vec4f &rhs);

	//Vec4f operator*(int rhs) const;
	//Vec4f& operator*=(int rhs);
	//Vec4f operator*(float rhs) const;
	//Vec4f& operator*=(float rhs);

	//Vec4f operator/(float rhs) const;
	//Vec4f& operator/=(float rhs);

	//float GetMagnitude() const;
	//Vec4f Normalized() const;

	//static Vec4f Lerp(const Vec4f& v1, const Vec4f& v2, float t);
	//static float AngleBetween(const Vec4f& v1, const Vec4f& v2);
	//static float Dot(const Vec4f& v1, const Vec4f& v2);
};
}

namespace std
{
	template<typename T>
	static void HashCombine(std::size_t &seed, const T &v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<>
	struct hash<dm::Vec3f>
	{
		size_t operator()(dm::Vec3f const &vector) const noexcept
		{
			size_t seed = 0;
			HashCombine(seed, vector.x);
			HashCombine(seed, vector.y);
			HashCombine(seed, vector.z);
			return seed;
		}
	};

	template<>
	struct hash<dm::Vec2f>
	{
		size_t operator()(dm::Vec2f const &vector) const noexcept
		{
			size_t seed = 0;
			HashCombine(seed, vector.x);
			HashCombine(seed, vector.y);
			return seed;
		}
	};
}
#endif VECTOR_H