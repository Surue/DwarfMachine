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

#include <engine/engine.h>
#include <iostream>
#include <vector>
#include <map>

namespace dm
{

enum class KeyCode : unsigned short
{
	SPACE = GLFW_KEY_SPACE,
	Q = GLFW_KEY_Q,
	W = GLFW_KEY_W,
	E = GLFW_KEY_E,
	A = GLFW_KEY_A,
	S = GLFW_KEY_S,
	D = GLFW_KEY_D
};

struct KeyPressedStatus { unsigned char previousKeyPressed; unsigned char keyPressed; };

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
private:
	Engine& m_Engine;

	std::vector<KeyPressedStatus> m_KeyPressedStatus{GLFW_KEY_LAST};

	GLFWwindow* m_Window;
	
 };
};
#endif INPUT_H
