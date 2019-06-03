#version 450

layout(binding = 3) uniform UboScene
{
	mat4 projection;
	mat4 view;
	vec3 cameraPos;
} scene;

layout (binding = 1) uniform sampler2D samplerHeight; 

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inViewVec;
layout (location = 3) in vec3 inEyePos;
layout (location = 4) in vec3 inWorldPos;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outDiffuse;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outMaterial;

vec3 sampleTerrainLayer()
{
	// Define some layer ranges for sampling depending on terrain height
	vec2 layers[6];
	layers[0] = vec2(-10.0, 10.0);
	layers[1] = vec2(5.0, 45.0);
	layers[2] = vec2(45.0, 80.0);
	layers[3] = vec2(75.0, 100.0);
	layers[4] = vec2(95.0, 140.0);
	layers[5] = vec2(140.0, 190.0);

	vec3 color = vec3(0.0);
	
	// Get height from displacement map
	float height = textureLod(samplerHeight, inUV, 0.0).r * 255.0;
	
	for (int i = 0; i < 6; i++)
	{
		float range = layers[i].y - layers[i].x;
		float weight = (range - abs(height - layers[i].y)) / range;
		weight = max(0.0, weight);
		color += weight * texture(samplerHeight, (inUV * 16.0)).rgb;
	}

	return color;
}

void main()
{
	vec3 N = normalize(inNormal);
	vec3 ambient = vec3(0.5);

	vec4 color = vec4(ambient * sampleTerrainLayer(), 1.0);

	outDiffuse = color;	
	outNormal = vec4(inNormal, 1.0);
	outPosition = scene.view * vec4(inWorldPos, 1.0);
	outMaterial = vec4(0.0);
}
