#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform UboObject 
{
	mat4 transform;
	vec4 baseColor;
	vec4 fogColor;
	vec2 fogLimits;
	float blendFactor;
} object;

layout(binding = 2) uniform samplerCube samplerColor;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inUVW;

layout(location = 0) out vec4 outColor;

void main() 
{
	vec3 cubemapColor = texture(samplerColor, inUVW).rgb;
	vec3 color = mix(object.baseColor.rgb, cubemapColor, object.blendFactor);

//	float fadeFactor = 1.0f - smoothstep(object.fogLimits.x, object.fogLimits.y, inPosition.y);
//	color = mix(color, object.fogColor.rgb, fadeFactor);
	
	outColor = vec4(color, 1.0f);
}