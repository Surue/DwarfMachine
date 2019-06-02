#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformScene 
{
	mat4 mvp;
} scene;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec2 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outUV;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	vec4 position = vec4(inPosition, 1.0f);

	gl_Position = scene.mvp * position;

	outPosition = vec3(scene.mvp * position);

	outUV = inUV;
}
