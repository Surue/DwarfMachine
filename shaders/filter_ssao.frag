#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

const int SSAO_KERNEL_SIZE = 64;
const float SSAO_RADIUS = 0.5f;

layout(binding = 0) uniform UniformScene
{
	vec3 kernel[64];
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
} scene;

layout(binding = 1) uniform sampler2D samplerPosition;
layout(binding = 2) uniform sampler2D samplerNormal;
layout(binding = 3) uniform sampler2D samplerNoise;
layout(binding = 4) uniform sampler2D samplerDepth;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec2 ViewRay;


layout(location = 0) out vec4 outSsao;

void main()
{
	vec3 worldPosition = texture(samplerPosition, inUV).rgb;
//	worldPosition = (scene.view * vec4(worldPosition, 1.0f)).rgb;

	vec3 normal = texture(samplerNormal, inUV).rgb;

//	mat3 normalMatrix = transpose(inverse(mat3(scene.view)));
//	normal = normalMatrix * normal;
//	normal = normalize(normal) * 0.5 + 0.5;

	ivec2 texDim = textureSize(samplerPosition, 0);
	ivec2 noiseDim = textureSize(samplerNoise, 0);
	vec2 noiseUv = vec2(float(texDim.x) / float(noiseDim.x), float(texDim.y) / (noiseDim.y)) * inUV;
	vec3 randomVec = texture(samplerNoise, noiseUv).rgb * 2.0f - 1.0f;

	//TBN Matrix
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0f;

	for(int i = 0; i < 32; i++)
	{
		vec3 samplePos = TBN * scene.kernel[i];
		samplePos = samplePos * SSAO_RADIUS + worldPosition;

		vec4 offset = vec4(samplePos, 1.0f);
		offset = scene.projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float sampleDepth = texture(samplerPosition, offset.xy).z; 

		float rangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(worldPosition.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z ? 1.0f : 0.0f);
	}

	occlusion = 1.0f - (occlusion / float(32));

	outSsao = vec4(occlusion, occlusion, occlusion, 1.0f);

//	outSsao = vec4(normal, 1.0);
}