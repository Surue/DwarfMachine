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

#include <engine/system_container.h>

#include <graphics/graphic_manager.h>
#include <engine/Input.h>
#include <component/component_manager.h>
#include <system/system_manager.h>
#include <graphics/mesh_manager.h>
#include <physic/physic_manager.h>

namespace dm
{
SystemContainer::SystemContainer()
{
}

SystemContainer::~SystemContainer()
{
	delete(m_GraphicManager);
	delete(m_InputManager);
	delete(m_EntityManager);
	delete(m_ComponentManager);
	delete(m_SystemManager);
	delete(m_ModelManager);
	delete(m_PhysicManager);
}

void SystemContainer::Init()
{
	m_GraphicManager = new GraphicManager();
	m_InputManager = new InputManager();
	m_EntityManager = new EntityManager();
	m_ComponentManager = new ComponentManagerContainer();
	m_SystemManager = new SystemManager();
	m_ModelManager = new MeshManager();
	m_PhysicManager = new PhysicManager();

	m_GraphicManager->Init();
	m_InputManager->Init();
	m_EntityManager->Init();
	m_ComponentManager->Init();
	m_SystemManager->Init();
	m_ModelManager->Init();
	m_PhysicManager->Init();
}

void SystemContainer::Update() 
{
	m_GraphicManager->Update();
	m_InputManager->Update();
	m_SystemManager->Update();
	m_PhysicManager->Update();
}

void SystemContainer::Draw()
{
	m_GraphicManager->Draw();
}

GraphicManager* SystemContainer::GetGraphicManager() const
{
	return m_GraphicManager;
}

InputManager* SystemContainer::GetInputManager() const
{
	return m_InputManager;
}

EntityManager* SystemContainer::GetEntityManager() const
{
	return m_EntityManager;
}

ComponentManagerContainer* SystemContainer::GetComponentManager() const
{
	return m_ComponentManager;
}

SystemManager* SystemContainer::GetSystemManager() const
{
	return m_SystemManager;
}

MeshManager* SystemContainer::GetModelManager() const
{
	return m_ModelManager;
}
}
