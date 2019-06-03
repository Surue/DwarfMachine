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

#ifndef COMPONENT_TYPE_H
#define COMPONENT_TYPE_H
enum class ComponentType : int {
	NONE = 0,
	TRANSFORM = 1,
	CAMERA = 2,
	MATERIAL_DEFAULT = 3,
	MODEL = 4,
	BOUNDING_SPHERE = 5,
	DRAWABLE = 6,
	MATERIAL_SKYBOX = 7,
	MESH_RENDERER = 8,
	POINT_LIGHT = 9, 
	DIRECTIONAL_LIGHT = 10,
	SPOT_LIGHT = 11,
	SHADOW_RENDERER = 12,
	MATERIAL_TERRAIN = 13,
	LENGTH = 14,
};
#endif //COMPONENT_TYPE_H
