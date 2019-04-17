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

#include <engine/Input.h>

namespace dm
{

InputManager::InputManager(Engine& engine) : m_Engine(engine)
{
}

void InputManager::Init(SDL_Window* window)
{
	m_Window = window;
}

void InputManager::Update()
{
	for (size_t i = 0; i < m_KeyPressedStatus.size(); i++)
	{
		m_KeyPressedStatus[i].previousKeyPressed = m_KeyPressedStatus[i].keyPressed;
		//m_KeyPressedStatus[i].keyPressed = false;
	}

	for (size_t i = 0; i < m_ButtonPressedStatus.size(); i++)
	{
		m_ButtonPressedStatus[i].previousKeyPressed = m_ButtonPressedStatus[i].keyPressed;
		//m_ButtonPressedStatus[i].keyPressed = false;
	}

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_WillQuit = true;
			break;

			// SDL2's new way of handling input
		case SDL_TEXTINPUT:
			//TODO ajouter le text input
			break;

		case SDL_KEYDOWN:
		{
			keyboard = SDL_GetKeyboardState(nullptr);

			const int index = event.key.keysym.scancode;
			m_KeyPressedStatus[index].keyPressed = true;
		}
		break;

		case SDL_KEYUP:
		{
			keyboard = SDL_GetKeyboardState(nullptr);

			const int index = event.key.keysym.scancode;
			m_KeyPressedStatus[index].keyPressed = false;
		}
		break;

		case SDL_MOUSEMOTION:
			m_MousePos.x += event.motion.x /*+ cameraX*/;
			m_MousePos.y += event.motion.y /*+ cameraY*/;
			break;

		case SDL_MOUSEBUTTONDOWN:
			this->mouse = SDL_GetMouseState(&(m_MousePos.x),
				&(m_MousePos.y));

			if (event.button.button == SDL_BUTTON_LEFT)
				m_ButtonPressedStatus[static_cast<int>(ButtonCode::LEFT)].keyPressed = true;

			else if (event.button.button == SDL_BUTTON_RIGHT)
				m_ButtonPressedStatus[static_cast<int>(ButtonCode::RIGHT)].keyPressed = true;

			else if (event.button.button == SDL_BUTTON_MIDDLE)
				m_ButtonPressedStatus[static_cast<int>(ButtonCode::MIDDLE)].keyPressed = true;
			break;

		case SDL_MOUSEBUTTONUP:
			this->mouse = SDL_GetMouseState(&(m_MousePos.x),
				&(m_MousePos.y));

			if (event.button.button == SDL_BUTTON_LEFT)
				m_ButtonPressedStatus[static_cast<int>(ButtonCode::LEFT)].keyPressed = false;

			else if (event.button.button == SDL_BUTTON_RIGHT)
				m_ButtonPressedStatus[static_cast<int>(ButtonCode::RIGHT)].keyPressed = false;

			else if (event.button.button == SDL_BUTTON_MIDDLE)
				m_ButtonPressedStatus[static_cast<int>(ButtonCode::MIDDLE)].keyPressed = false;
			break;

			// Brand new SDL2 event.
		case SDL_MOUSEWHEEL:
			// event.x; // Ammount scrolled horizontally
			// // If negative, scrolled to the right
			// // If positive, scrolled to the left

			// event.y; // Ammount scrolled vertically
			// // If negative, scrolled down
			// // If positive, scrolled up
			break;

		default:
			break;
		}
	}

	if (!scrollUpdate) {
		scrollX = 0;
		scrollY = 0;
	}else
	{
		scrollUpdate = false;
	}
}

bool InputManager::IsKeyDown(KeyCode key)
{
	return !m_KeyPressedStatus[static_cast<int>(key)].previousKeyPressed && m_KeyPressedStatus[static_cast<int>(key)].keyPressed;
}

bool InputManager::IsKeyUp(KeyCode key)
{
	return m_KeyPressedStatus[static_cast<int>(key)].previousKeyPressed && !m_KeyPressedStatus[static_cast<int>(key)].keyPressed;
}

bool InputManager::IsKeyHeld(KeyCode key)
{
	return m_KeyPressedStatus[static_cast<int>(key)].keyPressed;
}

Vec2i InputManager::GetMousePosition() const
{
	return m_MousePos;
}

bool InputManager::IsButtonDown(ButtonCode button) const
{
	return !m_ButtonPressedStatus[static_cast<int>(button)].previousKeyPressed && m_ButtonPressedStatus[static_cast<int>(button)].keyPressed;
}

bool InputManager::IsButtonReleased(ButtonCode button) const
{
	return m_ButtonPressedStatus[static_cast<int>(button)].previousKeyPressed && !m_ButtonPressedStatus[static_cast<int>(button)].keyPressed;
}

bool InputManager::IsButtonHeld(ButtonCode button) const
{
	return m_ButtonPressedStatus[static_cast<int>(button)].keyPressed;
}
}
