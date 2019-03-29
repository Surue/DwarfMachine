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

void InputManager::Init(GLFWwindow* window)
{
	m_Window = window;
}

void InputManager::Update()
{
	for (auto i = 0; i < GLFW_KEY_LAST; i++)
	{
		m_KeyPressedStatus[i].previousKeyPressed = m_KeyPressedStatus[i].keyPressed;
		m_KeyPressedStatus[i].keyPressed = glfwGetKey(m_Window, i);
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
}
