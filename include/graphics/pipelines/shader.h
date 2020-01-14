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

#ifndef SHADER_H
#define SHADER_H
#include <map>
#include <vulkan/vulkan.h>
#include "../../externals/glslang/glslang/MachineIndependent/preprocessor/PpContext.h"
#include <optional>

namespace  glslang
{
class TProgram;
class TType;
}

namespace dm
{
class Shader
{
public:
	using Define = std::pair<std::string, std::string>;

	class VertexInput
	{
	public:
		explicit VertexInput(const uint32_t &binding = 0, std::vector<VkVertexInputBindingDescription> bindingDescriptions = {},
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {}) :
			m_Binding(binding),
			m_BindingDescriptions(std::move(bindingDescriptions)),
			m_AttributeDescription(std::move(attributeDescriptions))
		{
		}

		const uint32_t &GetBinding() const { return m_Binding; }

		const std::vector<VkVertexInputBindingDescription> &GetBindingDescriptions() const { return m_BindingDescriptions; }

		const std::vector<VkVertexInputAttributeDescription> &GetAttributeDescriptions() const { return m_AttributeDescription; }

		bool operator<(const VertexInput &other) const
		{
			return m_Binding < other.m_Binding;
		}
	private:
		uint32_t m_Binding;
		std::vector<VkVertexInputBindingDescription> m_BindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> m_AttributeDescription;
	};

	class Uniform
	{
	public:
		explicit Uniform(const int32_t &binding = -1, const int32_t &offset = -1, const int32_t &size = -1, const int32_t &glType = -1, const bool &readOnly = false, const bool &writeOnly = false, const VkShaderStageFlags &stageFlags = 0) :
			m_Binding(binding),
			m_Offset(offset),
			m_Size(size),
			m_GlType(glType),
			m_ReadOnly(readOnly),
			m_WriteOnly(writeOnly),
			m_StageFlags(stageFlags)
		{}

		const int32_t &GetBinding() const { return m_Binding; }

		const int32_t &GetOffset() const { return m_Offset; }

		const int32_t &GetSize() const { return m_Size; }

		const int32_t &GetGlType() const { return m_GlType; }

		const bool &IsReadOnly() const { return m_ReadOnly; }

		const bool &IsWriteOnly() const { return m_WriteOnly; }

		const VkShaderStageFlags &GetStageFlags() const { return m_StageFlags; }

		bool operator==(const Uniform &other) const
		{
			return m_Binding == other.m_Binding && m_Offset == other.m_Offset && m_Size == other.m_Size && m_GlType == other.m_GlType && m_ReadOnly == other.m_ReadOnly
				&& m_WriteOnly == other.m_WriteOnly && m_StageFlags == other.m_StageFlags;
		}

		bool operator!=(const Uniform &other) const
		{
			return !(*this == other);
		}

		std::string ToString() const
		{
			std::stringstream stream;
			stream << "Uniform(binding " << m_Binding << ", offset " << m_Offset << ", size " << m_Size << ", glType " << m_GlType << " , stage " << m_StageFlags << ")";
			return stream.str();
		}
	private:
		friend class Shader;

		int32_t m_Binding;
		int32_t m_Offset;
		int32_t m_Size;
		int32_t m_GlType;
		bool m_ReadOnly;
		bool m_WriteOnly;
		VkShaderStageFlags m_StageFlags;
	};

	class UniformBlock
	{
	public:
		enum class Type
		{
			Uniform, Storage, Push
		};

		explicit UniformBlock(const int32_t &binding = -1, const int32_t &size = -1, const VkShaderStageFlags &stageFlags = 0, const Type &type = Type::Uniform):
			m_Binding(binding),
			m_Size(size),
			m_StageFlags(stageFlags),
			m_Type(type)
		{}

		const int32_t &GetBinding() const { return m_Binding; }

		const int32_t &GetSize() const { return m_Size; }

		const VkShaderStageFlags &GetStageFlags() const { return m_StageFlags; }

		const Type &GetType() const { return m_Type; }

		const std::map<std::string, Uniform> &GetUniforms() const { return m_Uniforms; }

		std::optional<Uniform> GetUniform(const std::string &name) const
		{
			auto it = m_Uniforms.find(name);

			if(it == m_Uniforms.end())
			{
				return {};
			}

			return it->second;
		}

		bool operator==(const UniformBlock &other) const
		{
			return m_Binding == other.m_Binding && m_Size == other.m_Size && m_StageFlags == other.m_StageFlags && m_Type == other.m_Type && m_Uniforms == other.m_Uniforms;
		}

		bool operator!=(const UniformBlock &other) const
		{
			return !(*this == other);
		}

		std::string ToString() const
		{
			std::stringstream stream;
			stream << "UniformBlock(binding " << m_Binding << ", size " << m_Size << ", type " << static_cast<uint32_t>(m_Type) << " , stage " << m_StageFlags <<")";
			return stream.str();
		}
	private:
		friend class Shader;
		int32_t m_Binding;
		int32_t m_Size;
		VkShaderStageFlags m_StageFlags;
		Type m_Type;
		std::map<std::string, Uniform> m_Uniforms;
	};

