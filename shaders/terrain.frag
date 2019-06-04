#version 450

layout(binding = 0) uniform UboScene
{
	mat4 projection;
	mat4 view;
	vec3 cameraPos;
	mat4 transform;
} scene;


layout(binding = 1) uniform sampler2D samplerHeight;
layout (binding = 2) uniform sampler2D samplerGrass; 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

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
	
//	// Get height from displacement map
//	float height = textureLod(samplerHeight, inUV, 0.0).r * 255.0;
//	
//	for (int i = 0; i < 6; i++)
//	{
//		float range = layers[i].y - layers[i].x;
//		float weight = (range - abs(height - layers[i].y)) / range;
//		weight = max(0.0, weight);
//		color += weight * texture(samplerHeight, (inUV * 16.0)).rgb;
//	}

	return color;
}

void main()
{
	vec3 N = normalize(inNormal);
	vec3 ambient = vec3(0.5);
	vec3 normal = normalize(inNormal);
	vec4 color = vec4(ambient * sampleTerrainLayer(), 1.0);

	// Fragment coords
	vec2 xy = inUV.xy;
	// Tile coords
	vec2 phase = fract(xy / vec2(1, 1));

	vec3 position = inPosition;
//	position.y = texture(samplerHeight, inUV).r;

	outDiffuse = texture(samplerGrass, inUV);	
	outNormal = vec4(normalize(normal), 1.0f);
	outPosition = vec4(position, 1.0);
	outMaterial = vec4(0.0);
}