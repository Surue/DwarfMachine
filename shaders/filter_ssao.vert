#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformScene
{
	float aspectRatio;
	float tanHalfFov;
} scene;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec2 ViewRay;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{ 
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
    
	ViewRay.x = gl_Position.x * scene.aspectRatio * scene.tanHalfFov;
    ViewRay.y = gl_Position.y * scene.tanHalfFov;
}