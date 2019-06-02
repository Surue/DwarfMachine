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

#include <component/lights/directional_light.h>
#include "imgui_impl_vulkan.h"

namespace dm
{
void DirectionalLightManager::Init() {}

void DirectionalLightManager::Update() {}

DirectionalLight* DirectionalLightManager::CreateComponent(Entity entity)
{
	DirectionalLight light;
	light.color = Color::White;
	light.direction = glm::vec3(0, -1.0, 0);
	light.intensity = 1;

	m_Components[entity - 1] = light;
	return &m_Components[entity - 1];
}

void DirectionalLightManager::DestroyComponent(Entity entity) {}

void DirectionalLightManager::OnDrawInspector(Entity entity)
{
	ImGui::Separator();
	ImGui::TextWrapped("Directional Light");
	ImGui::ColorPicker4("lightColor", &m_Components[entity - 1].color[0]);
	ImGui::DragFloat("intensity", &m_Components[entity - 1].intensity, 0.1f, 0, 100000);
	ImGui::DragFloat3("direction", &m_Components[entity - 1].direction[0], 0.1f);
}

void DirectionalLightManager::CreateComponent(json& componentJson, const Entity entity)
{
	DirectionalLight directionalLight;

	if (CheckJsonExists(componentJson, "direction"))
		directionalLight.direction = GetVector3FromJson(componentJson, "direction");

	if (CheckJsonExists(componentJson, "color"))
		directionalLight.color = GetColorFromJson(componentJson, "color");

	if (CheckJsonExists(componentJson, "intensity"))
		directionalLight.intensity = componentJson["intensity"];

	m_Components[entity - 1] = directionalLight;
}
}
