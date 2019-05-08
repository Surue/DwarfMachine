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
#include <entity/entity.h>
#include <component/component_manager.h>
#include <system/system_manager.h>
#include <graphics/window.h>

#include <engine/engine_application.h>

namespace dm
{
Engine *Engine::m_Instance = nullptr;

Engine::Engine() :
	m_App(nullptr)
{
	m_Instance = this;

	m_Settings = EngineSettings();
	m_Settings.windowSize = Vec2i(800, 600);

	m_GraphicManager = std::make_unique<GraphicManager>();
	m_InputManager = std::make_unique<InputManager>();
	m_EntityManager = std::make_unique<EntityManager>();
	m_ComponentManager = std::make_unique<ComponentManagerContainer>();
	m_SystemManager = std::make_unique<SystemManager>();

	m_PreviousFrameTime = m_Timer.now();
	m_CurrentFrame = m_Timer.now();
}

Engine::Engine(const EngineSettings engineSettings) :
	m_App(nullptr)
{
	m_Settings = engineSettings;

	m_GraphicManager = std::make_unique<GraphicManager>();
	m_InputManager = std::make_unique<InputManager>();
	m_EntityManager = std::make_unique<EntityManager>();
	m_ComponentManager = std::make_unique<ComponentManagerContainer>();
	m_SystemManager = std::make_unique<SystemManager>();

	m_PreviousFrameTime = m_Timer.now();
	m_CurrentFrame = m_Timer.now();
}

Engine::~Engine() {}

void Engine::Init()
{
	m_GraphicManager->Awake();
	m_Window = m_GraphicManager->GetWindow();

	m_InputManager->Init(m_Window->GetWindow());
}

void Engine::Start()
{
	MainLoop();
	Destroy();
}

void Engine::Stop() const
{
	m_Window->SetShouldClose();
}

GraphicManager* Engine::GetGraphicManager() const
{
	return m_GraphicManager.get();
}

InputManager* Engine::GetInputManager() const
{
	return m_InputManager.get();
}

EntityManager* Engine::GetEntityManager() const
{
	return m_EntityManager.get();
}

ComponentManagerContainer* Engine::GetComponentManager() const
{
	return m_ComponentManager.get();
}

SystemManager* Engine::GetSystemManager() const
{
	return m_SystemManager.get();
}

EngineSettings& Engine::GetSettings()
{
	return m_Settings;
}

void Engine::SetApplication(EngineApplication* app)
{
	m_App.reset(app);
}

void Engine::MainLoop()
{
	while (!m_Window->ShouldClose())
	{
		CalculateDeltaTime();

		std::cout << "dt = " << m_DeltaTime << ", FPS = " << m_Fps << "\n";

		m_InputManager->Update();

		//Updates
		m_SystemManager->Update(m_DeltaTime);
		m_GraphicManager->Update(m_DeltaTime);
		m_App->Update(m_DeltaTime);

		m_App->Draw();
		m_GraphicManager->Draw();
	}

}

void Engine::Destroy()
{
	m_GraphicManager->SetManager(nullptr);
	m_SystemManager->Destroy();
}

void Engine::CalculateDeltaTime()
{
	m_CurrentFrame = m_Timer.now();
	const auto dt = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(m_CurrentFrame - m_PreviousFrameTime).count() / 1000.0f;
	m_PreviousFrameTime = m_CurrentFrame;

	m_Fps = 1.0f / dt;
	m_DeltaTime = dt;
}
}
