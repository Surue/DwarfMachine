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

#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H
#include <vector>
#include <system/system.h>
#include <iostream>

namespace dm
{
class Engine;

class SystemManager final
{
public:
	SystemManager(Engine& engine);
	~SystemManager() = default;

	void Init();
	void Update();
	void Destroy();

	void AddComponent(const Entity entity, const ComponentMask oldMask, ComponentMask newMask)
	{
		for (auto& system : m_Systems)
		{
			const auto systemMask = system->GetSignature();
			if(newMask.IsNewMatch(oldMask, systemMask))
			{
				system->RegisterEntity(entity);
			}
		}
	}

	void DestroyComponent(const Entity entity, const ComponentMask oldMask, ComponentMask newMask)
	{
		for (auto& system : m_Systems)
		{
			const auto systemMask = system->GetSignature();
			if (newMask.IsNoLongerMatch(oldMask, systemMask))
			{
				system->UnRegisterEntity(entity);
			}
		}
	}

private:
	Engine& m_Engine;

	std::vector<std::unique_ptr<System>> m_Systems;

};
}
#endif SYSTEM_MANAGER_H