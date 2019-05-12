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

#ifndef SYSTEM_CONTAINER_H
#define SYSTEM_CONTAINER_H
#include <memory>

namespace dm
{
class GraphicManager;
class InputManager;
class EntityManager;
class ComponentManagerContainer;
class SystemManager;
class MeshManager;
class EngineApplication;

class SystemContainer
{
public:
	SystemContainer();

	void Init();

	void Update();

	void Draw();

	GraphicManager* GetGraphicManager() const;

	InputManager* GetInputManager() const;

	EntityManager* GetEntityManager() const;

	ComponentManagerContainer* GetComponentManager() const;

	SystemManager* GetSystemManager() const;

	MeshManager* GetModelManager() const;
private:
	std::unique_ptr<GraphicManager> m_GraphicManager;
	std::unique_ptr<InputManager> m_InputManager;
	std::unique_ptr<EntityManager> m_EntityManager;
	std::unique_ptr<ComponentManagerContainer> m_ComponentManager;
	std::unique_ptr<SystemManager> m_SystemManager;
	std::unique_ptr<MeshManager> m_ModelManager;
};
}

#endif SYSTEM_CONTAINER_H