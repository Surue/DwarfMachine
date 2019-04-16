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

Window::Window(Engine& engine) : m_Engine(engine)
{
	
}

Window::~Window()
{
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

void Window::Init()
{
	SDL_Init(SDL_INIT_VIDEO);


	const auto windowSize = m_Engine.GetSettings().windowSize;
	m_Window = SDL_CreateWindow(
		"An SDL2 Vulkan window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		windowSize.x,                               // width, in pixels
		windowSize.y,                               // height, in pixels
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE               // flags - see below
	);

	if (m_Window == nullptr) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		throw std::runtime_error("");
	}
	SDL_GetWindowData(m_Window, "Dwarf Machine");
}

void Window::PollEvents()
{
	SDL_PollEvent(&m_Event);
}

void Window::WaitEvents()
{
	SDL_WaitEvent(&m_Event);
}

bool Window::ShouldClose() const
{
	return m_ShouldQuit;
}

void Window::SetShouldClose()
{
	m_ShouldQuit = true;
}

void Window::GetFramebufferSize(int* width, int* height) const
{
	SDL_Vulkan_GetDrawableSize(m_Window, width, height);
}

void Window::CreateSurface(const VkInstance& instance, const VkAllocationCallbacks* allocator,
	VkSurfaceKHR* surface) const
{
	if (!SDL_Vulkan_CreateSurface(m_Window, instance, surface)) {
		throw std::runtime_error("Failed to create window surface!");
	}
}
}
