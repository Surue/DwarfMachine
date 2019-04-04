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

	class ComponentManager
	{
	public:
		ComponentManager()
		{
			m_TransformManager = TransformManager();
			m_CameraManager = CameraManager();
		}

		ComponentBase* CreateComponent(const Entity entity, const ComponentType componentType)
		{
			switch (componentType) {
			case ComponentType::NONE:
				throw std::runtime_error("Impossible to add a ComponentType::NONE to an entity");
				break;
			case ComponentType::TRANSFORM:
				return m_TransformManager.CreateComponent(entity);
				break;
			case ComponentType::CAMERA:
				return m_CameraManager.CreateComponent(entity);
				break;
			default:
				throw std::runtime_error("Fail to bind component to its own component manager");
				break;
			}
		}

		ComponentBase* AddComponent(const Entity entity, ComponentBase& component)
		{
			switch(component.componentType) { 
				case ComponentType::NONE: 
					throw std::runtime_error("Impossible to add a ComponentType::NONE to an entity");
				case ComponentType::TRANSFORM: 
					return static_cast<ComponentBase*>(m_TransformManager.AddComponent(entity, static_cast<Transform&>(component)));
				case ComponentType::CAMERA: 
					return static_cast<ComponentBase*>(m_CameraManager.AddComponent(entity, static_cast<Camera&>(component)));
				default: 
					throw std::runtime_error("Fail to bind component to its own component manager");
			}
		}

		template<class T>
		T* GetComponent(const Entity entity)
		{
			if(typeid(T).raw_name() == typeid(Transform).raw_name())
			{
				return m_TransformManager.GetComponent(entity);
			}

			std::runtime_error("Fail to bind component to its own component manager");
			return nullptr;
		}

		void DestroyComponent(ComponentType componentType);

	private:
		TransformManager m_TransformManager;
		CameraManager m_CameraManager;
	};
}

#endif COMPONENT_MANAGER_H