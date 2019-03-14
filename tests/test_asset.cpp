/*
MIT License

Copyright (c) 2017 SAE Institute Switzerland AG

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
#include <iostream>
#include <gtest/gtest.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <engine/engine.h>

class HelloTriangleApplication
{
public:
	void Run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}
private:
	GLFWwindow* window;

	const int WIDTH = 800;
	const int HEIGHT = 600;

	/**
	 * \brief Init a GLFW window
	 */
	void InitWindow()
	{
		//Init glfw
		glfwInit();

		//Set context to null (otherwise it would be openGL)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		//Lock resize function
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	/**
	 * \brief Init Vulkan
	 */
	void InitVulkan()
	{
		
	}

	/**
	 * \brief Main loop of the game
	 */
	void MainLoop()
	{
		while(!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	/**
	 * \brief Use to cleanup vulkan's allocation
	 */
	void Cleanup()
	{
		glfwDestroyWindow(window);

		glfwTerminate();
	}
};

TEST(GLFW, OpeningWindow)
{
	DM::Engine engine;

	try
	{
		engine.Run();
	}catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
		//return EXIT_FAILURE;
	}

	//return EXIT_SUCCESS;
}