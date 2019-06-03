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

#include <system/system_manager.h>
#include <graphics/renderer_meshes.h>
#include <system/frustum_culling.h>

namespace dm {
SystemManager::SystemManager()
{
	m_Systems.push_back(std::make_unique<FrustumCulling>());
}

void SystemManager::Init()
{
	for (auto& system : m_Systems)
	{
		system->Init();
	}
}

void SystemManager::Update()
{
	for (auto& system : m_Systems)
	{
		system->Update();
	}
}

void SystemManager::Clear()
{
	m_Systems.clear();

	m_Systems.push_back(std::make_unique<FrustumCulling>());
}

void SystemManager::Draw()
{
}

void SystemManager::Destroy()
{
	for (auto& system : m_Systems)
	{
		system->Destroy();
	}
}

void SystemManager::AddComponent(const Entity entity, const ComponentMask oldMask, ComponentMask newMask)
{
	for (auto& system : m_Systems)
	{
		const auto systemMask = system->GetSignature();
		if (newMask.IsNewMatch(oldMask, systemMask))
		{
			system->RegisterEntity(entity);
		}
	}
}

void SystemManager::DestroyComponent(const Entity entity, const ComponentMask oldMask, ComponentMask newMask)
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
}
