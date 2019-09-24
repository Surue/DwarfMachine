#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UboScene
{
	mat4 projection;
	mat4 view;
	vec3 cameraPos;
} scene;

layout(binding = 1) uniform UboObject
{
	mat4 transform;
	vec4 baseDiffuse;
	float metallic;
	float roughness;
	float ignoreFog;
	float ignoreLighting;
} object;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	vec4 position = vec4(inPosition, 1.0f);
	vec4 normal = vec4(inNormal, 0.0f);

	vec4 worldPosition = object.transform * position;

	gl_Position = scene.projection * scene.view * worldPosition;

//	outPosition = vec3(worldPosition);
	outPosition = vec3(scene.view * worldPosition);
	outUV = inUV;
	// Normal in view space
	mat3 normalMatrix = transpose(inverse(mat3(scene.view * object.transform)));
//	mat3 normalMatrix = transpose(inverse(mat3(object.transform)));
	outNormal = normalMatrix * inNormal;
}
