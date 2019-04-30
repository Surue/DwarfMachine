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

#include <component/transform.h>

namespace dm
{
TransformManager::TransformManager(Engine& engine) : 
ComponentBaseManager<Transform>(engine)
{
	
}

void TransformManager::Init()
{
	
}

void TransformManager::Update()
{

}

Transform* TransformManager::CreateComponent(const Entity entity)
{
	auto t = Transform();
	t.componentType = ComponentType::TRANSFORM;

	t.position.x = 0;
	t.position.y = 0;
	t.position.z = 0;

	t.rotation.x = 0;
	t.rotation.y = 0;
	t.rotation.z = 0;

	t.scaling.x = 0;
	t.scaling.y = 0;
	t.scaling.z = 0;

	m_Components[entity - 1] = t;

	return &m_Components[entity - 1];
}

void TransformManager::DestroyComponent(Entity entity) { }
Matrix4 TransformManager::GetWorldMatrix(Transform& component) 
{
	return Matrix4::TransformationMatrix(component.position, (3.14f / 180) * component.rotation, component.scaling);
}
}
