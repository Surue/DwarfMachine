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

#include <engine/component.h>
#include <string>

namespace dm
{
struct Transform final
{
	float x;
	float y;
	float z;

	friend std::ostream & operator<<(std::ostream & out, const Transform transform)
	{
		out << "(" << transform.x << ", " << transform.y << ", "<< transform.z << ")";
		return out;
	}
};

class TransformManager : ComponentBaseManager<Transform>
{
public:
	void Init() override;

	void Update() override;

	void AddComponent(const Entity entity, Transform& componentBase) override
	{
		m_Components[entity - 1] = componentBase;
	}

	void DestroyComponent(Entity entity) override
	{
		
	}

	Transform* GetComponent(const Entity entity) override
	{
		return &m_Components[entity - 1];
	}
private:
};
}

#endif TRANSFORM_H