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

#ifndef RENDERER_DEFERRED_H
#define RENDERER_DEFERRED_H
#include <graphics/render_pipeline.h>
#include "image_2d.h"
#include "image_cube.h"
#include "engine/color.h"
#include <glm/vec3.hpp>
#include "descriptor_handle.h"
#include "pipelines/pipeline_graphic.h"
#include <future>

namespace dm
{
class RendererDeferred : public RenderPipeline
{
public:
	explicit RendererDeferred(const Pipeline::Stage &pipelineStage);

	~RendererDeferred() = default;

	void Update() override;

	void Draw(const CommandBuffer& commandBuffer) override;

	static std::unique_ptr<Image2d> ComputeBRDF(const uint32_t &size);

	static std::unique_ptr<ImageCube> ComputeIrradiance(const std::shared_ptr<ImageCube> &source, const uint32_t &size);

	static std::unique_ptr<ImageCube> ComputePrefiltered(const std::shared_ptr<ImageCube> &source, const uint32_t &size);
private:
	struct DeferredLight
	{
		Color color = Color::White;
		glm::vec3 position;
		float radius;
	};

	std::vector<Shader::Define> GetDefines();

	DescriptorHandle m_DescriptorSet;
	UniformHandle m_UniformScene;
	StorageHandle m_StorageLight;

	PipelineGraphics m_Pipeline;

	//BRDF
	std::future<std::unique_ptr<Image2d>> m_FutureBRDF;
	std::optional<std::unique_ptr<Image2d>> m_CurrentBRDF;

	std::shared_ptr<ImageCube> m_Skybox;

	//Irradiance
	std::future<std::unique_ptr<Image2d>> m_FutureIrradiance;
	std::optional<std::unique_ptr<Image2d>> m_CurrentIrradiance;

	//Prefiltered
	std::future<std::unique_ptr<Image2d>> m_FuturePrefiltered;
	std::optional<std::unique_ptr<Image2d>> m_CurrentPrefiltered;
};
}

#endif