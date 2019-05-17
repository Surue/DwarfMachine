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

#include <engine/Input.h>
#include <entity/entity.h>
#include <component/component_manager.h>
#include <system/system_manager.h>
#include <graphics/window.h>
#include <graphics/graphic_manager.h>

#include <engine/engine_application.h>

namespace dm
{
Engine *Engine::m_Instance = nullptr;

Engine::Engine(const EngineSettings engineSettings) :
	m_Settings(engineSettings),
	m_App(nullptr)
{
	m_Instance = this;

	m_PreviousFrameTime = m_Timer.now();
	m_CurrentFrame = m_Timer.now();
}

Engine::~Engine() {}

void Engine::Init()
{
	m_SystemContainer.Init();
}

void Engine::Start()
{
	MainLoop();
	Destroy();
}

void Engine::Stop() const
{
	GetGraphicManager()->GetWindow()->SetShouldClose();
}

float Engine::GetDeltaTime() const { return m_DeltaTime; }

GraphicManager* Engine::GetGraphicManager() const
{
	return m_SystemContainer.GetGraphicManager();
}

InputManager* Engine::GetInputManager() const
{
	return m_SystemContainer.GetInputManager();
}

EntityManager* Engine::GetEntityManager() const
{
	return m_SystemContainer.GetEntityManager();
}

ComponentManagerContainer* Engine::GetComponentManager() const
{
	return m_SystemContainer.GetComponentManager();
}

SystemManager* Engine::GetSystemManager() const
{
	return m_SystemContainer.GetSystemManager();
}

EngineSettings& Engine::GetSettings()
{
	return m_Settings;
}

MeshManager* Engine::GetModelManager() const
{
	return m_SystemContainer.GetModelManager();
}

PipelineMaterialManager* Engine::GetPipelineMaterialManager() const
{
	return m_SystemContainer.GetPipelineMaterialManager();
}

void Engine::SetApplication(EngineApplication* app)
{
	m_App.reset(app);
}

void Engine::MainLoop()
{
	while (!GetGraphicManager()->GetWindow()->ShouldClose())
	{
		CalculateDeltaTime();

		m_SystemContainer.Update();
		m_App->Update();

		m_App->Draw();
		m_SystemContainer.Draw();
	}

}

void Engine::Destroy()
{
	
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
