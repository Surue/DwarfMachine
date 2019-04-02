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

namespace dm {

	class ComponentManager
	{
	public:
		ComponentManager()
		{
			m_TransformManager = new TransformManager();
		}

		template<class T>
		void AddComponent(Entity entity, T& component)
		{
			if (typeid(T).raw_name() == typeid(Transform).raw_name())
			{
				m_TransformManager->AddComponent(entity, component);
			}
			else {
				std::runtime_error("Fail to bind component to its own component manager");
			}
		}

		template<class T>
		T* GetComponent(const Entity entity)
		{
			if(typeid(T).raw_name() == typeid(Transform).raw_name())
			{
				return m_TransformManager->GetComponent(entity);
			}

			std::runtime_error("Fail to bind component to its own component manager");
		}

		void DestroyComponent(ComponentType componentType);

	private:
		TransformManager* m_TransformManager;
	};
}

#endif COMPONENT_MANAGER_H