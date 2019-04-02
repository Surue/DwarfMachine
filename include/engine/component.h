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

#ifndef COMPONENT_H
#define COMPONENT_H
#include <engine/entity.h>

#define INIT_COMPONENT_NMB 10

namespace dm
{
enum class ComponentType : int {
	TRANSFORM = 1 << 0
};

template<typename T>
class ComponentBaseManager
{
public:
	ComponentBaseManager()
	{
		
	}

	virtual ~ComponentBaseManager()
	{
		m_Components.clear();
	};

	virtual void Init() = 0;
	 
	virtual void Update() = 0;

	virtual void AddComponent(Entity entity, T& componentBase) = 0;
	virtual void DestroyComponent(Entity entity) = 0;

	virtual T* GetComponent(Entity entity) = 0;

	std::vector<T>& GetComponents()
	{
		return m_Components;
	}

protected:
	std::vector<T> m_Components;
};
}

#endif COMPONENT_H