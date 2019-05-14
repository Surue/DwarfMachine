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

#include <graphics/gizmos/gizmo_manager.h>

namespace dm
{
GizmoManager::GizmoManager()
{
}

void GizmoManager::Update()
{
	for(auto it = m_Gizmos.begin(); it != m_Gizmos.end();)
	{
		if(it->second.empty())
		{
			it = m_Gizmos.erase(it);
			continue;
		}

		(*it).first->Update((*it).second);
		++it;
	}
}

Gizmo* GizmoManager::AddGizmo(Gizmo* gizmo)
{
	auto it = m_Gizmos.find(gizmo->gizmoType);

	if(it == m_Gizmos.end())
	{
		m_Gizmos.emplace(gizmo->gizmoType, std::vector<Gizmo>());
		it = m_Gizmos.find(gizmo->gizmoType);
	}

	(*it).second.emplace_back(*gizmo);
	return gizmo;
}

void GizmoManager::RemoveGizmo(Gizmo* gizmo)
{
	auto it = m_Gizmos.find(gizmo->gizmoType);

	if(it != m_Gizmos.end())
	{
		it->second.erase(std::remove_if(it->second.begin(), it->second.end(), [&](Gizmo g)
		{
			return g == *gizmo;
		}), it->second.end());
	}
}

void GizmoManager::Clear()
{
	m_Gizmos.clear();
}
}
