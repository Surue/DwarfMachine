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

InputManager::InputManager(Engine& engine) : m_Engine(engine)
{
}

void InputManager::Init(GLFWwindow* window)
{
	m_Window = window;
}

void InputManager::Update()
{
	for (size_t i = 0; i < m_KeyPressedStatus.size(); i++)
	{
		m_KeyPressedStatus[i].previousKeyPressed = m_KeyPressedStatus[i].keyPressed;
		m_KeyPressedStatus[i].keyPressed = glfwGetKey(m_Window, i);
	}

	for (size_t i = 0; i < m_ButtonPressedStatus.size(); i++)
	{
		m_ButtonPressedStatus[i].previousKeyPressed = m_ButtonPressedStatus[i].keyPressed;
		m_ButtonPressedStatus[i].keyPressed = glfwGetMouseButton(m_Window, i);
	}
}

bool InputManager::IsKeyDown(KeyCode key)
{
	return m_KeyPressedStatus[static_cast<int>(key)].previousKeyPressed != GLFW_PRESS && m_KeyPressedStatus[static_cast<int>(key)].keyPressed == GLFW_PRESS;
}

bool InputManager::IsKeyUp(KeyCode key)
{
	return m_KeyPressedStatus[static_cast<int>(key)].previousKeyPressed == GLFW_PRESS && m_KeyPressedStatus[static_cast<int>(key)].keyPressed != GLFW_PRESS;
}

bool InputManager::IsKeyHeld(KeyCode key)
{
	return m_KeyPressedStatus[static_cast<int>(key)].keyPressed == GLFW_PRESS;
}

Vec2i InputManager::GetMousePosition() const
{
	double x, y;
	glfwGetCursorPos(m_Window, &x, &y);
	return Vec2i(x, y);
}

bool InputManager::IsButtonDown(ButtonCode button) const
{
	return m_ButtonPressedStatus[static_cast<int>(button)].previousKeyPressed != GLFW_PRESS && m_ButtonPressedStatus[static_cast<int>(button)].keyPressed == GLFW_PRESS;
}

bool InputManager::IsButtonReleased(ButtonCode button) const
{
	return m_ButtonPressedStatus[static_cast<int>(button)].previousKeyPressed == GLFW_PRESS && m_ButtonPressedStatus[static_cast<int>(button)].keyPressed != GLFW_PRESS;
}

bool InputManager::IsButtonHeld(ButtonCode button) const
{
	return m_ButtonPressedStatus[static_cast<int>(button)].keyPressed == GLFW_PRESS;
}
}
