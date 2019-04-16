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

#include <graphics/window.h>
#include <graphics/graphic_manager.h>
#include <iostream>

namespace dm
{
void CallbackFrame(GLFWwindow *window, int32_t width, int32_t height)
{
	//Window::Get()->m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	std::cout << "Resize\n";
}

Window::Window(Engine& engine) : m_Engine(engine)
{
	
}

Window::~Window()
{
	glfwDestroyWindow(m_Window);

	glfwTerminate();
}

void Window::Init()
{
	//Init glfw
	glfwInit();

	//Set context to null (otherwise it would be openGL)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	const auto windowSize = m_Engine.GetSettings().windowSize;
	m_Window = glfwCreateWindow(windowSize.x, windowSize.y, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);

	glfwSetFramebufferSizeCallback(m_Window, CallbackFrame);
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_Window);
}

void Window::SetShouldClose() const
{
	glfwSetWindowShouldClose(m_Window, true);
}

void Window::GetFramebufferSize(int* width, int* height) const
{
	glfwGetFramebufferSize(m_Window, width, height);
}

VkResult Window::CreateSurface(const VkInstance& instance, const VkAllocationCallbacks* allocator,
	VkSurfaceKHR* surface) const
{
	return glfwCreateWindowSurface(instance, m_Window, allocator, surface);
}
}
