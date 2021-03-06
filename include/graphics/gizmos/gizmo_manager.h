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

#ifndef GIZMO_MANAGER_H
#define GIZMO_MANAGER_H

#include <graphics/gizmos/gizmo.h>
#include <graphics/gizmos/gizmo_type.h>

namespace dm
{
class GizmoManager
{
public:
	GizmoManager();

	void Update();

	void AddGizmo(Gizmo& gizmo);

	void RemoveGizmo(Gizmo *gizmo);

	void Clear();

	const std::map<std::shared_ptr<GizmoType>, std::vector<Gizmo>> &GetGizmos() const { return m_Gizmos; }

	void OnEntityResize();
private:
	std::map<std::shared_ptr<GizmoType>, std::vector<Gizmo>> m_Gizmos;
};
}

#endif