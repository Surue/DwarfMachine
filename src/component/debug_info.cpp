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

#include <component/debug_info.h>
#include "imgui.h"

namespace dm
{
void DebugInfoManager::Init() {}

void DebugInfoManager::Update() {}

DebugInfo* DebugInfoManager::CreateComponent(Entity entity)
{
	DebugInfo debugInfo;
	debugInfo.name = "entity";
	m_Components[entity - 1] = debugInfo;

	return &m_Components[entity - 1];
}

void DebugInfoManager::DestroyComponent(Entity entity) {}

void DebugInfoManager::OnDrawInspector(Entity entity)
{
	ImGui::Separator();
	//ImGui::InputText("Name ", *m_Components[entity - 1].name, 20);
	char str1[128];
	strcpy(str1, m_Components[entity - 1].name.c_str());
	ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));
	m_Components[entity - 1].name = str1;
}

void DebugInfoManager::DecodeComponent(json& componentJson, const Entity entity)
{
	DebugInfo debugInfo;

	if (CheckJsonExists(componentJson, "name"))
	{
		debugInfo.name = componentJson["name"].get<std::string>();
	}

	m_Components[entity - 1] = debugInfo;
}

void DebugInfoManager::EncodeComponent(json& componentJson, const Entity entity)
{
	componentJson["type"] = ComponentType::DEBUG_INFO;

	componentJson["name"] = m_Components[entity - 1].name;
}
}
