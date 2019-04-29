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

#include <graphics/model.h>
#include "../../externals/SDL2-2.0.9/src/video/SDL_egl_c.h"

namespace dm
{
std::shared_ptr<Model> Model::Create(const std::string filename)
{
	//TODO add modelRegister to store already loaded models
	return nullptr;
}

Model::Model() :
	m_VertexBuffer(nullptr),
	m_IndexBuffer(nullptr),
	m_VertexCount(0),
	m_IndexCount(0)
{}

bool Model::CmdRender(const CommandBuffer& commandBuffer, const uint32_t& instance) const
{
	if(m_VertexBuffer != nullptr && m_IndexBuffer != nullptr)
	{
		VkBuffer vertexBuffer[] = { m_VertexBuffer->GetBuffer() };
		VkDeviceSize offset[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer, offset);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetBuffer(), 0, GetIndexType());
		vkCmdDrawIndexed(commandBuffer, m_IndexCount, instance, 0, 0, 0);
	}
	else if(m_VertexBuffer != nullptr && m_IndexBuffer == nullptr)
	{
		VkBuffer vertexBuffer[] = { m_VertexBuffer->GetBuffer() };
		VkDeviceSize offset[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer, offset);
		vkCmdDraw(commandBuffer, m_VertexCount, instance, 0, 0);
	}
	else
	{
		return false;
	}

	return true;
}
}
