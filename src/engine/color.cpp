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

#include <engine/color.h>
#include <stdexcept>

namespace dm
{
const Color Color::White = Color(1.0f, 1.0f, 1.0f);

Color::Color(const float red, const float green, const float blue, const float alpha):
	r(red),
	g(green),
	b(blue),
	a(alpha) {}

bool Color::operator==(const Color& other) const
{
	return r == other.r && g == other.g && b == other.b && a == other.a;
}

bool Color::operator!=(const Color& other) const
{
	return !(*this == other);
}

float &Color::operator[](const uint32_t index)
{
	switch(index)
	{
	case 0:
		return r;
	case 1:
		return g;
	case 2:
		return b;
	case 3:
		return a;
	default:
		throw std::runtime_error("Color is out of bound");
	}
}
}
