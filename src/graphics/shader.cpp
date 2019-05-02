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

#include <graphics/shader.h>
#include <graphics/uniform_buffer.h>
#include "graphics/storage_buffer.h"
#include "graphics/image_2d.h"
#include <graphics/image_cube.h>
#include <iostream>
#include "engine/file.h"
#include <graphics/graphic_manager.h>
#include "../../externals/glslang/SPIRV/GlslangToSpv.h"

namespace dm
{
Shader::Shader(std::string name):
	m_Name(std::move(name)),
	m_LastDescriptorBinding(0)
{}

bool Shader::ReportedNotFound(const std::string& name, const bool& reportIfFound) const
{
	if(std::find(m_NotFoundNames.begin(), m_NotFoundNames.end(), name) == m_NotFoundNames.end())
	{
		if(reportIfFound)
		{
			m_NotFoundNames.emplace_back(name);
		}

		return true;
	}

	return false;
}

void Shader::ProcessShader()
{
	std::map<VkDescriptorType, uint32_t> descriptorPoolCounts;

	// Process to descriptors.
	for (const auto &[uniformBlockName, uniformBlock] : m_UniformBlocks)
	{
		VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

		switch (uniformBlock.m_Type)
		{
		case UniformBlock::Type::Uniform:
			descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			m_DescriptorSetLayout.emplace_back(UniformBuffer::GetDescriptorSetLayout(static_cast<uint32_t>(uniformBlock.m_Binding), descriptorType, uniformBlock.m_StageFlags, 1));
			break;
		case UniformBlock::Type::Storage:
			descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			m_DescriptorSetLayout.emplace_back(StorageBuffer::GetDescriptorSetLayout(static_cast<uint32_t>(uniformBlock.m_Binding), descriptorType, uniformBlock.m_StageFlags, 1));
			break;
		case UniformBlock::Type::Push:
			// Push constants are described in the pipeline.
			break;
		default:
			break;
		}

		IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
		m_DescriptorLocations.emplace(uniformBlockName, uniformBlock.m_Binding);
		m_DescriptorSizes.emplace(uniformBlockName, uniformBlock.m_Size);
	}

	for (const auto &[uniformName, uniform] : m_Uniform)
	{
		VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

		switch (uniform.m_GlType)
		{
		case 0x8B5E: // GL_SAMPLER_2D
		case 0x904D: // GL_IMAGE_2D
		case 0x9108: // GL_SAMPLER_2D_MULTISAMPLE
		case 0x9055: // GL_IMAGE_2D_MULTISAMPLE
			descriptorType = uniform.m_WriteOnly ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			m_DescriptorSetLayout.emplace_back(Image2d::GetDescriptorSetLayout(static_cast<uint32_t>(uniform.m_Binding), descriptorType, uniform.m_StageFlags, 1));
			break;
		case 0x8B60: // GL_SAMPLER_CUBE
		case 0x9050: // GL_IMAGE_CUBE
			descriptorType = uniform.m_WriteOnly ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			m_DescriptorSetLayout.emplace_back(ImageCube::GetDescriptorSetLayout(static_cast<uint32_t>(uniform.m_Binding), descriptorType, uniform.m_StageFlags, 1));
			break;
		default:
			break;
		}

		IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
		m_DescriptorLocations.emplace(uniformName, uniform.m_Binding);
		m_DescriptorSizes.emplace(uniformName, uniform.m_Size);
	}

	for (const auto &[type, descriptorCount] : descriptorPoolCounts)
	{
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = type;
		descriptorPoolSize.descriptorCount = descriptorCount;
		m_DescriptorPools.emplace_back(descriptorPoolSize);
	}

	// FIXME: This is a AMD workaround that works on Nvidia too...
	m_DescriptorPools = std::vector<VkDescriptorPoolSize>(6);
	m_DescriptorPools[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	m_DescriptorPools[0].descriptorCount = 4096;
	m_DescriptorPools[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	m_DescriptorPools[1].descriptorCount = 2048;
	m_DescriptorPools[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	m_DescriptorPools[2].descriptorCount = 2048;
	m_DescriptorPools[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	m_DescriptorPools[3].descriptorCount = 2048;
	m_DescriptorPools[4].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	m_DescriptorPools[4].descriptorCount = 2048;
	m_DescriptorPools[5].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	m_DescriptorPools[5].descriptorCount = 2048;

	// Sort descriptors by binding.
	std::sort(m_DescriptorSetLayout.begin(), m_DescriptorSetLayout.end(), [](const VkDescriptorSetLayoutBinding &l, const VkDescriptorSetLayoutBinding &r)
	{
		return l.binding < r.binding;
	});

	// Gets the last descriptors binding.
	if (!m_DescriptorSetLayout.empty())
	{
		m_LastDescriptorBinding = m_DescriptorSetLayout.back().binding;
	}

	// Gets the descriptor type for each descriptor.
	for (const auto &descriptor : m_DescriptorSetLayout)
	{
		m_DescriptorTypes.emplace(descriptor.binding, descriptor.descriptorType);
	}

	// Process attribute descriptions.
	uint32_t currentOffset = 4;

	for (const auto &[attributeName, attribute] : m_Attribute)
	{
		VkVertexInputAttributeDescription attributeDescription = {};
		attributeDescription.location = static_cast<uint32_t>(attribute.m_Location);
		attributeDescription.binding = 0;
		attributeDescription.format = GlTypeToVk(attribute.m_GlType);
		attributeDescription.offset = currentOffset;
		m_AttributeDescriptions.emplace_back(attributeDescription);
		currentOffset += attribute.m_Size;
	}
}

VkFormat Shader::GlTypeToVk(const int32_t& type)
{
	switch (type)
	{
	case 0x1406: // GL_FLOAT
		return VK_FORMAT_R32_SFLOAT;
	case 0x8B50: // GL_FLOAT_VEC2
		return VK_FORMAT_R32G32_SFLOAT;
	case 0x8B51: // GL_FLOAT_VEC3
		return VK_FORMAT_R32G32B32_SFLOAT;
	case 0x8B52: // GL_FLOAT_VEC4
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case 0x1404: // GL_INT
		return VK_FORMAT_R32_SINT;
	case 0x8B53: // GL_INT_VEC2
		return VK_FORMAT_R32G32_SINT;
	case 0x8B54: // GL_INT_VEC3
		return VK_FORMAT_R32G32B32_SINT;
	case 0x8B55: // GL_INT_VEC4
		return VK_FORMAT_R32G32B32A32_SINT;
	case 0x1405: // GL_UNSIGNED_INT
		return VK_FORMAT_R32_SINT;
	case 0x8DC6: // GL_UNSIGNED_INT_VEC2
		return VK_FORMAT_R32G32_SINT;
	case 0x8DC7: // GL_UNSIGNED_INT_VEC3
		return VK_FORMAT_R32G32B32_SINT;
	case 0x8DC8: // GL_UNSIGNED_INT_VEC4
		return VK_FORMAT_R32G32B32A32_SINT;
	default:
		return VK_FORMAT_UNDEFINED;
	}
}

std::optional<uint32_t> Shader::GetDescriptorLocation(const std::string& name) const
{
	auto it = m_DescriptorLocations.find(name);

	if (it == m_DescriptorLocations.end())
	{
		return {};
	}

	return it->second;
}

std::optional<uint32_t> Shader::GetDescriptorSize(const std::string& name) const
{
	auto it = m_DescriptorSizes.find(name);

	if(it == m_DescriptorSizes.end())
	{
		return {};
	}

	return it->second;
}

std::optional<Shader::Uniform> Shader::GetUniform(const std::string& name) const
{
	auto it = m_Uniform.find(name);

	if(it == m_Uniform.end())
	{
		return {};
	}

	return it->second;
}

std::optional<Shader::UniformBlock> Shader::GetUniformBlock(const std::string& name) const
{
	auto it = m_UniformBlocks.find(name);

	if (it == m_UniformBlocks.end())
	{
		return {};
	}

	return it->second;
}

std::optional<Shader::Attribute> Shader::GetAttribute(const std::string& name) const
{
	auto it = m_Attribute.find(name);

	if (it == m_Attribute.end())
	{
		return {};
	}

	return it->second;
}

std::vector<VkPushConstantRange> Shader::GetPushConstantRanges() const
{
	std::vector<VkPushConstantRange> pushConstantRanges;
	uint32_t currentOffset = 0;

	for(const auto &[uniformBlockName, uniformBlock] : m_UniformBlocks)
	{
		if(uniformBlock.GetType() != Shader::UniformBlock::Type::Push)
		{
			continue;
		}

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = uniformBlock.GetStageFlags();
		pushConstantRange.offset = currentOffset;
		pushConstantRange.size = static_cast<uint32_t>(uniformBlock.GetSize());
		pushConstantRanges.emplace_back(pushConstantRange);
		currentOffset += pushConstantRange.size;
	}

	return pushConstantRanges;
}

std::optional<VkDescriptorType> Shader::GetDescriptorType(const uint32_t& location) const
{
	auto it = m_DescriptorTypes.find(location);

	if (it == m_DescriptorTypes.end())
	{
		return {};
	}

	return it->second;
}

std::string Lowercase(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
}

std::string FileSuffix(const std::string &path)
{
	auto start = path.find_last_of('\\');

	if (start == std::string::npos)
	{
		start = 0;
	}

	else
	{
		start++; // We do not want the separator.
	}

	auto end = path.find_last_of('.');

	if (end == std::string::npos || end < start)
	{
		return "";
	}

	return path.substr(end);
}

VkShaderStageFlagBits Shader::GetShaderStage(const std::string& filename)
{
	std::string fileExt = Lowercase(FileSuffix(filename));

	if(fileExt == ".comp")
	{
		return VK_SHADER_STAGE_COMPUTE_BIT;
	}

	if(fileExt == ".vert")
	{
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

	if(fileExt == ".frag")
	{
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	std::cout << "no shader extension for " << fileExt << "\n";
	return VK_SHADER_STAGE_ALL;
}

std::string Shader::InsertDefineBlock(const std::string& shaderCode, const std::string& blockCode)
{
	std::string updatedCode = shaderCode;
	std::size_t foundIndex0 = updatedCode.find('\n', 0);
	std::size_t foundIndex1 = updatedCode.find('\n', foundIndex0 + 1);
	std::size_t foundIndex2 = updatedCode.find('\n', foundIndex1 + 1);
	updatedCode.insert(foundIndex2, blockCode);
	return updatedCode;
}

std::string Trim(std::string str, std::string_view whitespace = " \t\n\r")
{
	auto strBegin = str.find_first_not_of(whitespace);

	if (strBegin == std::string::npos)
	{
		return "";
	}

	auto strEnd = str.find_last_not_of(whitespace);
	auto strRange = strEnd - strBegin + 1;

	auto trimmed = str;
	trimmed = trimmed.substr(strBegin, strRange);
	return trimmed;
}

std::vector<std::string> Split(const std::string &str, const std::string &sep, bool trim = false)
{
	std::unique_ptr<char[]> copy(new char[strlen(str.c_str()) + 1]);
	std::strcpy(copy.get(), str.c_str());

	std::vector<std::string> splitVector;
	auto current = std::strtok(copy.get(), sep.c_str());

	while (current != nullptr)
	{
		auto currentS = std::string(current);

		if (trim)
		{
			currentS = Trim(currentS);
		}

		splitVector.emplace_back(currentS);
		current = std::strtok(nullptr, sep.c_str());
	}

	return splitVector;
}

std::string ReplaceFirst(std::string str, std::string_view token, std::string_view to)
{
	const auto startPos = str.find(token);

	if (startPos == std::string::npos)
	{
		return str;
	}

	str.replace(startPos, token.length(), to);
	return str;
}

bool Contains(std::string_view str, std::string_view token)
{
	return str.find(token) != std::string::npos;
}

std::string RemoveAll(std::string str, char token)
{
	str.erase(remove(str.begin(), str.end(), token), str.end());
	return str;
}

std::string Shader::ProcessIncludes(const std::string& shaderCode)
{
	auto lines = Split(shaderCode, "\n", true);

	std::stringstream stream;

	for (const auto &line : lines)
	{
		if (Contains(line, "#include"))
		{
			std::string filename = ReplaceFirst(line, "#include", "");
			filename = RemoveAll(filename, '\"');
			filename = Trim(filename);

			auto fileLoaded = Files::Read(filename);

			if (!fileLoaded)
			{
				std::cout << "Shader Include could not be loaded: " << filename.c_str() << "\n";
				continue;
			}

			stream << "\n" << *fileLoaded << "\n";
			continue;
		}

		stream << line << "\n";
	}

	return stream.str();
}

EShLanguage GetEshLanguage(const VkShaderStageFlags &stageFlag)
{
	switch (stageFlag)
	{
	case VK_SHADER_STAGE_COMPUTE_BIT:
		return EShLangCompute;
	case VK_SHADER_STAGE_VERTEX_BIT:
		return EShLangVertex;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		return EShLangTessControl;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		return EShLangTessEvaluation;
	case VK_SHADER_STAGE_GEOMETRY_BIT:
		return EShLangGeometry;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return EShLangFragment;
	default:
		return EShLangCount;
	}
}

TBuiltInResource GetResources()
{
	TBuiltInResource resources = {};
	resources.maxLights = 32;
	resources.maxClipPlanes = 6;
	resources.maxTextureUnits = 32;
	resources.maxTextureCoords = 32;
	resources.maxVertexAttribs = 64;
	resources.maxVertexUniformComponents = 4096;
	resources.maxVaryingFloats = 64;
	resources.maxVertexTextureImageUnits = 32;
	resources.maxCombinedTextureImageUnits = 80;
	resources.maxTextureImageUnits = 32;
	resources.maxFragmentUniformComponents = 4096;
	resources.maxDrawBuffers = 32;
	resources.maxVertexUniformVectors = 128;
	resources.maxVaryingVectors = 8;
	resources.maxFragmentUniformVectors = 16;
	resources.maxVertexOutputVectors = 16;
	resources.maxFragmentInputVectors = 15;
	resources.minProgramTexelOffset = -8;
	resources.maxProgramTexelOffset = 7;
	resources.maxClipDistances = 8;
	resources.maxComputeWorkGroupCountX = 65535;
	resources.maxComputeWorkGroupCountY = 65535;
	resources.maxComputeWorkGroupCountZ = 65535;
	resources.maxComputeWorkGroupSizeX = 1024;
	resources.maxComputeWorkGroupSizeY = 1024;
	resources.maxComputeWorkGroupSizeZ = 64;
	resources.maxComputeUniformComponents = 1024;
	resources.maxComputeTextureImageUnits = 16;
	resources.maxComputeImageUniforms = 8;
	resources.maxComputeAtomicCounters = 8;
	resources.maxComputeAtomicCounterBuffers = 1;
	resources.maxVaryingComponents = 60;
	resources.maxVertexOutputComponents = 64;
	resources.maxGeometryInputComponents = 64;
	resources.maxGeometryOutputComponents = 128;
	resources.maxFragmentInputComponents = 128;
	resources.maxImageUnits = 8;
	resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	resources.maxCombinedShaderOutputResources = 8;
	resources.maxImageSamples = 0;
	resources.maxVertexImageUniforms = 0;
	resources.maxTessControlImageUniforms = 0;
	resources.maxTessEvaluationImageUniforms = 0;
	resources.maxGeometryImageUniforms = 0;
	resources.maxFragmentImageUniforms = 8;
	resources.maxCombinedImageUniforms = 8;
	resources.maxGeometryTextureImageUnits = 16;
	resources.maxGeometryOutputVertices = 256;
	resources.maxGeometryTotalOutputComponents = 1024;
	resources.maxGeometryUniformComponents = 1024;
	resources.maxGeometryVaryingComponents = 64;
	resources.maxTessControlInputComponents = 128;
	resources.maxTessControlOutputComponents = 128;
	resources.maxTessControlTextureImageUnits = 16;
	resources.maxTessControlUniformComponents = 1024;
	resources.maxTessControlTotalOutputComponents = 4096;
	resources.maxTessEvaluationInputComponents = 128;
	resources.maxTessEvaluationOutputComponents = 128;
	resources.maxTessEvaluationTextureImageUnits = 16;
	resources.maxTessEvaluationUniformComponents = 1024;
	resources.maxTessPatchComponents = 120;
	resources.maxPatchVertices = 32;
	resources.maxTessGenLevel = 64;
	resources.maxViewports = 16;
	resources.maxVertexAtomicCounters = 0;
	resources.maxTessControlAtomicCounters = 0;
	resources.maxTessEvaluationAtomicCounters = 0;
	resources.maxGeometryAtomicCounters = 0;
	resources.maxFragmentAtomicCounters = 8;
	resources.maxCombinedAtomicCounters = 8;
	resources.maxAtomicCounterBindings = 1;
	resources.maxVertexAtomicCounterBuffers = 0;
	resources.maxTessControlAtomicCounterBuffers = 0;
	resources.maxTessEvaluationAtomicCounterBuffers = 0;
	resources.maxGeometryAtomicCounterBuffers = 0;
	resources.maxFragmentAtomicCounterBuffers = 1;
	resources.maxCombinedAtomicCounterBuffers = 1;
	resources.maxAtomicCounterBufferSize = 16384;
	resources.maxTransformFeedbackBuffers = 4;
	resources.maxTransformFeedbackInterleavedComponents = 64;
	resources.maxCullDistances = 8;
	resources.maxCombinedClipAndCullDistances = 8;
	resources.maxSamples = 4;
	resources.limits.nonInductiveForLoops = true;
	resources.limits.whileLoops = true;
	resources.limits.doWhileLoops = true;
	resources.limits.generalUniformIndexing = true;
	resources.limits.generalAttributeMatrixVectorIndexing = true;
	resources.limits.generalVaryingIndexing = true;
	resources.limits.generalSamplerIndexing = true;
	resources.limits.generalVariableIndexing = true;
	resources.limits.generalConstantMatrixVectorIndexing = true;
	return resources;
}

VkShaderModule Shader::ProcessShader(const std::string& shaderCode, const VkShaderStageFlags &stageFlag)
{
	auto logicalDevice = Engine::Get()->GetGraphicManager()->GetLogicalDevice();

	EShLanguage language = GetEshLanguage(stageFlag);
	glslang::TProgram program;
	glslang::TShader shader(language);
	TBuiltInResource resources = GetResources();

	auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);
	messages = static_cast<EShMessages>(messages | EShMsgDebugInfo);

	const char *shaderSource = shaderCode.c_str();
	shader.setStrings(&shaderSource, 1);

	//shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, 110);
	//shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
	//shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);

	shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, 100);
	shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);
	shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

	const int defaultVersion = glslang::EShTargetOpenGL_450;

	if (!shader.parse(&resources, defaultVersion, false, messages))
	{
		std::cout << "%s\n", shader.getInfoLog();
		std::cout << "%s\n", shader.getInfoDebugLog();
		std::cout << "SPRIV shader compile failed!\n";
	}

	program.addShader(&shader);

	if (!program.link(messages) || !program.mapIO())
	{
		std::cout << "Error while linking shader program.\n";
	}

	program.buildReflection();
	//program.dumpReflection();

	for (uint32_t dim = 0; dim < 3; ++dim)
	{
		auto localSize = program.getLocalSize(dim);

		if (localSize > 1)
		{
			m_LocalSizes[dim] = localSize;
		}
	}

	for (int32_t i = program.getNumLiveUniformBlocks() - 1; i >= 0; i--)
	{
		LoadUniformBlock(program, stageFlag, i);
	}

	for (int32_t i = 0; i < program.getNumLiveUniformVariables(); i++)
	{
		LoadUniform(program, stageFlag, i);
	}

	for (int32_t i = 0; i < program.getNumLiveAttributes(); i++)
	{
		LoadVertexAttribute(program, stageFlag, i);
	}

	glslang::SpvOptions spvOptions;
#if defined(ACID_VERBOSE)
	spvOptions.generateDebugInfo = true;
	spvOptions.disableOptimizer = true;
	spvOptions.optimizeSize = false;
#else
	spvOptions.generateDebugInfo = false;
	spvOptions.disableOptimizer = false;
	spvOptions.optimizeSize = true;
#endif

	spv::SpvBuildLogger logger;
	std::vector<uint32_t> spirv;
	GlslangToSpv(*program.getIntermediate((EShLanguage)language), spirv, &logger, &spvOptions);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = spirv.size() * sizeof(uint32_t);
	createInfo.pCode = spirv.data();

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(*logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;
}

void Shader::IncrementDescriptorPool(std::map<VkDescriptorType, uint32_t>& descriptorPoolCounts,
	const VkDescriptorType& type)
{
	if (type == VK_DESCRIPTOR_TYPE_MAX_ENUM)
	{
		return;
	}

	auto it = descriptorPoolCounts.find(type);

	if (it != descriptorPoolCounts.end())
	{
		it->second++;
	}
	else
	{
		descriptorPoolCounts.emplace(type, 1);
	}
}

void Shader::LoadUniformBlock(const glslang::TProgram& program, const VkShaderStageFlags& stageFlag,
	const int32_t& i)
{
	for (auto &[uniformBlockName, uniformBlock] : m_UniformBlocks)
	{
		if (uniformBlockName == program.getUniformBlockName(i))
		{
			uniformBlock.m_StageFlags |= stageFlag;
			return;
		}
	}

	auto type = UniformBlock::Type::Uniform;

	if (strcmp(program.getUniformBlockTType(i)->getStorageQualifierString(), "buffer") == 0)
	{
		type = UniformBlock::Type::Storage;
	}

	if (program.getUniformBlockTType(i)->getQualifier().layoutPushConstant)
	{
		type = UniformBlock::Type::Push;
	}

	m_UniformBlocks.emplace(program.getUniformBlockName(i), UniformBlock(program.getUniformBlockBinding(i), program.getUniformBlockSize(i), stageFlag, type));
}

void Shader::LoadUniform(const glslang::TProgram& program, const VkShaderStageFlags& stageFlag, const int32_t& i)
{
	if (program.getUniformBinding(i) == -1)
	{
		auto uniformName = program.getUniformName(i);
		auto splitName = Split(uniformName, ".");

		if (splitName.size() > 1)
		{
			for (auto &[uniformBlockName, uniformBlock] : m_UniformBlocks)
			{
				if (uniformBlockName == splitName.at(0))
				{
					uniformBlock.m_Uniforms.emplace(ReplaceFirst(uniformName, splitName.at(0) + ".", ""),
						Uniform(program.getUniformBinding(i), program.getUniformBufferOffset(i), ComputeSize(program.getUniformTType(i)), program.getUniformType(i), false, false,
							stageFlag));
					return;
				}
			}
		}
	}

	for (auto &[uniformName, uniform] : m_Uniform)
	{
		if (uniformName == program.getUniformName(i))
		{
			uniform.m_StageFlags |= stageFlag;
			return;
		}
	}

	auto &qualifier = program.getUniformTType(i)->getQualifier();
	m_Uniform.emplace(program.getUniformName(i),
		Uniform(program.getUniformBinding(i), program.getUniformBufferOffset(i), -1, program.getUniformType(i), qualifier.readonly, qualifier.writeonly, stageFlag));
}

void Shader::LoadVertexAttribute(const glslang::TProgram& program, const VkShaderStageFlags& stageFlag,
	const int32_t& i)
{
	std::string name = program.getAttributeName(i);

	if (name.empty())
	{
		return;
	}

	for (const auto &[attributeName, attribute] : m_Attribute)
	{
		if (attributeName == name)
		{
			return;
		}
	}

	auto &qualifier = program.getAttributeTType(i)->getQualifier();
	m_Attribute.emplace(name, Attribute(qualifier.layoutSet, qualifier.layoutLocation, ComputeSize(program.getAttributeTType(i)), program.getAttributeType(i)));
}

int32_t Shader::ComputeSize(const glslang::TType* ttype)
{
	// glslang::TType::computeNumComponents is available but has many issues resolved in this method.
	int components = 0;

	if (ttype->getBasicType() == glslang::EbtStruct || ttype->getBasicType() == glslang::EbtBlock)
	{
		for (const auto &tl : *ttype->getStruct())
		{
			components += ComputeSize(tl.type);
		}
	}
	else if (ttype->getMatrixCols() != 0)
	{
		components = ttype->getMatrixCols() * ttype->getMatrixRows();
	}
	else
	{
		components = ttype->getVectorSize();
	}

	if (ttype->getArraySizes() != nullptr)
	{
		int32_t arraySize = 1;

		for (int32_t d = 0; d < ttype->getArraySizes()->getNumDims(); ++d)
		{
			auto dimSize = ttype->getArraySizes()->getDimSize(d);

			// This only makes sense in paths that have a known array size.
			if (dimSize != glslang::UnsizedArraySize)
			{
				arraySize *= dimSize;
			}
		}

		components *= arraySize;
	}

	return sizeof(float) * components;
}
}
