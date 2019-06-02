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

namespace dm
{
class GraphicManager;
class InputManager;
class EntityManager;
class ComponentManagerContainer;
class SystemManager;
class MeshManager;
class EngineApplication;
class PhysicManager;
class PipelineMaterialManager;

class ModuleContainer
{
public:
	ModuleContainer();
	~ModuleContainer();

	void Init();

	void Update();

	void Draw();

	void Clear();

	GraphicManager* GetGraphicManager() const;

	InputManager* GetInputManager() const;

	EntityManager* GetEntityManager() const;

	ComponentManagerContainer* GetComponentManager() const;

	SystemManager* GetSystemManager() const;

	MeshManager* GetModelManager() const;

	PipelineMaterialManager* GetPipelineMaterialManager() const;
private:
	GraphicManager* m_GraphicManager;
	InputManager* m_InputManager;
	EntityManager* m_EntityManager;
	ComponentManagerContainer* m_ComponentManager;
	SystemManager* m_SystemManager;
	MeshManager* m_ModelManager;
	PhysicManager* m_PhysicManager;
	PipelineMaterialManager* m_PipelineMaterialManager;
};
}

#endif SYSTEM_CONTAINER_H