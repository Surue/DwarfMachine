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
#include <component/materials/material_default.h>
#include "model.h"
#include "physic/bounding_sphere.h"
#include "drawable.h"
#include "materials/material_skybox.h"
#include "mesh_renderer.h"
#include "light.h"

namespace dm {
class ComponentManagerContainer final
{
public:
	ComponentManagerContainer();
	~ComponentManagerContainer() = default;

	void Init(){};

	void Destroy();

	ComponentBase* CreateComponent(Entity entity, ComponentType componentType) const;

	ComponentBase* AddComponent(Entity entity, ComponentBase& component) const;

	ComponentBase* GetComponent(Entity entity, ComponentType componentType) const;

	void DestroyComponent(Entity entity, ComponentType componentType) const;

	CameraManager* GetCameraManager() const { return m_CameraManager.get(); }

	MaterialDefaultManager* GetMaterialManager() const { return m_MaterialDefaultManager.get(); }

	TransformManager* GetTransformManager() const { return m_TransformManager.get(); }

	LightManager* GetLightManager() const { return m_LightManager.get(); }

	void DrawOnInspector(Entity entity);

	void OnEntityResize(int newSize);

private:
	std::unique_ptr<TransformManager> m_TransformManager;
	std::unique_ptr<CameraManager> m_CameraManager;
	std::unique_ptr<MaterialDefaultManager> m_MaterialDefaultManager;
	std::unique_ptr<MaterialSkyboxManager> m_MaterialSkyboxManager;
	std::unique_ptr<ModelComponentManager> m_MeshManager;
	std::unique_ptr<BoundingSphereManager> m_BoundingSphereManager;
	std::unique_ptr<DrawableManager> m_DrawableManager;
	std::unique_ptr<MeshRendererManager> m_MeshRendererManager;
	std::unique_ptr<LightManager> m_LightManager;
};
}
#endif COMPONENT_MANAGER_H