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

#ifndef JSON_UTILITY_H
#define JSON_UTILITY_H

#include <string>
#include <memory>

#include <utility/json.hpp>
#include <glm/vec4.hpp>
#include <engine/color.h>
using json = nlohmann::json;

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>



namespace dm
{
	bool IsJsonValueNumeric(const json::value_type& jsonValue);

	bool CheckJsonExists(const json& jsonObject, const std::string& parameterName);

	bool CheckJsonParameter(const json& jsonObject, const std::string& parameterName, json::value_t expectedType);

	bool CheckJsonNumber(const json& jsonObject, const std::string& parameterName);

	bool GetBoolFromJson(const json& jsonObject, const std::string& parameterName);

	glm::vec2 GetVector2FromJson(const json& jsonObject, const std::string& parameterName);

	glm::vec3 GetVector3FromJson(const json& jsonObject, const std::string& parameterName);

	glm::vec4 GetVector4FromJson(const json& jsonObject, const std::string& parameterName);

	Color GetColorFromJson(const json& jsonObject, const std::string& parameterName);

	std::unique_ptr<json> LoadJson(std::string jsonPath);
}
#endif