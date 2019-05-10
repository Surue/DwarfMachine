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

#include <editor/editor.h>
#include <engine/engine.h>
#include <graphics/graphic_manager.h>
#include <editor/editor_renderer.h>
#include <imgui.h>
#include <component/component_manager.h>
#include "imgui_gizmo.h"
#include "entity/entity_handle.h"
#include <glm/ext/matrix_transform.inl>
#include "engine/Input.h"
#include <glm/detail/func_trigonometric.inl>
#include "editor/log.h"

namespace dm
{
Editor::Editor()
{
	GraphicManager::Get()->SetManager(new EditorRenderManager());

	ImGuizmo::SetOrthographic(false);
}

void Editor::Awake()
{
	//TODO ajouter l'appel de la fonction 
	//ImGuizmo::SetOrthographic(false); 
}

void Editor::Start() {}

void Editor::Update(float dt)
{
	MoveEditorCamera(dt);

	lastDeltaTime = dt;

	auto* inputManager = Engine::Get()->GetInputManager();

	if (inputManager->IsKeyDown(KeyCode::SPACE)) {
		log("space");
	}

	if (inputManager->IsKeyDown(KeyCode::W))
		log("W");

	if (inputManager->IsKeyDown(KeyCode::A))
		log("A");

	if (inputManager->IsKeyDown(KeyCode::S))
		log("S");

	if (inputManager->IsKeyDown(KeyCode::D))
		log("D");
}

void Editor::Draw()
{
	DrawDock();

	DrawInspector();

	DrawHierarchy();

	DrawTransformHandle();

	DrawConsole();
}

void Editor::DrawInspector() 
{
	ImGui::Begin("Inspector");

	if(m_CurrentEntitySelected != INVALID_ENTITY)
	{
		Engine::Get()->GetComponentManager()->DrawOnInspector(m_CurrentEntitySelected);
	}

	ImGui::End();
}

void Editor::DrawDock()
{
	bool showDemoWindow = true;

	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &showDemoWindow, windowFlags);
	ImGui::PopStyleVar();

	ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		const ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Docking"))
		{
			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspaceFlags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspaceFlags ^= ImGuiDockNodeFlags_NoSplit;
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspaceFlags & ImGuiDockNodeFlags_NoResize) != 0))                dockspaceFlags ^= ImGuiDockNodeFlags_NoResize;
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspaceFlags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspaceFlags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspaceFlags ^= ImGuiDockNodeFlags_PassthruCentralNode;
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspaceFlags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspaceFlags ^= ImGuiDockNodeFlags_AutoHideTabBar;
			ImGui::Separator();

			ImGui::EndMenu();
		}

		DrawStats();

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void Editor::DrawHierarchy()
{
	ImGui::Begin("Hierarchy");

	EntityManager* manager = Engine::Get()->GetEntityManager();

	for (auto entity : manager->GetEntities())
	{
		if (entity == INVALID_ENTITY) continue;
		std::string oss = "entity " + std::to_string(entity);
		if (ImGui::Selectable(oss.c_str(), m_CurrentEntitySelected == entity))
		{
			m_CurrentEntitySelected = entity;
		}
		ImGui::Separator();
	}

	ImGui::End();
}

void Editor::DrawTransformHandle()
{
	
}

void Editor::DrawStats()
{
	if (ImGui::BeginMenu("Stats"))
	{
		std::string deltaTime = "Delta Time [ms] : ";
		deltaTime += std::to_string(lastDeltaTime);
		ImGui::Text(deltaTime.c_str());

		std::string fps = "FPS : ";
		fps += std::to_string(1 / lastDeltaTime);
		ImGui::Text(fps.c_str());
		ImGui::EndMenu();
	}
}

void Editor::DrawConsole()
{
	ImGui::Begin("Console");
	ImGui::Text(Debug::Print().c_str());
	ImGui::End();
}

void Editor::MoveEditorCamera(float dt)
{
	auto camera = GraphicManager::Get()->GetCamera();

	auto cameraSpeed = 4.5f * dt;
	auto* inputManager = Engine::Get()->GetInputManager();

	if (inputManager->IsKeyHeld(KeyCode::SHIFT_LEFT))
		cameraSpeed *= 2;

	if (inputManager->IsKeyHeld(KeyCode::LEFT) || inputManager->IsKeyHeld(KeyCode::A))
		camera->cameraPos -= glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * cameraSpeed;

	if (inputManager->IsKeyHeld(KeyCode::RIGHT) || inputManager->IsKeyHeld(KeyCode::D))
		camera->cameraPos += glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * cameraSpeed;

	if (inputManager->IsKeyHeld(KeyCode::DOWN) || inputManager->IsKeyHeld(KeyCode::S))
		camera->cameraPos -= cameraSpeed * camera->cameraFront;

	if (inputManager->IsKeyHeld(KeyCode::UP) || inputManager->IsKeyHeld(KeyCode::W))
		camera->cameraPos += cameraSpeed * camera->cameraFront;

	if (inputManager->scrollY > 0.1f)
	{
		camera->cameraPos += 10 * cameraSpeed * camera->cameraFront;
	}
	else if (inputManager->scrollY < -0.1f)
	{
		camera->cameraPos -= 10 * cameraSpeed * camera->cameraFront;
	}

	Vec2i mousePos = inputManager->GetMousePosition();
	static Vec2i lastPos;

	if (inputManager->IsButtonDown(ButtonCode::MIDDLE))
	{
		lastPos = mousePos;
	}

	const auto sensitivity = 0.005f;
	const glm::vec2 offset = glm::vec2((mousePos.x - lastPos.x) * sensitivity, (lastPos.y - mousePos.y) * sensitivity);

	lastPos = mousePos;

	if (inputManager->IsButtonHeld(ButtonCode::MIDDLE))
	{
		camera->cameraPos.x += offset.x;
		camera->cameraPos.y -= offset.y;
	}

	if (inputManager->IsButtonHeld(ButtonCode::RIGHT)) {
		camera->yaw += offset.x * 10.0f;
		camera->pitch -= offset.y * 10.0f;

		if (camera->pitch > 89.0f)
			camera->pitch = 89.0f;
		if (camera->pitch < -89.0f)
			camera->pitch = -89.0f;

	}

	glm::vec3 front;
	front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
	front.y = sin(glm::radians(camera->pitch));
	front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
	camera->cameraFront = glm::normalize(front);
	camera->viewMatrix = glm::lookAt(camera->cameraPos, camera->cameraPos + camera->cameraFront, camera->cameraUp);
}
}
