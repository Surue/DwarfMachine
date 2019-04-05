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

	Vec3f operator/(float rhs) const;
	Vec3f& operator/=(float rhs);

	float GetMagnitude() const;
	Vec3f Normalized() const;

	static Vec3f Lerp(const Vec3f& v1, const Vec3f& v2, float t);
	static float AngleBetween(const Vec3f& v1, const Vec3f& v2);
	static float Dot(const Vec3f& v1, const Vec3f& v2);
	static Vec3f Cross(const Vec3f& v1, const Vec3f& v2);
};
}
#endif VECTOR_H