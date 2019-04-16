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
#include <graphics/surface.h>
#include <graphics/instance.h>
#include <graphics/physical_device.h>
#include <graphics/window.h>

namespace dm
{
Surface::Surface(const Instance* instance, const PhysicalDevice* physicalDevice, Window* window) :
	m_Instance(instance),
	m_PhysicalDevice(physicalDevice),
	m_Surface(VK_NULL_HANDLE),
	m_Capabilities({}),
	m_Format({})
{
	window->CreateSurface(*m_Instance, nullptr, &m_Surface);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(*m_PhysicalDevice, m_Surface, &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> surfaceFormat(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(*m_PhysicalDevice, m_Surface, &formatCount, surfaceFormat.data());

	if((formatCount == 1) && (surfaceFormat[0].format == VK_FORMAT_UNDEFINED))
	{
		m_Format.format = VK_FORMAT_B8G8R8A8_UNORM;
		m_Format.colorSpace = surfaceFormat[0].colorSpace;
	}else
	{
		bool found_B8G8R8A_UNORM = false;

		for(auto &&surfaceFormat : surfaceFormat)
		{
			if(surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
			{
				m_Format.format = surfaceFormat.format;
				m_Format.colorSpace = surfaceFormat.colorSpace;
				found_B8G8R8A_UNORM = true;
				break;
			}
		}

		if(!found_B8G8R8A_UNORM)
		{
			m_Format.format = surfaceFormat[0].format;
			m_Format.colorSpace = surfaceFormat[0].colorSpace;
		}
	}
}

Surface::~Surface()
{
	vkDestroySurfaceKHR(*m_Instance, m_Surface, nullptr);
}
}
