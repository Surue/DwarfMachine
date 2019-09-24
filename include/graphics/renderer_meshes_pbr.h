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

#ifndef RENDERER_MESHES_PBR_H
#define RENDERER_MESHES_PBR_H
#include <graphics/render_pipeline.h>
#include <system/system.h>
#include <graphics/buffers/uniform_handle.h>
#include "descriptor_handle.h"

namespace dm
{
class RendererMeshesPBR : public RenderPipeline
{
public:
	explicit RendererMeshesPBR(const Pipeline::Stage &pipelineStage);

	~RendererMeshesPBR() = default;

	void Update() override;

	void Draw(const CommandBuffer &commandBuffer) override;

	void RegisterEntity(const Entity entity) override;
private:
	UniformHandle m_UniformScene;
};
}

#endif RENDERER_MESHES_H