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

#ifndef FILTER_SSAO_H
#define FILTER_SSAO_H
#include <graphics/post_filter.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace dm
{
class FilterSsao : public PostFilter
{
public:
	explicit FilterSsao(const Pipeline::Stage &pipelineStage);
	void Update() override;
	void Draw(const CommandBuffer& commandBuffer) override;
private:
	static std::shared_ptr<Image2d> ComputeNoise(const uint32_t &size);

	UniformHandle m_UniformScene;
	UniformHandle m_UniformKernel;

	std::shared_ptr<Image2d> m_Noise;

	std::vector<glm::vec4> m_Kernel;
};
}

#endif