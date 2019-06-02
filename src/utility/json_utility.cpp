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

#include <utility/json_utility.h>


#include <fstream>
#include <editor/log.h>

namespace dm
{
bool IsJsonValueNumeric(const json::value_type& jsonValue)
{
	return jsonValue.type() == json::value_t::number_float ||
		jsonValue.type() == json::value_t::number_integer ||
		jsonValue.type() == json::value_t::number_unsigned;
}

bool CheckJsonExists(const json& jsonObject, const std::string& parameterName)
{
	return jsonObject.find(parameterName) != jsonObject.end();
}

bool CheckJsonParameter(const json& jsonObject, const std::string& parameterName, const json::value_t expectedType)
{
	return CheckJsonExists(jsonObject, parameterName) && jsonObject[parameterName].type() == expectedType;
}

bool CheckJsonNumber(const json& jsonObject, const std::string& parameterName)
{
	return CheckJsonParameter(jsonObject, parameterName, json::value_t::number_float) or
		   CheckJsonParameter(jsonObject, parameterName, json::value_t::number_integer) or
		   CheckJsonParameter(jsonObject, parameterName, json::value_t::number_unsigned);
}

glm::vec2 GetVector2FromJson(const json& jsonObject, const std::string& parameterName)
{
	glm::vec2 vector = glm::vec2();
	if (CheckJsonParameter(jsonObject, parameterName, json::value_t::array))
	{
		if (jsonObject[parameterName].size() == 2)
		{
			auto vectorJson = jsonObject[parameterName];
			if (IsJsonValueNumeric(vectorJson[0]))
			{
				vector.x = vectorJson[0];
			}
			if (IsJsonValueNumeric(vectorJson[1]))
			{
				vector.y = vectorJson[1];
			}
		}
	}
	else if (CheckJsonParameter(jsonObject, parameterName, json::value_t::object))
	{
		auto vectorJson = jsonObject[parameterName];
		if (IsJsonValueNumeric(vectorJson["x"]))
		{
			vector.x = vectorJson["x"];
		}
		if (IsJsonValueNumeric(vectorJson["y"]))
		{
			vector.y = vectorJson["y"];
		}
	}
	return vector;
}

glm::vec3 GetVector3FromJson(const json& jsonObject, const std::string& parameterName)
{
	glm::vec3 vector = glm::vec3();
	if (CheckJsonParameter(jsonObject, parameterName, json::value_t::array))
	{
		if (jsonObject[parameterName].size() == 2)
		{
			auto vectorJson = jsonObject[parameterName];
			if (IsJsonValueNumeric(vectorJson[0]))
			{
				vector.x = vectorJson[0];
			}
			if (IsJsonValueNumeric(vectorJson[1]))
			{
				vector.y = vectorJson[1];
			}
			if (IsJsonValueNumeric(vectorJson[2]))
			{
				vector.z = vectorJson[2];
			}
		}
	}
	else if (CheckJsonParameter(jsonObject, parameterName, json::value_t::object))
	{
		auto vectorJson = jsonObject[parameterName];
		if (IsJsonValueNumeric(vectorJson["x"]))
		{
			vector.x = vectorJson["x"];
		}
		if (IsJsonValueNumeric(vectorJson["y"]))
		{
			vector.y = vectorJson["y"];
		}
		if (IsJsonValueNumeric(vectorJson["z"]))
		{
			vector.z = vectorJson["z"];
		}
	}
	return vector;
}

glm::vec4 GetVector4FromJson(const json& jsonObject, const std::string& parameterName)
{
	glm::vec4 vector = glm::vec4();
	if (CheckJsonParameter(jsonObject, parameterName, json::value_t::array))
	{
		if (jsonObject[parameterName].size() == 2)
		{
			auto vectorJson = jsonObject[parameterName];
			if (IsJsonValueNumeric(vectorJson[0]))
			{
				vector.x = vectorJson[0];
			}
			if (IsJsonValueNumeric(vectorJson[1]))
			{
				vector.y = vectorJson[1];
			}
			if (IsJsonValueNumeric(vectorJson[2]))
			{
				vector.z = vectorJson[2];
			}
			if (IsJsonValueNumeric(vectorJson[3]))
			{
				vector.w = vectorJson[3];
			}
		}
	}
	else if (CheckJsonParameter(jsonObject, parameterName, json::value_t::object))
	{
		auto vectorJson = jsonObject[parameterName];
		if (IsJsonValueNumeric(vectorJson["x"]))
		{
			vector.x = vectorJson["x"];
		}
		if (IsJsonValueNumeric(vectorJson["y"]))
		{
			vector.y = vectorJson["y"];
		}
		if (IsJsonValueNumeric(vectorJson["z"]))
		{
			vector.z = vectorJson["z"];
		}
		if (IsJsonValueNumeric(vectorJson["w"]))
		{
			vector.w = vectorJson["w"];
		}
	}
	return vector;
}

Color GetColorFromJson(const json& jsonObject, const std::string& parameterName)
{
	Color vector = Color::White;
	if (CheckJsonParameter(jsonObject, parameterName, json::value_t::array))
	{
		if (jsonObject[parameterName].size() == 2)
		{
			auto vectorJson = jsonObject[parameterName];
			if (IsJsonValueNumeric(vectorJson[0]))
			{
				vector.r = vectorJson[0];
			}
			if (IsJsonValueNumeric(vectorJson[1]))
			{
				vector.g = vectorJson[1];
			}
			if (IsJsonValueNumeric(vectorJson[2]))
			{
				vector.b = vectorJson[2];
			}
			if (IsJsonValueNumeric(vectorJson[3]))
			{
				vector.a = vectorJson[3];
			}
		}
	}
	else if (CheckJsonParameter(jsonObject, parameterName, json::value_t::object))
	{
		auto vectorJson = jsonObject[parameterName];
		if (IsJsonValueNumeric(vectorJson["r"]))
		{
			vector.r = vectorJson["r"];
		}
		if (IsJsonValueNumeric(vectorJson["g"]))
		{
			vector.g = vectorJson["g"];
		}
		if (IsJsonValueNumeric(vectorJson["b"]))
		{
			vector.b = vectorJson["b"];
		}
		if (IsJsonValueNumeric(vectorJson["a"]))
		{
			vector.a = vectorJson["a"];
		}
	}
	return vector;
}

std::unique_ptr<json> LoadJson(std::string jsonPath)
{
	std::ifstream jsonFile(jsonPath.c_str());
	if (jsonFile.peek() == std::ifstream::traits_type::eof())
	{
		{
			std::ostringstream oss;
			oss << "[JSON ERROR] EMPTY JSON FILE at: " << jsonPath;
			Debug::Log(oss.str());
		}
		return nullptr;
	}
	std::unique_ptr<json> jsonContent = std::make_unique<json>();
	try
	{
		jsonFile >> *jsonContent;
	}
	catch (json::parse_error& e)
	{
		{
			std::ostringstream oss;
			oss << "THE FILE: " << jsonPath << " IS NOT JSON\n" << e.what();
			Debug::Log(oss.str());
		}
		return nullptr;
	}
	return jsonContent;
}
}
