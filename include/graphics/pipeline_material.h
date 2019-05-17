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

#ifndef PIPELINE_MATERIAL_H
#define PIPELINE_MATERIAL_H

#include <graphics/pipeline_graphic.h>
#include <graphics/render_stage.h>

namespace dm
{
class PipelineMaterial
{
public:
	static PipelineMaterial* Create(const Pipeline::Stage &pipelineStage, const PipelineGraphicsCreate &pipelineCreate);

	PipelineMaterial(Pipeline::Stage pipelineStage, PipelineGraphicsCreate pipelineCreate);

	bool BindPipeline(const CommandBuffer &commandBuffer);

	const Pipeline::Stage &GetStage() const { return m_PipelineStage; }

	const PipelineGraphicsCreate &GetPipelineGraphics() const { return m_PipelineGraphicsCreate; }

	const PipelineGraphics *GetPipeline() const { return m_Pipeline.get(); }
private:
	Pipeline::Stage m_PipelineStage;
	PipelineGraphicsCreate m_PipelineGraphicsCreate;
	const RenderStage *m_RenderStage;
	std::unique_ptr<PipelineGraphics> m_Pipeline;
};
}

#endif PIPELINE_MATERIAL_H