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

#ifndef DESCRIPTOR_HANDLE_H
#define DESCRIPTOR_HANDLE_H
#include <graphics/descriptor.h>
#include <graphics/shader.h>
#include <graphics/descriptor_set.h>

#include <iostream>
#include <graphics/buffers/uniform_handle.h>
#include <graphics/buffers/storage_handle.h>
#include <graphics/buffers/push_handle.h>

namespace dm
{
class PushHandle;
class StorageHandle;

class DescriptorHandle
{
public:
	DescriptorHandle();

	explicit DescriptorHandle(const Pipeline &pipeline);

	/*~DescriptorHandle()
	{
		std::cout << "~DescriptorHandle\n";
	};*/

	template<typename T>
	static T *AsPtr(T &obj) { return &obj; }

	template<typename T>
	static T *AsPtr(T *obj) { return obj; }

	template<typename T>
	static T *AsPtr(const std::shared_ptr<T> &obj) { return obj.get(); }

	template<typename T>
	static T *AsPtr(const std::unique_ptr<T> &obj) { return obj.get(); }

	template<typename T>
	void Push(const std::string &descriptorName, const T &descriptor, const std::optional<OffsetSize> &offsetSize = {})
	{
		if (m_Shader == nullptr)
		{
			return;
		}

		// Finds the local value given to the descriptor name.
		auto it = m_Descriptor.find(descriptorName);

		if (it != m_Descriptor.end())
		{
			// If the descriptor and size have not changed then the write is not modified.
			if (it->second.descriptor == AsPtr(descriptor) && it->second.offsetSize == offsetSize)
			{
				return;
			}

			m_Descriptor.erase(it);
		}

		// Only non-null descriptors can be mapped.
		if (AsPtr(descriptor) == nullptr)
		{
			return;
		}

		// When adding the descriptor find the location in the shader.
		auto location = m_Shader->GetDescriptorLocation(descriptorName);

		if (!location)
		{
			if (m_Shader->ReportedNotFound(descriptorName, true))
			{
				std::cout << "Could not find descriptor in shader '%s' of name '%s'\n" << m_Shader->GetName().c_str() << ", " << descriptorName.c_str();
			}

			return;
		}

		auto descriptorType = m_Shader->GetDescriptorType(*location);

		if (!descriptorType)
		{
			if (m_Shader->ReportedNotFound(descriptorName, true))
			{
				std::cout << "Could not find descriptor in shader '%s' of name '%s' at location '%i'\n" << m_Shader->GetName().c_str() << descriptorName.c_str() << *location;
			}
			return;
		}

		// Adds the new descriptor value.
		auto writeDescriptor = AsPtr(descriptor)->GetWriteDescriptor(*location, *descriptorType, offsetSize);
		m_Descriptor.emplace(descriptorName, DescriptorValue{ AsPtr(descriptor), std::move(writeDescriptor), offsetSize, *location });
		m_Changed = true;
	}

	template<typename T>
	void Push(const std::string &descriptorName, const T &descriptor, WriteDescriptorSet writeDescriptorSet)
	{
		if (m_Shader == nullptr)
		{
			return;
		}

		auto it = m_Descriptor.find(descriptorName);

		if (it != m_Descriptor.end())
		{
			m_Descriptor.erase(it);
		}

		auto location = m_Shader->GetDescriptorLocation(descriptorName);
		auto descriptorType = m_Shader->GetDescriptorType(*location);

		m_Descriptor.emplace(descriptorName, DescriptorValue{ AsPtr(descriptor), std::move(writeDescriptorSet), {}, *location });
		m_Changed = true;
	}

	void Push(const std::string &descriptorName, UniformHandle &uniformHandle, const std::optional<OffsetSize> &offsetSize = {});

	void Push(const std::string &descriptorName, StorageHandle &storageHandle, const std::optional<OffsetSize> &offsetSize = {});

	void Push(const std::string &descriptorName, PushHandle &pushHandle, const std::optional<OffsetSize> &offsetSize = {});

	bool Update(const Pipeline &pipeline);

	void BindDescriptor(const CommandBuffer &commandBuffer, const Pipeline &pipeline);

	const DescriptorSet *GetDescriptorSet() const { return m_DescriptorSet.get(); }
private:
	struct DescriptorValue
	{
		const Descriptor *descriptor;
		WriteDescriptorSet writeDescriptor;
		std::optional<OffsetSize> offsetSize;
		uint32_t location;
	};

	const Shader *m_Shader;
	bool m_PushDescriptor;
	std::unique_ptr<DescriptorSet> m_DescriptorSet;

	std::map<std::string, DescriptorValue> m_Descriptor;
	std::vector<VkWriteDescriptorSet> m_WriteDescriptorSets;

	bool m_Changed;
};
}
#endif DESCRIPTOR_HANDLE_H