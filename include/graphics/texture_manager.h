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

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <graphics/image_2d.h>
#include <map>

namespace dm
{
class TextureManager
{
public:
	static TextureManager* Get();

	TextureManager()
	{
	}

	std::shared_ptr<Image2d> GetTextureByName(const std::string& name)
	{
		auto it = m_Textures.find(name);

		if(it != m_Textures.end())
		{
			return it->second;
		}

		m_Textures.emplace(name, Image2d::Create(name));
		auto it3 = m_Textures.find(name);

		if (it3 != m_Textures.end())
		{
			return it3->second;
		}
	}
private:
	std::map<std::string, std::shared_ptr<Image2d>> m_Textures;
};
}

#endif