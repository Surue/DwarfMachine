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

#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <component/transform.h>
#include <component/camera.h>
#include <component/control_type.h>

namespace dm {

class Engine;

class ComponentManager final
{
public:
	ComponentManager(Engine& engine);

	ComponentBase* CreateComponent(Entity entity, ComponentType componentType);

	ComponentBase* AddComponent(Entity entity, ComponentBase& component);


	ComponentBase* GetComponent(const Entity entity, const ComponentType componentType);

	void DestroyComponent(const Entity entity, ComponentType componentType);

private:
	TransformManager m_TransformManager;
	CameraManager m_CameraManager;
	ControllerTypeManager m_ControllerTypeManager;

	Engine& m_Engine;
};
}

#endif COMPONENT_MANAGER_H