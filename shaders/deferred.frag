#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformScene
{
	mat4 view;
	vec3 cameraPosition;
	
	int lightCount;
	
	vec4 fogColor;
	float fogDensity;
	float fogGradient;
	vec4 directionLightDirection;
	vec4 directionLightColor;
} scene;

struct PointLight
{
	vec4 color;
	vec3 position;
	float radius;
};

layout(binding = 1) buffer BufferLights
{
	PointLight pointLights[];
} bufferLights;

layout(binding = 2) uniform sampler2D samplerPosition;
layout(binding = 3) uniform sampler2D samplerDiffuse;
layout(binding = 4) uniform sampler2D samplerNormal;
layout(binding = 5) uniform sampler2D samplerMaterial;
layout(binding = 6) uniform sampler2D samplerBRDF;
layout(binding = 7) uniform samplerCube samplerIrradiance;
layout(binding = 8) uniform samplerCube samplerPrefiltered;
layout(binding = 9) uniform sampler2D samplerSsao;

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

const float PI = 3.1415926535897932384626433832795f;

vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float sqr(float x)
{
	return x * x;
}

float attenuation(float Dl, float radius)
{
	if(radius <= 0.0f)
	{
		return 1.0f;
	}

	float lightInvRadius = 1 / radius;

	float distanceSquare = dot(Dl, Dl);
    float factor = distanceSquare * lightInvRadius * lightInvRadius;
    float smoothFactor = max(1.0 - factor * factor, 0.0);
    return (smoothFactor * smoothFactor) / max(distanceSquare, 1e-4);
}

float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0f) + 1.0f;
	return alpha2 / (PI * denom * denom);
}

float G_SchlocksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;
	float GL = dotNL / (dotNL * (1.0f - k) + k);
	float GV = dotNV / (dotNV * (1.0f - k) + k);
	return GL * GV;
}

vec3 F_Schlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 prefilteredReflection(vec3 R, float roughness)
{
	float lod = roughness * float(textureQueryLevels(samplerPrefiltered));
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = textureLod(samplerPrefiltered, R, lodf).rgb;
	vec3 b = textureLod(samplerPrefiltered, R, lodc).rgb;
	return mix(a, b, lod - lodf);
}

vec3 specularContribution(vec3 diffuse, vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	vec3 H = normalize(V + L);
	float dotNH = clamp(dot(N, H), 0.0f, 1.0f);
	float dotNV = clamp(dot(N, V), 0.0f, 1.0f);
	float dotNL = clamp(dot(N, L), 0.0f, 1.0f);
	
	vec3 color = vec3(0.0f);
	
	if(dotNL > 0.0f)
	{
		float D = D_GGX(dotNH, roughness);
		
		float G = G_SchlocksmithGGX(dotNL, dotNV, roughness);
		
		vec3 F = F_Schlick(dotNV, F0);
		vec3 spec = D * F * G / (4.0f * dotNL * dotNV  + 0.001f);
		vec3 kD = (vec3(1.0f) - F) * (1.0f - metallic);
		color += (kD * diffuse / PI + spec) * dotNL;
	}
	
	return color;
}

void main()
{
	outColor = scene.directionLightDirection;

	mat4 Inv = scene.view;
	Inv = inverse(Inv);

	vec3 screenPosition = texture(samplerPosition, inUV).rgb;
	vec3 worldPosition = vec3(Inv * vec4(screenPosition, 1.0));
	
	vec4 diffuse = texture(samplerDiffuse, inUV);
	vec3 normal = texture(samplerNormal, inUV).rgb;
	mat3 normalMatrix = transpose(inverse(mat3(scene.view)));
	normal = normal * normalMatrix;

	vec3 material = texture(samplerMaterial, inUV).rgb;
	float ssao = texture(samplerSsao, inUV).r;
	diffuse *= ssao;
	
	float metallic = material.r;
	float roughness = material.g;
	bool ignoreFog = material.b == (1.0f / 3.0f) || material.b == (3.0f / 3.0f);
	bool ignoreLighting = material.b == (2.0f / 3.0f) || material.b == (3.0f / 3.0f);
	
	vec3 N = normalize(normal);
	vec3 V = normalize(scene.cameraPosition - worldPosition);
	vec3 R = reflect(-V, N);
	
	if(!ignoreLighting && normal != vec3(0.0f))
	{
		vec3 F0 = vec3(0.04f);
		F0 = mix(F0, diffuse.rgb, metallic);
		vec3 Lo = vec3(0.0f);
		
		for(int i = 0; i < scene.lightCount; i++)
		{
			PointLight light = bufferLights.pointLights[i];
			vec3 L = light.position - worldPosition;
			float Dl = length(L);
			L /= Dl;
			Lo += attenuation(Dl, light.radius) * light.color.rgb * specularContribution(diffuse.rgb, L, V, N, F0, metallic, roughness);
		}
				
		vec2 brdf = texture(samplerBRDF, vec2(max(dot(N, V), 0.0f), roughness)).rg;
		vec3 reflection = prefilteredReflection(R, roughness).rgb;
		vec3 irradiance = texture(samplerIrradiance, N).rgb;
		
		vec3 albedo = irradiance * diffuse.rgb;
		
		vec3 F = F_SchlickR(max(dot(N, V), 0.0f), F0, roughness);
		
		vec3 specular = reflection * (F * brdf.r + brdf.g);
		
		vec3 kD = 1.0f - F;
		kD *= 1.0f - metallic;
		vec3 ambient = (kD * albedo + specular);

		// Directional light
		vec3 L = normalize(vec3(-scene.directionLightDirection));
		vec3 H = normalize(L + scene.cameraPosition);
		vec3 directionalDiffuse = vec3(scene.directionLightColor) * max(dot(N, L), 0.01) * specularContribution(diffuse.rgb, L, V, N, F0, metallic, roughness);

		vec3 color = ambient + directionalDiffuse + Lo;
		
		// Tone mapping
		color = Uncharted2Tonemap(color * 4.5f);
		color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	

		// Gamma correction
		color = color / (color + vec3(1.0));
		color = pow(color, vec3(1.0f / 2.2f));

		outColor = vec4(color, 1.0f);
	}
	else
	{
		outColor = vec4(diffuse.rgb, 1.0f);
	}
	
	if(!ignoreFog && normal != vec3(0.0f))
	{
		float fogFactor = exp(-pow(length(screenPosition.xyz) * scene.fogDensity, scene.fogGradient));
		fogFactor = clamp(fogFactor, 0.0f, 1.0f);
		outColor = mix(scene.fogColor, outColor, fogFactor);
	}
}