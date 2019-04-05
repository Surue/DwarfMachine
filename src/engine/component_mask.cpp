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

#include <engine/component_mask.h>

namespace dm
{
void ComponentMask::AddComponent(ComponentType componentType)
{
	mask |= (1 << static_cast<int>(componentType));
}

void ComponentMask::RemoveComponent(ComponentType componentType)
{
	mask &= ~(1 << static_cast<int>(componentType));
}

bool ComponentMask::Matches(const ComponentMask systemMask) const
{
	return ((mask & systemMask.mask) == systemMask.mask);
}

bool ComponentMask::IsNewMatch(ComponentMask oldMask, const ComponentMask systemMask) const
{
	return Matches(systemMask) && !oldMask.Matches(systemMask);
}

bool ComponentMask::IsNoLongerMatch(ComponentMask oldMask, const ComponentMask systemMask) const
{
	return oldMask.Matches(systemMask) && !Matches(systemMask);
}
}
