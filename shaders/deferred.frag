#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformScene
{
	mat4 view;
	vec3 cameraPosition;
	
	int pointLightCount; 
	int spotLightCount;
	
	vec4 fogColor;
	float fogDensity;
	float fogGradient;
	vec4 directionalLightDirection;
	vec4 directionalLightColor;

	mat4 directionalLightViewMatrix;
} scene;

struct PointLight
{
	vec4 color;
	vec3 position;
	float radius;
};

struct SpotLight
{
	vec4 color;
	vec3 position;
	float range;
	vec3 target;
	float angle;
};

layout(binding = 1) buffer BufferPointLights
{
	PointLight pointLights[];
} bufferPointLights;

layout(binding = 2) buffer BufferSpotLights
{
	SpotLight spotLights[];
} bufferSpotLights;

layout(binding = 3) uniform sampler2D samplerPosition;
layout(binding = 4) uniform sampler2D samplerDiffuse;
layout(binding = 5) uniform sampler2D samplerNormal;
layout(binding = 6) uniform sampler2D samplerMaterial;
layout(binding = 7) uniform sampler2D samplerBRDF;
layout(binding = 8) uniform samplerCube samplerIrradiance;
layout(binding = 9) uniform samplerCube samplerPrefiltered;
layout(binding = 10) uniform sampler2D samplerSsao;
layout(binding = 11) uniform sampler2D shadowMap;

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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

float textureProj(vec4 P, vec2 offset)
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	shadowCoord.st = shadowCoord.st * 0.5 + 0.5;
	
	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0) 
	{
		float dist = texture(shadowMap, vec2(shadowCoord.st + offset)).r;
		if (shadowCoord.w > 0.0 && dist < shadowCoord.z) 
		{
			shadow = 0.25;
		}
	}
	return shadow;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(shadowMap, 0).xy;
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}

void main()
{
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
		
		for(int i = 0; i < scene.pointLightCount; i++)
		{
			PointLight light = bufferPointLights.pointLights[i];
			vec3 L = light.position - worldPosition;
			float Dl = length(L);
			L /= Dl;
			Lo += attenuation(Dl, light.radius) * light.color.rgb * specularContribution(diffuse.rgb, L, V, N, F0, metallic, roughness);
		}

		for(int i = 0; i < scene.spotLightCount; i++)
		{
			SpotLight light = bufferSpotLights.spotLights[i];
			vec3 L = light.position.xyz - worldPosition;
			float Dl = length(L);
			L /= Dl;
			
			float lightCosInnerAngle = cos(radians(light.angle * 0.5));
			float lightCosOuterAngle = cos(radians(light.angle));

			// Direction vector from source to target
			vec3 dir = normalize(light.position.xyz - light.target.xyz);

			// Dual cone spot light with smooth transition between inner and outer angle
			float cosDir = dot(L, dir);
			float spotEffect = smoothstep(lightCosOuterAngle, lightCosInnerAngle, cosDir);
			float heightAttenuation = smoothstep(light.range, 0.0f, Dl);

			Lo += spotEffect * heightAttenuation * light.color.rgb * specularContribution(diffuse.rgb, L, V, N, F0, metallic, roughness);
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
		vec3 L = normalize(vec3(-scene.directionalLightDirection));
		vec3 H = normalize(L + scene.cameraPosition);
		vec3 directionalDiffuse = vec3(scene.directionalLightColor) * max(dot(N, L), 0.01) * specularContribution(diffuse.rgb, L, V, N, F0, metallic, roughness);

		vec3 color = ambient + (directionalDiffuse + Lo);
		
		//shadow
		color *= filterPCF(scene.directionalLightViewMatrix * vec4(worldPosition, 1.0));

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