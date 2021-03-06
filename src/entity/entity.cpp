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

#include <entity/entity.h>
#include <string>
#include <component/component_manager.h>
#include <system/system_manager.h>
#include "engine/engine.h"
#include <editor/editor.h>

namespace dm
{
class Editor;

EntityManager::EntityManager()
{
	ComponentMask emptyMask;
	emptyMask.mask = static_cast<int>(ComponentType::NONE);

	m_EntityInfos.resize(INIT_ENTITY_NMB, INVALID_ENTITY);
	m_EntityMask.resize(INIT_ENTITY_NMB, emptyMask);
}

void EntityManager::Init()
{
}

void EntityManager::Update()
{
}

void EntityManager::Clear()
{
	m_EntityInfos.resize(INIT_ENTITY_NMB);
	m_EntityMask.resize(INIT_ENTITY_NMB);
}

void EntityManager::Draw()
{
}

Entity EntityManager::CreateEntity()
{
	auto newEntity = INVALID_ENTITY;

	//Add a new entity if vector not full
	if(m_LastEntity < m_EntityInfos.size())
	{
		m_EntityInfos[m_LastEntity] = m_LastEntity + 1;
		newEntity = m_EntityInfos[m_LastEntity];

		m_LastEntity++;
		if (m_LastEntity == m_EntityInfos.size()) //Resize vector if it's full
		{
			ResizeEntity();
		}
		else if (m_EntityInfos[m_LastEntity] != INVALID_ENTITY) //Find next free index
		{
			for(int i = m_LastEntity; i < m_EntityInfos.size(); i++)
			{
				if(m_EntityInfos[i] == INVALID_ENTITY)
				{
					m_LastEntity = i;
					break;
				}
			}
		}
	}
	else //Resize the vector
	{
		ResizeEntity();

		m_EntityInfos[m_LastEntity] = m_LastEntity + 1;
		newEntity = m_EntityInfos[m_LastEntity];

		m_LastEntity++;
	}

	return newEntity;
}

void EntityManager::DestroyEntity(const Entity entity)
{
	if(entity <= 0 || entity >= m_EntityMask.size())
	{
		throw std::runtime_error("Try to remove non existing entity: " + std::to_string(entity));
	}

	//Remove entity
	m_EntityMask[entity - 1].mask = static_cast<int>(ComponentType::NONE);
	m_EntityInfos[entity - 1] = INVALID_ENTITY;
	m_LastEntity = entity - 1;
}

void EntityManager::AddComponent(const Entity entity, const ComponentType componentType)
{
	m_EntityMask[entity - 1].AddComponent(componentType);
}

void EntityManager::DestroyComponent(const Entity entity, const ComponentType componentType)
{
	m_EntityMask[entity - 1].RemoveComponent(componentType);
}

bool EntityManager::HasComponent(const Entity entity, const ComponentType componentType)
{
	ComponentMask tmpComponentMask;
	tmpComponentMask.AddComponent(componentType);
	return m_EntityMask[entity - 1].Matches(tmpComponentMask);
}

void EntityManager::ResizeEntity(const size_t newSize)
{
	m_EntityMask.resize(newSize);
	m_EntityInfos.resize(newSize, static_cast<int>(ComponentType::NONE));

	Engine::Get()->GetComponentManager()->OnEntityResize(newSize);
	const auto editor = reinterpret_cast<Editor*>(Engine::Get()->GetApplication());
	editor->GetGizmoManager()->OnEntityResize();
}

ComponentMask EntityManager::GetEntityMask(const Entity entity)
{
	return m_EntityMask[entity - 1];
}

void EntityManager::ResizeEntity()
{
	ComponentMask emptyMask;
	emptyMask.mask = static_cast<int>(ComponentType::NONE);

	m_EntityMask.resize(m_EntityMask.size() + INIT_ENTITY_NMB, emptyMask);
	m_EntityInfos.resize(m_EntityInfos.size() + INIT_ENTITY_NMB, static_cast<int>(ComponentType::NONE));

	Engine::Get()->GetComponentManager()->OnEntityResize(INIT_ENTITY_NMB + m_EntityMask.size());
	const auto editor = reinterpret_cast<Editor*>(Engine::Get()->GetApplication());
	editor->GetGizmoManager()->OnEntityResize();
}
}
