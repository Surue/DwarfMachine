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
#include <entity/entity.h>
#include <component/component_type.h>
#include <engine/metadata.h>
#include "utility/json_utility.h"

#define INIT_COMPONENT_NMB 100

namespace dm
{

struct ComponentBase
{
	ComponentType componentType = ComponentType::NONE;
	//TODO corriger cette connerie qui augmente la taille de tout les components
};

template<typename T>
class ComponentBaseManager : public Metadata
{
public:
	ComponentBaseManager() {}

	virtual ~ComponentBaseManager()
	{
		m_Components.clear();
	};

	virtual void Init() = 0;
	 
	virtual void Update() = 0;

	void Clear()
	{
		m_Components.clear();
		m_Components.resize(INIT_COMPONENT_NMB);
	}

	virtual T* CreateComponent(Entity entity) = 0;
	
	virtual T* AddComponent(const Entity entity, T& component)
	{
		m_Components[entity - 1] = std::move(component);
		return &m_Components[entity - 1];
	};

	virtual void DestroyComponent(Entity entity) = 0;

	T* GetComponent(const Entity entity)
	{
		return &m_Components[entity - 1];
	};

	std::vector<T>& GetComponents()
	{
		return m_Components;
	}

	virtual void OnDrawInspector(Entity entity) = 0;

	virtual void OnEntityResize(int newSize)
	{
		m_Components.resize(newSize);
	}
protected:
	std::vector<T> m_Components{INIT_COMPONENT_NMB};
};
}

#endif COMPONENT_H