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

#include <component/lights/spot_light.h>
#include "imgui_impl_vulkan.h"

namespace dm
{
void SpotLightManager::Init() {}
void SpotLightManager::Update() {}
SpotLight* SpotLightManager::CreateComponent(Entity entity)
{
	SpotLight light;
	light.color = Color::White;
	light.intensity = 1;
	light.angle = 30;
	light.target = glm::vec3(0, -10, 0);

	m_Components[entity - 1] = light;
	return &m_Components[entity - 1];
}
void SpotLightManager::DestroyComponent(Entity entity) {}
void SpotLightManager::OnDrawInspector(const Entity entity)
{
	ImGui::Separator();
	ImGui::TextWrapped("Spot Light");
	ImGui::ColorPicker4("lightColor", &m_Components[entity - 1].color[0]);
	ImGui::DragFloat("intensity", &m_Components[entity - 1].intensity, 0.1f, 0, 100000);
	ImGui::DragFloat3("target", &m_Components[entity - 1].target[0], 0.1f);
	ImGui::DragFloat("angle", &m_Components[entity - 1].angle, 0.1f, 0, 360);
	ImGui::DragFloat("range", &m_Components[entity - 1].range, 0.1f, 0, 360);
}
}
