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

#include <engine/scene.h>
#include "engine/engine.h"
#include "editor/log.h"
#include "entity/entity_handle.h"

namespace dm
{
SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
	delete(m_EntityManager);
}

void SceneManager::Init()
{
	m_EntityManager = Engine::Get()->GetEntityManager();

	m_IsInited = true;

	if(!m_CurrentScenePath.empty())
	{
		LoadSceneFromPath(m_CurrentScenePath);
	}
}

void SceneManager::Update()
{
}

void SceneManager::Draw()
{
}

void SceneManager::LoadSceneFromPath(const std::string& scenePath)
{

	std::ostringstream oss;
	oss << "Loading scene from: " << scenePath;
	Debug::Log(oss.str());
	
	const auto sceneJsonPtr = LoadJson(scenePath);

	if (sceneJsonPtr != nullptr)
	{
		LoadSceneFromJson(*sceneJsonPtr);
	}
	else
	{
		Debug::Log("Invalid JSON format for scene");
	}
}

void SceneManager::LoadSceneFromJson(json& sceneJson)
{
	Engine::Get()->Clear();

	m_SceneInfo = SceneInfo {};

	if(CheckJsonParameter(sceneJson, "name", json::value_t::string))
	{
		m_SceneInfo.name = sceneJson["name"].get<std::string>();
	}else
	{
		m_SceneInfo.name = "New Scene";
	}

	Debug::Log("Loading Scene: " + m_SceneInfo.name);

	//Load all Entities
	if (CheckJsonParameter(sceneJson, "entities", json::value_t::array))
	{
		const auto entityNmb = sceneJson["entities"].size();
		if (entityNmb > INIT_ENTITY_NMB)
		{
			Debug::Log("Resize entity to => " + entityNmb);
			m_EntityManager->ResizeEntity(entityNmb);
		}

		for (auto& entityJson : sceneJson["entities"])
		{
			Entity entity = INVALID_ENTITY;
			entity = m_EntityManager->CreateEntity();
			EntityHandle handle = EntityHandle(entity);

			//Entity infos
			//TODO ajouter un component de debug si l'engine est en mode editor
			/*if (CheckJsonExists(entityJson, "name"))
			{
				m_EntityManager->GetEntityInfo(entity).name = entityJson["name"].get<std::string>();
			}
			else
			{
				std::ostringstream oss;
				oss << "Entity " << entity;
				m_EntityManager->GetEntityInfo(entity).name = oss.str();
			}*/


			if (entity != INVALID_ENTITY && CheckJsonExists(entityJson, "components"))
			{
				for (auto& componentJson : entityJson["components"])
				{
					if (CheckJsonExists(componentJson, "type"))
					{
						const ComponentType componentType = componentJson["type"];
						const auto index = static_cast<int>(log2(static_cast<double>(componentType)));
						
						m_ComponentManager[index].DecodeComponent(componentJson, entity, componentType);
						handle.AddComponentType(componentType);
						
					}
					else
					{
						std::ostringstream oss;
						oss << "[Error] No type specified for component with json content: " << componentJson;
						Debug::Log(oss.str());
					}
				}
			}
			else
			{
				std::ostringstream oss;
				oss << "[Error] No components attached in the JSON entity: " << entity << "with json content: " << entityJson;
				Debug::Log(oss.str());
			}
		}
	}

}

void SceneManager::SaveScene()
{
	json jsonScene;
	jsonScene["name"] = m_SceneInfo.name;

	jsonScene["entities"] = nlohmann::detail::value_t::array;

	int entityCount = 0;
	for (auto entity : m_EntityManager->GetEntities())
	{
		if(entity == INVALID_ENTITY)
		{
			continue;
		}

		json jsonEntity;

		std::string name;
		name = "entity " + entityCount;
		jsonEntity["name"] = "entity";

		auto handle = EntityHandle(entity);

		auto componentCount = 0;

		for(auto i = 0; i < static_cast<int>(ComponentType::LENGTH); i++)
		{
			if (handle.HasComponent(static_cast<ComponentType>(i)))
			{
				jsonEntity["components"][componentCount] = m_ComponentManager->EncodeComponent(entity, static_cast<ComponentType>(i));
				componentCount++;
			}
		}

		jsonScene["entites"][entityCount] = jsonEntity;

		entityCount++;
	}

	std::string sceneFilename = "../ressources/scenes/";
	sceneFilename += m_SceneInfo.name;
	sceneFilename += ".scene";

	// File write
	std::ofstream myFile;
	myFile.open(sceneFilename);
	myFile.flush();
	myFile << std::setw(4) << jsonScene << std::endl;
	myFile.close();
}
}
