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

#include <component/component_manager.h>
#include "entity/entity_handle.h"

namespace dm
{
ComponentManagerContainer::ComponentManagerContainer() :
	m_TransformManager(std::make_unique<TransformManager>()),
	m_CameraManager(std::make_unique<CameraManager>()),
	m_ControllerTypeManager(std::make_unique<ControllerTypeManager>()),
	m_MaterialDefaultManager(std::make_unique<MaterialDefaultManager>()),
	m_MeshManager(std::make_unique<MeshManager>())
{ }

void ComponentManagerContainer::Destroy()
{
	m_TransformManager.reset(nullptr);
	m_CameraManager.reset(nullptr);
	m_ControllerTypeManager.reset(nullptr);
	m_MaterialDefaultManager.reset(nullptr);
	m_MaterialDefaultManager.reset(nullptr);
	m_MaterialDefaultManager.reset(nullptr);
	m_MeshManager.reset(nullptr);
}

ComponentBase* ComponentManagerContainer::CreateComponent(const Entity entity, const ComponentType componentType) const
{
	switch (componentType)
	{
	case ComponentType::NONE:
		throw std::runtime_error("Impossible to add a ComponentType::NONE to an entity");
	case ComponentType::TRANSFORM:
		return m_TransformManager->CreateComponent(entity);
	case ComponentType::CAMERA:
		return m_CameraManager->CreateComponent(entity);
	case ComponentType::CONTROL_TYPE: 
		return m_ControllerTypeManager->CreateComponent(entity);
	case ComponentType::MATERIAL_DEFAULT:
		return m_MaterialDefaultManager->CreateComponent(entity);
	case ComponentType::MESH:
		return m_MeshManager->CreateComponent(entity);
	default:
		throw std::runtime_error("Fail to bind component to its own component manager");
	}
}

ComponentBase* ComponentManagerContainer::AddComponent(const Entity entity, ComponentBase& component) const
{
	switch (component.componentType)
	{
	case ComponentType::NONE:
		throw std::runtime_error("Impossible to add a ComponentType::NONE to an entity");
	case ComponentType::TRANSFORM:
		return static_cast<ComponentBase*>(m_TransformManager->AddComponent(entity, static_cast<Transform&>(component)));
	case ComponentType::CAMERA:
		return static_cast<ComponentBase*>(m_CameraManager->AddComponent(entity, static_cast<Camera&>(component)));
	case ComponentType::CONTROL_TYPE:
		return static_cast<ComponentBase*>(m_ControllerTypeManager->AddComponent(entity, static_cast<ControllerType&>(component)));
	case ComponentType::MATERIAL_DEFAULT:
		return static_cast<ComponentBase*>(m_MaterialDefaultManager->AddComponent(entity, static_cast<MaterialDefault&>(component)));
	case ComponentType::MESH:
		return static_cast<ComponentBase*>(m_MeshManager->AddComponent(entity, static_cast<Mesh&>(component)));
	default:
		throw std::runtime_error("Fail to bind component to its own component manager");
	}
}

ComponentBase* ComponentManagerContainer::GetComponent(const Entity entity, const ComponentType componentType) const
{
	switch (componentType)
	{
	case ComponentType::TRANSFORM:
		return m_TransformManager->GetComponent(entity);
	case ComponentType::CAMERA:
		return m_CameraManager->GetComponent(entity);
	case ComponentType::CONTROL_TYPE:
		return m_ControllerTypeManager->GetComponent(entity);
	case ComponentType::MATERIAL_DEFAULT:
		return m_MaterialDefaultManager->GetComponent(entity);
	case ComponentType::MESH:
		return m_MeshManager->GetComponent(entity);
	case ComponentType::NONE: break;
	case ComponentType::LENGTH: break;
	default: ;
	}

	throw std::runtime_error("Try to get non existent component");
}

void ComponentManagerContainer::DestroyComponent(const Entity entity, const ComponentType componentType) const
{
	switch (componentType)
	{
	case ComponentType::NONE: 
		throw std::runtime_error("Impossible to destroy a ComponentType::NONE from an entity");
	case ComponentType::TRANSFORM: 
		m_TransformManager->DestroyComponent(entity);
		break;
	case ComponentType::CAMERA: 
		m_CameraManager->DestroyComponent(entity);
		break;
	case ComponentType::CONTROL_TYPE: 
		m_ControllerTypeManager->DestroyComponent(entity);
		break;
	case ComponentType::MATERIAL_DEFAULT: 
		m_MaterialDefaultManager->DestroyComponent(entity);
		break;
	case ComponentType::MESH: 
		m_MeshManager->DestroyComponent(entity);
		break;
	case ComponentType::LENGTH: break;
	default: 
		throw std::runtime_error("Fail to bind component to its own component manager");
	}
}

void ComponentManagerContainer::DrawOnInspector(Entity entity)
{
	auto entityHandle = EntityHandle(entity);

	if(entityHandle.HasComponent(ComponentType::TRANSFORM))
	{
		m_TransformManager->OnDrawInspector(entity);
	}

	if (entityHandle.HasComponent(ComponentType::CAMERA))
	{
		m_CameraManager->OnDrawInspector(entity);
	}

	if (entityHandle.HasComponent(ComponentType::MATERIAL_DEFAULT))
	{
		m_MaterialDefaultManager->OnDrawInspector(entity);
	}
}
}