	class Attribute
	{
	public:
		explicit Attribute(const int32_t &set = -1, const int32_t &location = -1, const int32_t &size = -1, const int32_t &glType = -1) :
			m_Set(set),
			m_Location(location),
			m_Size(size),
			m_GlType(glType)
		{}

		const int32_t &GetSet() const { return m_Set; }

		const int32_t &GetLocation() const { return m_Location; }

		const int32_t &GetSize() const { return m_Size; }

		const int32_t &GetGLType() const { return m_GlType; }

		bool operator==(const Attribute &other) const
		{
			return m_Set == other.m_Set && m_Location == other.m_Location && m_Size == other.m_Size && m_GlType == other.m_GlType;
		}

		bool operator!=(const Attribute &other) const
		{
			return !(*this == other);
		}

		std::string ToString() const
		{
			std::stringstream stream;
			stream << "VertexAttribute(set " << m_Set << "', location " << m_Location << ", size " << m_Size << ", glType " << m_GlType << ")";
			return stream.str();
		}
	private:
		friend Shader;

		int32_t m_Set;
		int32_t m_Location;
		int32_t m_Size;
		int32_t m_GlType;
	};

	explicit Shader(std::string name);

	const std::string &GetName() const { return m_Name; }

	bool ReportedNotFound(const std::string &name, const bool &reportIfFound) const;

	void ProcessShader();

	static VkFormat GlTypeToVk(const int32_t &type);

	std::optional<uint32_t> GetDescriptorLocation(const std::string &name) const;

	std::optional<uint32_t> GetDescriptorSize(const std::string &name) const;

	std::optional<Uniform> GetUniform(const std::string &name) const;

	std::optional<UniformBlock> GetUniformBlock(const std::string &name) const;

	std::optional<Attribute> GetAttribute(const std::string &name) const;

	std::vector<VkPushConstantRange> GetPushConstantRanges() const;

	const uint32_t &GetLastDescriptorBinding() const { return m_LastDescriptorBinding; }

	const std::map<std::string, Uniform> &GetUniforms() const { return m_Uniform; }

	const std::map<std::string, UniformBlock> &GetUniformBlocks() const { return m_UniformBlocks; }

	const std::map<std::string, Attribute> &GetAttributes() const { return m_Attribute; }

	const std::array<std::optional<uint32_t>, 3> &GetLocalSizes() const { return m_LocalSizes; }

	const std::vector<VkDescriptorSetLayoutBinding> &GetDescriptorSetLayouts() const
	{
		return m_DescriptorSetLayout;
	}

	const std::vector<VkDescriptorPoolSize> &GetDescriptorPools() const { return m_DescriptorPools; }

	std::optional<VkDescriptorType> GetDescriptorType(const uint32_t &location) const;

	const std::vector<VkVertexInputAttributeDescription> &GetAttributeDescriptions() const { return m_AttributeDescriptions; }

	static VkShaderStageFlagBits GetShaderStage(const std::string &filename);

	static std::string InsertDefineBlock(const std::string &shaderCode, const std::string &blockCode);

	static std::string ProcessIncludes(const std::string &shaderCode);

	VkShaderModule ProcessShader(const std::string &shaderCode, const VkShaderStageFlags &stageFlag);

	std::string ToString() const;

private:
	static void IncrementDescriptorPool(std::map<VkDescriptorType, uint32_t> &descriptorPoolCounts, const VkDescriptorType &type);

	void LoadUniformBlock(const glslang::TProgram &program, const VkShaderStageFlags &stageFlag, const int32_t &i);

	void LoadUniform(const glslang::TProgram &program, const VkShaderStageFlags &stageFlag, const int32_t &i);

	void LoadVertexAttribute(const glslang::TProgram &program, const VkShaderStageFlags &stageFlag, const int32_t &i);

	static int32_t ComputeSize(const glslang::TType *ttype);

	std::string m_Name;
	std::map<std::string, Uniform> m_Uniform;
	std::map<std::string, UniformBlock> m_UniformBlocks;
	std::map<std::string, Attribute> m_Attribute;

	std::array<std::optional<uint32_t>, 3> m_LocalSizes;

	std::map<std::string, uint32_t> m_DescriptorLocations;
	std::map<std::string, uint32_t> m_DescriptorSizes;

	std::vector<VkDescriptorSetLayoutBinding> m_DescriptorSetLayout;
	uint32_t m_LastDescriptorBinding;
	std::vector<VkDescriptorPoolSize> m_DescriptorPools;
	std::map<uint32_t, VkDescriptorType> m_DescriptorTypes;
	std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;

	mutable std::vector<std::string> m_NotFoundNames;
};
}

#endif SHADER_H