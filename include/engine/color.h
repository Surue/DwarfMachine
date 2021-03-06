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

#ifndef COLOR_H
#define COLOR_H
#include <cstdint>

namespace dm
{
class Color
{
public:
	Color() = default;

	/**
	 * \brief 
	 * \param red 
	 * \param green 
	 * \param blue 
	 * \param alpha 
	 */
	explicit Color(float red, float green, float blue, float alpha = 1.0f);

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;

	bool operator==(const Color &other) const;

	bool operator!=(const Color &other) const;

	Color operator*(const float rhs) const
	{
		return Color(r * rhs, g * rhs, b * rhs, a);
	}

	float &operator[](const uint32_t index);

	static const Color White;

};
}

#endif COLOR_H