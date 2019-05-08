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

#ifndef ENTITY_H
#define ENTITY_H
#include <vector>

#include <engine/engine.h>
#include <component/component_type.h>
#include "component/component_mask.h"
#define INIT_ENTITY_NMB 10

namespace dm
{
using Entity = unsigned int ;
const Entity INVALID_ENTITY = 0U;

class EntityManager final
{
public:
	EntityManager();

	~EntityManager() = default;

	EntityManager& operator=(const EntityManager&) = delete;
	EntityManager(EntityManager &&) = default; //move constructor
	EntityManager(const EntityManager &) = delete; //delete copy constructor

	Entity CreateEntity();
	void DestroyEntity(Entity entity);

	void AddComponent(Entity entity, ComponentType componentType);
	void DestroyComponent(Entity entity, ComponentType componentType);

	bool HasComponent(Entity entity, ComponentType componentType);

	void ResizeEntity(size_t newSize);

	ComponentMask GetEntityMask(Entity entity);

private:
	void ResizeEntity();

	unsigned int m_LastEntity = 0;
	std::vector<Entity> m_EntityInfos;
	std::vector<ComponentMask> m_EntityMask;
};
}

#endif ENTITY_H