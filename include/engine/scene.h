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

#ifndef SCENE_H
#define SCENE_H
#include <string>

#include <utility/json_utility.h>
#include <entity/entity.h>
#include <component/component_manager.h>

namespace dm
{
struct SceneInfo
{
	std::string name  = "";
	std::string path  = "";
};

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void Init();

	void Update();

	void Draw();

	/**
	* \brief Load a Scene and create all its GameObject
	* \param scenePath the scene path given by the configuration
	*/
	void LoadSceneFromPath(const std::string& scenePath);

	void LoadSceneFromJson(json& sceneJson);

private:
	void InitScenePySystems();

	EntityManager* m_EntityManager;
	ComponentManagerContainer* m_ComponentManager;

	bool m_IsInited = false;

	std::string m_CurrentScenePath;

	SceneInfo m_SceneInfo;
};
}

#endif
