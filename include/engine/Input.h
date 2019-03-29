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

#ifndef INPUT_H
#define INPUT_H

#include <vector>

#include <engine/engine.h>

#include <engine/vector.h>

namespace dm
{

/**
 * \brief Enum representing all key on an US keyboard
 */
enum class KeyCode
{
	SPACE = GLFW_KEY_SPACE,
	A = GLFW_KEY_A,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	Q = GLFW_KEY_Q,
	W = GLFW_KEY_W,
	S = GLFW_KEY_S,
	R = GLFW_KEY_R
};

enum class ButtonCode
{
	LEFT = GLFW_MOUSE_BUTTON_LEFT,
	RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
	MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
};

struct KeyPressedStatus final
{
	unsigned char previousKeyPressed; 
	unsigned char keyPressed;
};

class InputManager
{
public:
	InputManager(Engine& engine);
	~InputManager() = default;

	void Init(GLFWwindow* window);

	void Update();

	bool IsKeyDown(KeyCode key);

	bool IsKeyUp(KeyCode key);

	bool IsKeyHeld(KeyCode key);

	Vec2i GetMousePosition() const;

	bool IsButtonDown(ButtonCode button) const;

	bool IsButtonReleased(ButtonCode button) const;

	bool IsButtonHeld(ButtonCode button) const;
private:
	Engine& m_Engine;

	std::vector<KeyPressedStatus> m_KeyPressedStatus{ GLFW_KEY_LAST };
	std::vector<KeyPressedStatus> m_ButtonPressedStatus{ GLFW_MOUSE_BUTTON_LAST };

	GLFWwindow* m_Window;
	
 };
};
#endif INPUT_H
