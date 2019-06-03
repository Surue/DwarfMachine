#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformScene
{
	float innerTessFactor;
	float outerTessFactor;

	float noiseFreq;
	int noiseOctave;
	float invNoiseSize;
	float heightScale;

	float triSize;
	vec4 viewport;

	mat4 ModelView;
    mat4 ModelViewProjection;
    mat4 Projection;
    mat4 InvProjection;
    mat4 InvView;

    vec4 eyePosWorld;

    vec4 frustumPlanes[6];

    int gridW, gridH;
    vec3 tileSize;
    vec3 gridOrigin;
    float tileBoundingSphereR;
} scene;

layout(binding = 1) uniform mediump sampler2D samplerNoise;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inVertexEye;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outDiffuse;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outMaterial;

float saturate(float v) {
    return clamp( v, 0.0, 1.0);
}

// cheaper than smoothstep
float linearstep(float a, float b, float x)
{
    return saturate((x - a) / (b - a));
}

float noise(vec2 p)
{
    return texture(samplerNoise, p * scene.invNoiseSize).x*2.0-1.0;
}

vec3 shadeTerrain(vec3 vertex,
                  vec3 vertexEye, 
                  vec3 normal
                  )
{
    const float shininess = 100.0;
    const vec3 ambientColor = vec3(0.05, 0.05, 0.15 );
    const float wrap = 0.3;

    vec3 rockColor = vec3(0.4, 0.4, 0.4 );
    vec3 snowColor = vec3(0.9, 0.9, 1.0 );
    vec3 grassColor = vec3(77.0 / 255.0, 100.0 / 255.0, 42.0 / 255.0 );
    vec3 brownColor = vec3(82.0 / 255.0, 70.0 / 255.0, 30.0 / 255.0 );
    vec3 waterColor = vec3(0.2, 0.4, 0.5 );
    vec3 treeColor = vec3(0.0, 0.2, 0.0 );

    //vec3 noisePos = vertex.xyz + vec3(translate.x, 0.0, translate.y);
    vec3 noisePos = vertex.xyz;
    float nois = noise(noisePos.xz)*0.5+0.5;

    float height = vertex.y;

    // snow
    float snowLine = 0.7;
    //float snowLine = 0.6 + nois*0.1;
    float isSnow = smoothstep(snowLine, snowLine+0.1, height * (0.5+0.5*normal.y));

    // world-space
    vec3 viewDir = normalize(scene.eyePosWorld.xyz - vertex);
    vec3 n = normalize(normal);

#if 0
    // add some noise variation to colors
    grassColor = mix(grassColor*0.5, grassColor*1.5, nois);
    brownColor = mix(brownColor*0.25, brownColor*1.5, nois);
#endif

    // choose material color based on height and normal

    vec3 matColor;
    matColor = mix(rockColor, grassColor, smoothstep(0.6, 0.8, normal.y));
    matColor = mix(matColor, brownColor, smoothstep(0.9, 1.0, normal.y ));
    // snow
    matColor = mix(matColor, snowColor, isSnow);

    float isWater = smoothstep(0.05, 0.0, height);
    matColor = mix(matColor, waterColor, isWater);

    vec3 finalColor = ambientColor*matColor;
        
    return finalColor;

    //return vec3(dist);
    //return vec3(dnoise(vertex.xz).z*0.5+0.5);
    //return normal*0.5+0.5;
    //return vec3(normal.y);
    //return vec3(fogCoord);
    //return vec3(diffuse);
    //return vec3(specular);
    //return diffuse*matColor + specular.xxx
    //return matColor;
    //return vec3(occ);
    //return vec3(sun)*sunColor;
    //return noise2*0.5+0.5;
}

void main()
{
    outDiffuse = vec4(shadeTerrain(inPosition.xyz, inVertexEye.xyz, inNormal), 1.0);    // shade per pixel
	outPosition = vec4(inPosition, 1.0f);
	outNormal = vec4(normalize(inNormal), 1.0f);
	outMaterial = vec4(0.0);
}