#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformScene
{
	mat4 view;
	vec3 cameraPosition;
	vec3 lightDir;
} scene;

layout(binding = 1, rgba8) uniform writeonly image2D writeColor;

layout(binding = 2) uniform sampler2D samplerColor;
layout(binding = 3) uniform sampler2D samplerPosition;

layout(location = 0) in vec2 inUV;

const vec3 sunColor = vec3(1.0, 1.0, 0.7);
const vec3 lightColor = vec3(1.0, 1.0, 0.7)*1.5;
const vec3 fogColor = vec3(0.7, 0.8, 1.0)*0.7;
const float fogExp = 0.05;

vec3 applyFog(vec3 col, float dist, vec3 viewDir)
{
//    float fogAmount = exp(-dist*fogExp);
//    float sunAmount = max(dot(viewDir, vec3(0.0, -1.0, -1.0)), 0.0);
//    sunAmount = pow(sunAmount, 8.0);
//    vec3 fogCol = mix(fogColor, sunColor, sunAmount);
//    return mix(fogCol, col, fogAmount);

    float fogAmount = exp(-dist*fogExp);
    return mix(fogColor, col, fogAmount);
}

void main() 
{
	mat4 Inv = scene.view;
	Inv = inverse(Inv);

	vec3 screenPosition = texture(samplerPosition, inUV).rgb;
	vec3 worldPosition = vec3(Inv * vec4(screenPosition, 1.0));

	vec3 vertexEye = vec3(scene.view * vec4(worldPosition, 1));

	float dist = length(vertexEye);

	vec3 color = texture(samplerColor, inUV).rgb;

	vec3 viewDir = normalize(scene.cameraPosition.xyz - worldPosition);

	color = applyFog(color, dist, viewDir);
	
//	float fadeFactor = 1.0f - smoothstep(100, -100, worldPosition.y);
//
//	color = mix(color, fogColor, fadeFactor);

	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), vec4(color, 1.0));
}