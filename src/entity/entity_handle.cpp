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

#include <entity/entity_handle.h>
#include <graphics/graphic_manager.h>
#include <engine/engine.h>

namespace dm
{
EntityHandle::EntityHandle(const Entity entity)
{
	m_Entity = entity;

	m_ComponentManager = Engine::Get()->GetComponentManager();
	m_EntityManager = Engine::Get()->GetEntityManager();
	m_SystemManager = Engine::Get()->GetSystemManager();
	m_RendererContainer = GraphicManager::Get()->GetRendererContainer();
}

bool EntityHandle::HasComponent(const ComponentType componentType) const
{
	return m_EntityManager->HasComponent(m_Entity, componentType);
}

void EntityHandle::DestroyComponent(const ComponentType componentType) const
{
	const auto oldMask = m_EntityManager->GetEntityMask(m_Entity);

	m_EntityManager->DestroyComponent(m_Entity, componentType);
	m_SystemManager->DestroyComponent(m_Entity, oldMask, m_EntityManager->GetEntityMask(m_Entity));
	if (m_RendererContainer != nullptr)
		m_RendererContainer->DestroyComponent(m_Entity, oldMask, m_EntityManager->GetEntityMask(m_Entity));

	m_ComponentManager->DestroyComponent(m_Entity, componentType);
}

void EntityHandle::Destroy()
{
	const auto oldMask = m_EntityManager->GetEntityMask(m_Entity);

	m_EntityManager->DestroyEntity(m_Entity);
	m_SystemManager->DestroyComponent(m_Entity, oldMask, m_EntityManager->GetEntityMask(m_Entity));
}
}
