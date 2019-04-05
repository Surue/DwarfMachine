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

#ifndef ENTITY_HANDLE_H
#define ENTITY_HANDLE_H

#include <engine/entity.h>
#include <engine/component_manager.h>
#include <engine/component_type.h>
#include <map>

namespace dm
{
class ComponentManager;

class EntityHandle
{
public:
	EntityHandle(Entity entity, Engine& engine);

	template<class T>
	T* AddComponent(T& component)
	{
		m_EntityManager->AddComponent(m_Entity, component.componentType);
		//TODO Update system
		return static_cast<T*>(m_ComponentManager->AddComponent(m_Entity, component));
	}

	template<class T>
	T* CreateComponent(const ComponentType componentType) const
	{
		m_EntityManager->AddComponent(m_Entity, componentType);
		//TODO Update system
		return static_cast<T*>(m_ComponentManager->CreateComponent(m_Entity, componentType));
	}

	template<class T>
	T* GetComponent()
	{
		return static_cast<T*>(m_ComponentManager->GetComponent<T>(m_Entity));
	}

	bool HasComponent(const ComponentType componentType) const;

	void DestroyComponent(const ComponentType componentType) const;

	void Destroy();


private:
	Entity m_Entity;
	ComponentManager* m_ComponentManager = nullptr;
	EntityManager* m_EntityManager = nullptr;
	Engine& m_Engine;
};
}

#endif ENTITY_HANDLE_H