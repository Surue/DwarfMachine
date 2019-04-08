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

#ifndef SURFACE_H
#define SURFACE_H

#include <vulkan/vulkan.h>

namespace dm
{
class Instance;
class PhysicalDevice;

class Surface
{
public:
	Surface(const Instance *instance, const PhysicalDevice *physicalDevice, GLFWwindow* window);
	~Surface();

	operator const VkSurfaceKHR &() const { return m_Surface; }
	const VkSurfaceKHR &GetSurface() const { return m_Surface; }
	const VkSurfaceCapabilitiesKHR &GetCapabilities() const { return m_Capabilities; }
	const VkSurfaceFormatKHR &GetFormat() const { return m_Format; }
private:
	const Instance *m_Instance;
	const PhysicalDevice *m_PhysicalDevice;

	VkSurfaceKHR m_Surface;
	VkSurfaceCapabilitiesKHR m_Capabilities;
	VkSurfaceFormatKHR m_Format;
};
}

#endif
