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

#ifndef SHADOW_BOX_H
#define SHADOW_BOX_H
#include <glm/mat4x2.hpp>
#include <array>

namespace dm
{
struct Camera;

class ShadowBox
{
public:
	ShadowBox();

	void Update(const Camera& camera, const glm::mat4 lightMatrix);

	const glm::mat4 &GetProjectionViewMatrix() const { return m_ProjectionViewMatrix; }

private:

	std::array<glm::vec4, 8> CalculateFrustumVertices(const Camera& camera, const glm::vec3 &centreNear, const glm::vec3 &centreFar);

	glm::vec4 CalculateLightSpaceFrustumCorner(const glm::vec3 &startPoint, const glm::vec3 &direction, const float &width) const;


	float m_ShadowOffset;
	float m_ShadowDistance;

	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ProjectionViewMatrix;

	glm::mat4 m_LightViewMatrix;

	float m_FarHeight, m_FarWidth;
	float m_NearHeight, m_NearWidth;

	glm::vec3 m_MinExtents;
	glm::vec3 m_MaxExtents;
};
}
#endif