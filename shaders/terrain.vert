#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outNormal;

void main(void)
{
	gl_Position = vec4(inPosition.xyz, 1.0);
	outUV = inUV;
	outNormal = inNormal;
}