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

#include <graphics/model_manager.h>

#include <utility/xxhash.hpp>
#include <graphics/model_cube.h>
#include <graphics/model_sphere.h>
#include <graphics/model_obj.h>
#include <graphics/model_plane.h>

namespace dm
{
ModelManager::ModelManager()
{
	m_RegisteredModels["ModelCube"] = ModelCube::Create();
	m_RegisteredModels["ModelSphere"] = ModelSphere::Create();
	m_RegisteredModels["ModelPlane"] = ModelPlane::Create();
	m_RegisteredModels["ressources/models/chalet.obj"] = ModelObj::Create("ressources/models/chalet.obj");
}

Model* ModelManager::GetModel(const std::string name)
{
	const auto it = m_RegisteredModels.find(name);

	if (it == m_RegisteredModels.end())
	{
		return nullptr;
	}

	return it->second.get();
}
}
