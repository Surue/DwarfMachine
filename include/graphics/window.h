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

#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "engine/engine.h"

namespace dm
{
class Window final
{
public:
	Window(Engine& engine);
	~Window();

	void Init();

	operator const GLFWwindow &() const { return *m_Window; }

	GLFWwindow* GetWindow() { return m_Window; }

	static void PollEvents()
	{
		glfwPollEvents();
	}

	static void WaitEvents()
	{
		glfwWaitEvents();
	}

	bool ShouldClose() const;

	void SetShouldClose() const;

	void GetFramebufferSize(int* width, int* height) const;

	VkResult CreateSurface(const VkInstance &instance, const VkAllocationCallbacks *allocator, VkSurfaceKHR *surface) const;
private:
	friend void CallbackFrame(GLFWwindow *window, int32_t width, int32_t height);

	Engine& m_Engine;

	GLFWwindow* m_Window = nullptr;
};
}

#endif WINDOW_H