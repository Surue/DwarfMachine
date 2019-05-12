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
#include <graphics/model_manager.h>

namespace dm
{
SystemContainer::SystemContainer()
{
}

void SystemContainer::Init()
{
	m_GraphicManager = std::make_unique<GraphicManager>();
	m_InputManager = std::make_unique<InputManager>();
	m_EntityManager = std::make_unique<EntityManager>();
	m_ComponentManager = std::make_unique<ComponentManagerContainer>();
	m_SystemManager = std::make_unique<SystemManager>();
	m_ModelManager = std::make_unique<ModelManager>();

	m_GraphicManager->Init();
	m_InputManager->Init();
	m_EntityManager->Init();
	m_ComponentManager->Init();
	m_SystemManager->Init();
	m_ModelManager->Init();
}

void SystemContainer::Update() 
{
	m_GraphicManager->Update();
	m_InputManager->Update();
	m_SystemManager->Update();
}

void SystemContainer::Draw()
{
	m_GraphicManager->Draw();
}

GraphicManager* SystemContainer::GetGraphicManager() const
{
	return m_GraphicManager.get();
}

InputManager* SystemContainer::GetInputManager() const
{
	return m_InputManager.get();
}

EntityManager* SystemContainer::GetEntityManager() const
{
	return m_EntityManager.get();
}

ComponentManagerContainer* SystemContainer::GetComponentManager() const
{
	return m_ComponentManager.get();
}

SystemManager* SystemContainer::GetSystemManager() const
{
	return m_SystemManager.get();
}

ModelManager* SystemContainer::GetModelManager() const
{
	return m_ModelManager.get();
}
}
