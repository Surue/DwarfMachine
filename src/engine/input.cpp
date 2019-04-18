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
#include <iostream>

namespace dm
{

InputManager::InputManager(Engine& engine) : 
	m_Engine(engine), 
	keyboard(nullptr), 
	mouse(0), 
	m_MousePos(0, 0)
{
}

void InputManager::Init(SDL_Window* window)
{
	m_Window = window;
}

void InputManager::Update()
{
	for (size_t i = 0; i < KEYBOARD_SIZE; i++)
	{
		m_KeyPressedDown[i] = false;
		m_KeyPressedUp[i] = false;
	}

	for (size_t i = 0; i < static_cast<int>(ButtonCode::MOUSE_MAX); i++)
	{
		m_ButtonUp[i] = false;
		m_ButtonDown[i] = false;
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
			this->keyboard = SDL_GetKeyboardState(nullptr);

			const int index = event.key.keysym.scancode;
			m_KeyPressedDown[index] = true;
		}
		break;

		case SDL_KEYUP:
		{
			this->keyboard = SDL_GetKeyboardState(nullptr);

			const int index = event.key.keysym.scancode;
			m_KeyPressedUp[index] = true;
		}
		break;

		case SDL_MOUSEMOTION:
			m_MousePos.x = -event.motion.x ;
			m_MousePos.y = -event.motion.y;
			break;

		case SDL_MOUSEBUTTONDOWN:
			this->mouse = SDL_GetMouseState(&(m_MousePos.x),
				&(m_MousePos.y));

			if (event.button.button == SDL_BUTTON_LEFT)
				m_ButtonDown[static_cast<int>(ButtonCode::LEFT)] = true;

			else if (event.button.button == SDL_BUTTON_RIGHT)
				m_ButtonDown[static_cast<int>(ButtonCode::RIGHT)] = true;

			else if (event.button.button == SDL_BUTTON_MIDDLE)
				m_ButtonDown[static_cast<int>(ButtonCode::MIDDLE)] = true;
			break;

		case SDL_MOUSEBUTTONUP:
			this->mouse = SDL_GetMouseState(&(m_MousePos.x),
				&(m_MousePos.y));

			if (event.button.button == SDL_BUTTON_LEFT)
				m_ButtonUp[static_cast<int>(ButtonCode::LEFT)] = true;

			else if (event.button.button == SDL_BUTTON_RIGHT)
				m_ButtonUp[static_cast<int>(ButtonCode::RIGHT)] = true;

			else if (event.button.button == SDL_BUTTON_MIDDLE)
				m_ButtonUp[static_cast<int>(ButtonCode::MIDDLE)] = true;
			break;

			// Brand new SDL2 event.
		case SDL_MOUSEWHEEL:
			scrollX = event.wheel.y;
			scrollY = event.wheel.y;

			scrollUpdate = true;
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
	return m_KeyPressedDown[static_cast<int>(key)];
}

bool InputManager::IsKeyUp(KeyCode key)
{
	return m_KeyPressedUp[static_cast<int>(key)];
}

bool InputManager::IsKeyHeld(KeyCode key) const
{
	if (!keyboard || keyboard == nullptr)
		return false;

	const auto sdlKey = static_cast<int>(key);

	return keyboard[sdlKey] != 0;
}

Vec2i InputManager::GetMousePosition() const
{
	return m_MousePos;
}

bool InputManager::IsButtonDown(ButtonCode button) const
{
	return m_ButtonDown[static_cast<int>(button)];
}

bool InputManager::IsButtonReleased(ButtonCode button) const
{
	return m_ButtonUp[static_cast<int>(button)];
}

bool InputManager::IsButtonHeld(const ButtonCode button) const
{
	switch (button)
	{
	case ButtonCode::LEFT:
		if (mouse & SDL_BUTTON(1))
			return true;
		break;

	case ButtonCode::RIGHT:
		if (mouse & SDL_BUTTON(3))
			return true;
		break;

	case ButtonCode::MIDDLE:
		if (mouse & SDL_BUTTON(2))
			return true;
		break;

	default:
		break;
	}

	return false;
}
}
