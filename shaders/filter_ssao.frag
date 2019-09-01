#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

const int SSAO_KERNEL_SIZE = 64;
const float SSAO_RADIUS = 0.3f;

layout(binding = 0) uniform UniformScene
{
	mat4 projection;
	vec3 cameraPosition;
} scene;

layout(binding = 1) uniform sampler2D samplerPosition;
layout(binding = 2) uniform sampler2D samplerNormal;
layout(binding = 3) uniform sampler2D samplerNoise;

layout(binding = 4, std140) uniform UniformKernel
{
	vec4 kernel[64]; 
} ssaoKernel;

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outSsao;

void main()
{
	//View space
	vec3 worldPosition = texture(samplerPosition, inUV).rgb;
	vec3 normal = texture(samplerNormal, inUV).rgb;

	//random pos
	ivec2 texDim = textureSize(samplerPosition, 0);
	ivec2 noiseDim = textureSize(samplerNoise, 0);
	vec2 noiseUv = vec2(float(texDim.x) / float(noiseDim.x), float(texDim.y) / (noiseDim.y)) * inUV;
	vec3 randomVec = texture(samplerNoise, noiseUv).rgb * 2.0f - 1.0f;

	//TBN Matrix
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0f;

	for(int i = 0; i < SSAO_KERNEL_SIZE; ++i)
	{
		vec3 samplePos = TBN * ssaoKernel.kernel[i].rgb;
		samplePos = samplePos * SSAO_RADIUS + worldPosition;

		vec4 offset = vec4(samplePos, 1.0f);
		offset = scene.projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float sampleDepth = texture(samplerPosition, offset.xy).z; 

		float rangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(worldPosition.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z ? 1.0f : 0.0f) * rangeCheck;
	}

	occlusion = 1.0f - (occlusion / float(SSAO_KERNEL_SIZE));

	outSsao = vec4(occlusion, occlusion, occlusion, 1.0f);
}