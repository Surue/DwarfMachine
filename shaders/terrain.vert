#version 450

layout(binding = 0) uniform UboScene
{
	mat4 projection;
	mat4 view;
	vec3 cameraPos;
	mat4 transform;
} scene;

layout(binding = 1) uniform sampler2D samplerHeight;

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

void main(void)
{
	vec4 position = vec4(inPosition, 1.0f);
	vec4 normal = vec4(inNormal, 0.0f);
	position.y = texture(samplerHeight, inUV).r * 5;
	vec4 worldPosition = scene.transform * position;
//	worldPosition.y = texture(samplerHeight, inUV).r * 5;

	gl_Position = scene.projection * scene.view * worldPosition;

	outPosition = vec3(scene.view * worldPosition);
	outUV = inUV;
	// Normal in view space
	mat3 normalMatrix = transpose(inverse(mat3(scene.view * scene.transform)));
//	mat3 normalMatrix = transpose(inverse(mat3(object.transform)));
	outNormal = normalMatrix * inNormal;
}