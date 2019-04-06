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

#ifndef ENGINE_H
#define ENGINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <engine/vector.h>

namespace dm
{
class GraphicManager;
class InputManager;
class EntityManager;
class ComponentManager;
class SystemManager;

struct EngineSettings
{
	Vec2i windowSize;
};

class Engine
{
public:
	Engine();
	Engine(EngineSettings engineSettings);

	void Init();

	void Start();

	void Stop() const;

	GraphicManager* GetGraphicManager() const;

	InputManager* GetInputManager() const;

	EntityManager* GetEntityManager() const;

	ComponentManager* GetComponentManager() const;

	SystemManager* GetSystemManager() const;

	EngineSettings& GetSettings();
private:
	/**
	 * \brief Main loop of the game
	 */
	void MainLoop();

	void Destroy();

	EngineSettings m_Settings;

	GLFWwindow* m_Window = nullptr;

	GraphicManager* m_GraphicManager = nullptr;
	InputManager* m_InputManager = nullptr;
	EntityManager* m_EntityManager = nullptr;
	ComponentManager* m_ComponentManager = nullptr;
	SystemManager* m_SystemManager = nullptr;
};
}

#endif ENGINE_H