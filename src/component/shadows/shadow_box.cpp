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

#include <component/shadows/shadow_box.h>
#include <component/camera.h>
#include <SDL_stdinc.h>

namespace dm
{
ShadowBox::ShadowBox(): m_ShadowOffset(0), m_ShadowDistance(0), m_FarHeight(0), m_FarWidth(0), m_NearHeight(0),
                        m_NearWidth(0)
{
}

void ShadowBox::Update(const Camera& camera, const glm::mat4 lightMatrix)
{
	m_LightViewMatrix = lightMatrix;

	m_FarWidth = m_ShadowDistance * std::tan(camera.fov * (M_PI / 180.f));
	m_NearWidth = camera.frustumNear * std::tan(camera.fov * (M_PI / 180.f));
	m_FarHeight = m_FarWidth / camera.aspect;
	m_NearHeight = m_NearWidth / camera.aspect;

	auto forwardVector = camera.front;

	auto toFar = forwardVector * m_ShadowDistance;
	auto toNear = forwardVector * camera.frustumNear;
	auto centreNear = toNear + camera.pos;
	auto centreFar = toFar + camera.pos;

	auto points = CalculateFrustumVertices(camera, centreNear, centreFar);

	m_MinExtents = glm::vec3(100000, 100000, 100000);
	m_MaxExtents = glm::vec3(-100000, -100000, -100000);

	for (uint32_t i = 0; i < points.size(); i++)
	{
		auto point = glm::vec3(points[i]);
		m_MinExtents = glm::min(m_MinExtents, point);
		m_MaxExtents = glm::max(m_MinExtents, point);
	}

	m_MaxExtents.z += m_ShadowOffset;
}

std::array<glm::vec4, 8> ShadowBox::CalculateFrustumVertices(const Camera& camera, const glm::vec3& centreNear, const glm::vec3& centreFar)
{
	auto upVector = camera.up;
	auto rightVector = camera.right;
	auto downVector = -upVector;
	auto leftVector = -rightVector;

	auto farTop = centreFar + (upVector * m_FarHeight);
	auto farBottom = centreFar + (downVector * m_FarHeight);
	auto nearTop = centreNear + (upVector * m_NearHeight);
	auto nearBottom = centreNear + (downVector * m_NearHeight);

	auto points = std::array<glm::vec4, 8>();
	points[0] = CalculateLightSpaceFrustumCorner(farTop, rightVector, m_FarWidth);
	points[1] = CalculateLightSpaceFrustumCorner(farTop, leftVector, m_FarWidth);
	points[2] = CalculateLightSpaceFrustumCorner(farBottom, rightVector, m_FarWidth);
	points[3] = CalculateLightSpaceFrustumCorner(farBottom, leftVector, m_FarWidth);
	points[4] = CalculateLightSpaceFrustumCorner(nearTop, rightVector, m_NearWidth);
	points[5] = CalculateLightSpaceFrustumCorner(nearTop, leftVector, m_NearWidth);
	points[6] = CalculateLightSpaceFrustumCorner(nearBottom, rightVector, m_NearWidth);
	points[7] = CalculateLightSpaceFrustumCorner(nearBottom, leftVector, m_NearWidth);
	return points;
}

glm::vec4 Transform(const glm::mat4 matrix, const glm::vec4 &other)
{
	glm::vec4 result = glm::vec4();

	for (int32_t row = 0; row < 4; row++)
	{
		result[row] = matrix[0][row] * other.x + matrix[1][row] * other.y + matrix[2][row] * other.z + matrix[3][row] * other.w;
	}

	return result;
}

glm::vec4 ShadowBox::CalculateLightSpaceFrustumCorner(const glm::vec3& startPoint, const glm::vec3& direction,
	const float& width) const
{
	glm::vec3 tmp = (startPoint + (direction * width));
	glm::vec4 point = glm::vec4(tmp.x, tmp.y, tmp.z, 0);
	point = Transform(m_LightViewMatrix, point);
	return point;
}
};
