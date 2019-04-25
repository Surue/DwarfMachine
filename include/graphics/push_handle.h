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

#ifndef PUSH_HANDLE_H
#define PUSH_HANDLE_H

#include <graphics/pipeline.h>

namespace dm
{
class PushHandle
{
public:
	explicit PushHandle(const bool &multiPipeline = false);

	explicit PushHandle(const Shader::UniformBlock &uniformBlock, const bool &multiPipeline = false);

	template<typename T>
	void Push(const T &object, const std::size_t &offset, const std::size_t &size)
	{
		std::memcpy(m_data.get() + offset, &object, size);
	}

	template<typename T>
	void Push(const std::string &uniformName, const T &object, const std::size_t &size = 0)
	{
		if (!m_UniformBlock)
		{
			return;
		}

		auto uniform = m_UniformBlock->GetUniform(uniformName);

		if (!uniform)
		{
			return;
		}

		auto realSize = size;

		if (realSize == 0)
		{
			realSize == std::min(sizeof(object), static_cast<std::size_t>(uniform->GetSize()));
		}

		Push(object, static_cast<std::size_t>(uniform->GetOffset()), realSize);
	}

	bool Update(const std::optional<Shader::UniformBlock> &uniformBlock);

	void BindPush(const CommandBuffer &commandBuffer, const Pipeline &pipeline);
private:
	bool m_MultiPipeline;
	std::optional<Shader::UniformBlock> m_UniformBlock;
	std::unique_ptr<char[]> m_Data;
};
}

#endif PUSH_HANDLE_H