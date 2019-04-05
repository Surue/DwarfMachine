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

#include <engine/transform.h>
#include <graphics/camera.h>

namespace dm {

	class ComponentManager final
	{
	public:
		ComponentManager()
		{
			m_TransformManager = TransformManager();
			m_CameraManager = CameraManager();
		}

		ComponentBase* CreateComponent(Entity entity, ComponentType componentType);

		ComponentBase* AddComponent(Entity entity, ComponentBase& component);

		template<class T>
		T* GetComponent(const Entity entity)
		{
			if(typeid(T).raw_name() == typeid(Transform).raw_name())
			{
				return m_TransformManager.GetComponent(entity);
			}

			throw std::runtime_error("Fail to bind component to its own component manager");
		}

		void DestroyComponent(const Entity entity, ComponentType componentType);

	private:
		TransformManager m_TransformManager;
		CameraManager m_CameraManager;
	};
}

#endif COMPONENT_MANAGER_H