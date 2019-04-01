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

#include <engine/entity.h>
#include <iostream>
#include <string>

namespace dm
{
EntityManager::EntityManager(Engine& engine) : m_Engine(engine)
{
	std::cout << "Entity Manager build\n";
	m_Entities.resize(INIT_ENTITY_NMB, INVALID_ENTITY);
}

Entity EntityManager::CreateEntity()
{
	Entity newEntity = INVALID_ENTITY;

	//Add a new entity if vector not full
	if(m_LastEntity < m_Entities.size())
	{
		m_Entities[m_LastEntity] = m_LastEntity + 1;
		newEntity = m_Entities[m_LastEntity];

		m_LastEntity++;
		if (m_LastEntity == m_Entities.size()) //Resize vector if it's full
		{
			std::cout << "resize after adding new entity\n";
			m_Entities.resize(m_Entities.size() + INIT_ENTITY_NMB, INVALID_ENTITY);
		}
		else if (m_Entities[m_LastEntity] != INVALID_ENTITY) //Find next free index
		{
			std::cout << "move last entity\n";
			for(int i = m_LastEntity; i < m_Entities.size(); i++)
			{
				if(m_Entities[i] == INVALID_ENTITY)
				{
					m_LastEntity = i;
					break;
				}
			}
		}
	}
	else //Resize the vector
	{
		std::cout << "resize before adding new entity\n";

		m_Entities.resize(m_Entities.size() + INIT_ENTITY_NMB, INVALID_ENTITY);

		m_Entities[m_LastEntity] = m_LastEntity + 1;
		newEntity = m_Entities[m_LastEntity];

		m_LastEntity++;
	}

	return newEntity;
}

void EntityManager::DestroyEntity(const Entity entity)
{
	if(entity <= 0 || entity >= m_Entities.size())
	{
		throw std::runtime_error("Try to remove non existing entity: " + std::to_string(entity));
	}

	//Remove entity
	std::cout << "delete entity\n";
	m_Entities[entity - 1] = INVALID_ENTITY;
	m_LastEntity = entity - 1;

	//Update all components manager
	//TODO remove all components

	//Update all systems
	//TODO update systems
	
}
}
