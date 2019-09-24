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

#ifndef PREFAB_FACTORY_H
#define PREFAB_FACTORY_H
#include <glm/vec3.hpp>
#include <engine/engine.h>
#include <entity/entity_handle.h>
#include <graphics/mesh_manager.h>
#include <graphics/texture_manager.h>

namespace dm
{
struct MaterialDefault;
class EntityManager;

class PrefabFactor
{
public:
	static void CreateRock1(const glm::vec3 pos);

	static void CreateRock2(glm::vec3 pos);

	static void CreateRock3(glm::vec3 pos)
	{

	}

	static void CreateTree1(glm::vec3 pos);

	static void CreateTree2(glm::vec3 pos);

	static void CreateTree3(glm::vec3 pos);

	static void CreateTree4(glm::vec3 pos);

	static void CreateTree5(glm::vec3 pos);

	static void CreateSphere(glm::vec3 pos);

	static void CreateCube(glm::vec3 pos);

	static void CreatePlane(glm::vec3 pos);

	static void CreatePointLight(glm::vec3 pos);

	static void CreateSpotLight(glm::vec3 pos);

	static void CreateTerrain(glm::vec3 pos);

	static void CreateTerrainPrefab(glm::vec3 pos);

	static void CreateSpherePBR(glm::vec3 pos);
};
}

#endif