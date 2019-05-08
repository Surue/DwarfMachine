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

#include <entity/entity.h>
#include <component/component_manager.h>
#include <component/component_type.h>
#include <system/system_manager.h>
#include "graphics/renderer_container.h"

namespace dm
{

class EntityHandle
{
public:
	EntityHandle(Entity entity);

	template<class T>
	T* AddComponent(T& component)
	{
		const auto oldMask = m_EntityManager->GetEntityMask(m_Entity);
		auto result = static_cast<T*>(m_ComponentManager->AddComponent(m_Entity, component));
		m_EntityManager->AddComponent(m_Entity, component.componentType);
		m_SystemManager->AddComponent(m_Entity, oldMask, m_EntityManager->GetEntityMask(m_Entity));
		if(m_RendererContainer != nullptr)
		m_RendererContainer->AddComponent(m_Entity, oldMask, m_EntityManager->GetEntityMask(m_Entity));
		return result;
	}

	template<class T>
	T* CreateComponent(const ComponentType componentType) const
	{
		const auto oldMask = m_EntityManager->GetEntityMask(m_Entity);

		m_EntityManager->AddComponent(m_Entity, componentType);
		m_SystemManager->AddComponent(m_Entity, oldMask, m_EntityManager->GetEntityMask(m_Entity));
		if (m_RendererContainer != nullptr)
		m_RendererContainer->AddComponent(m_Entity, oldMask, m_EntityManager->GetEntityMask(m_Entity));
		return static_cast<T*>(m_ComponentManager->CreateComponent(m_Entity, componentType));
	}

	template<class T>
	T* GetComponent(const ComponentType componentType)
	{
		return static_cast<T*>(m_ComponentManager->GetComponent(m_Entity, componentType));
	}

	bool HasComponent(ComponentType componentType) const;

	void DestroyComponent(ComponentType componentType) const;

	void Destroy();


private:
	Entity m_Entity;
	ComponentManagerContainer* m_ComponentManager = nullptr;
	EntityManager* m_EntityManager = nullptr;
	SystemManager* m_SystemManager = nullptr;
	RendererContainer* m_RendererContainer = nullptr;
};
}

#endif ENTITY_HANDLE_H