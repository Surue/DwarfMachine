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

#include <glm/glm.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>
#include <vulkan/vulkan.h>
#include <SFML/Graphics/GLCheck.hpp>

#define APP_SHORT_NAME "vulkansamples_instance"

TEST(Engine, TestAssetImport)
{

	/* VULKAN_KEY_START */

	// initialize the VkApplicationInfo structure
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = APP_SHORT_NAME;
	app_info.applicationVersion = 1;
	app_info.pEngineName = APP_SHORT_NAME;
	app_info.engineVersion = 1;
	app_info.apiVersion = VK_API_VERSION_1_0;

	// initialize the VkInstanceCreateInfo structure
	VkInstanceCreateInfo inst_info = {};
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pNext = NULL;
	inst_info.flags = 0;
	inst_info.pApplicationInfo = &app_info;
	inst_info.enabledExtensionCount = 0;
	inst_info.ppEnabledExtensionNames = NULL;
	inst_info.enabledLayerCount = 0;
	inst_info.ppEnabledLayerNames = NULL;

	VkInstance inst;
	VkResult res;

	res = vkCreateInstance(&inst_info, NULL, &inst);
	if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
		std::cout << "cannot find a compatible Vulkan ICD\n";
		exit(-1);
	}
	else if (res) {
		std::cout << "unknown error\n";
		exit(-1);
	}

	vkDestroyInstance(inst, NULL);

	/* VULKAN_KEY_END */

	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 6;

	auto window = new sf::RenderWindow(sf::VideoMode(720, 720), "test", sf::Style::Default, settings);
	ImGui::SFML::Init(*window);

	auto running = true;
	while (running)
	{

		// Event management
		sf::Event event{};
		while (window->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
			{
				running = false;
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				std::cout << "Key Pressed: " << event.key.code << "\n";

			}
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//switch framebuffer
		window->display();
	}
	delete window;

	std::cout << "success\n";
}