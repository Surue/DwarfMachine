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

#include <graphics/filters/filter_ssao.h>
#include <graphics/graphic_manager.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace dm
{
float RandomFloat(float a, float b) {
	float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

FilterSsao::FilterSsao(const Pipeline::Stage& pipelineStage) :
	PostFilter(pipelineStage, { "../Shaders/filter_default.vert", "../Shaders/filter_ssao.frag" }, {}),
	m_Noise(ComputeNoise(4)),
	m_Kernel(64)
{
	for(uint32_t i = 0; i < 64; ++i)
	{
		glm::vec3 sample = glm::vec3(RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f));
		sample = glm::normalize(sample);
		sample *= RandomFloat(0.0f, 1.0f);
		float scale = static_cast<float>(i) / static_cast<float>(64);
		scale = glm::mix(0.1f, 1.0f, scale * scale);
		m_Kernel[i] = glm::vec4(sample, 1.0f);
	}
}

void FilterSsao::Update() {}

void FilterSsao::Draw(const CommandBuffer& commandBuffer)
{
	auto camera = GraphicManager::Get()->GetCamera();

	m_UniformScene.Push("projection", camera->proj);
	m_UniformScene.Push("cameraPosition", camera->pos);

	m_UniformKernel.Push("kernel", *m_Kernel.data(), sizeof(glm::vec4) * m_Kernel.size());

	m_DescriptorSet.Push("UniformScene", m_UniformScene);
	m_DescriptorSet.Push("UniformKernel", m_UniformKernel);
	m_DescriptorSet.Push("samplerPosition", GraphicManager::Get()->GetAttachment("position"));
	m_DescriptorSet.Push("samplerNormal", GraphicManager::Get()->GetAttachment("normal"));
	m_DescriptorSet.Push("samplerNoise", m_Noise);

	bool updateSuccess = m_DescriptorSet.Update(m_Pipeline);

	if(!updateSuccess)
	{
		return;
	}

	m_Pipeline.BindPipeline(commandBuffer);

	m_DescriptorSet.BindDescriptor(commandBuffer, m_Pipeline);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

std::shared_ptr<Image2d> FilterSsao::ComputeNoise(const uint32_t &size)
{
	std::vector<glm::vec3> ssaoNoise(size * size);

	for(uint32_t i = 0; i < size * size; i++)
	{
		glm::vec3 noise = glm::vec3(RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f), 0.0f);
		noise = glm::normalize(noise);
		ssaoNoise[i] = noise;
	}

	auto noiseImage = std::make_shared<Image2d>("ressources/textures/noise4x4.png");

	return noiseImage;
}
}
