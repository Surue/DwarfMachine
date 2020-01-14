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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <string>

#include <component/component.h>

#include <glm/mat4x4.hpp>

namespace dm
{
struct Transform final : ComponentBase
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 worldMatrix = glm::mat4x4(1.0f);

	friend std::ostream & operator<<(std::ostream & out, const Transform transform)
	{
		
		out << "(" << transform.position.x << ", " << transform.position.y << ", "<< transform.position.z << ")";
		return out;
	}
};

class TransformManager final : public ComponentBaseManager<Transform>
{
public:
	TransformManager();

	void Init() override;

	void Update() override;

	Transform* CreateComponent(Entity entity) override;

	Transform* AddComponent(const Entity entity, Transform& component) override;

	void DestroyComponent(Entity entity) override;

	static glm::mat4x4 GetWorldMatrix(Transform &component);

	void OnDrawInspector(Entity entity) override;

	void DecodeComponent(json& componentJson, Entity entity) override;

	void EncodeComponent(json& componentJson, const Entity entity) override;
};
}

#endif TRANSFORM_H