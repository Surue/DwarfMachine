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

#ifndef CAMERA_H
#define CAMERA_H

#include <component/component.h>

#include <glm/mat4x4.hpp>

namespace dm {
struct Camera final : ComponentBase
{
	glm::mat4x4 proj{};
	glm::mat4x4 viewMatrix{};

	bool isMainCamera;
};

struct Transform;

class CameraManager final : public ComponentBaseManager<Camera>
{
public:
	CameraManager();

	void Awake() override;

	void Start() override;

	void Update() override;

	Camera* CreateComponent(Entity entity) override;

	Camera* AddComponent(Entity entity, Camera& componentBase) override;

	Transform* GetTransformOfCamera(Camera& component);

	void DestroyComponent(Entity entity) override;

	void OnDrawInspector(Entity entity) override;
private:
	GraphicManager* m_GraphicManager;
};
}
#endif