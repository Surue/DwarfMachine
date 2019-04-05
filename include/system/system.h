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

#ifndef SYSTEM_H
#define SYSTEM_H
#include <vector>
#include <entity/entity.h>
#include <component/component_mask.h>

namespace dm
{
class Engine;

class System
{
public:
	System(Engine* engine);
	virtual ~System() = default;
	System(const System &) = default;
	System &operator=(const System &) = default;
	System(System &&) = default;
	System &operator=(System &&) = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	void Destroy();

	void RegisterEntity(const Entity entity);

	void UnRegisterEntity(const Entity entity);

	ComponentMask GetSignature() const;

private:
	Engine* m_Engine = nullptr;

	std::vector<Entity> m_RegisteredEntities;

	ComponentMask m_Signature;
};
}

#endif