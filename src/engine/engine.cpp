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
#include <engine/engine.h>

#include <graphics/graphic_manager.h>
#include <engine/Input.h>
#include <engine/entity.h>
#include <engine/transform.h>
#include <engine/component_manager.h>

namespace dm
{
void Engine::Init()
{
	m_GraphicManager = new GraphicManager(*this);
	m_InputManager = new InputManager(*this);
	m_EntityManager = new EntityManager(*this);
	m_ComponentManager = new ComponentManager();

	m_GraphicManager->Init();
	m_Window = m_GraphicManager->GetWindow();

	m_InputManager->Init(m_Window);
}

void Engine::Start()
{
	MainLoop();
	Destroy();
}

void Engine::Stop() const
{
	glfwSetWindowShouldClose(m_Window, true);
}

InputManager* Engine::GetInputManager() const
{
	return m_InputManager;
}

EntityManager* Engine::GetEntityManager() const
{
	return m_EntityManager;
}

ComponentManager* Engine::GetComponentManager() const
{
	return m_ComponentManager;
}

void Engine::MainLoop()
{
	while (!glfwWindowShouldClose(m_Window))
	{
		glfwPollEvents();
		m_InputManager->Update();

		//Updates
		m_GraphicManager->Update();
	}
}

void Engine::Destroy()
{
	m_GraphicManager->Destroy();

	delete(m_GraphicManager);
	delete(m_InputManager);
	delete(m_ComponentManager);
	delete(m_EntityManager);
}

}
