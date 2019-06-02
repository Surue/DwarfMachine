#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout(binding = 1) uniform sampler2D shadowMap;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out float outShadowMap;

void main()
{
	float depthValue = texture(shadowMap, inUV).r;
	outShadowMap = 0.5;
}