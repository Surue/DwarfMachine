#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#extension GL_ARB_tessellation_shader : enable

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

layout(quads, fractional_even_spacing, cw) in;

in gl_PerVertex {
    vec4 gl_Position;
} gl_in[];

layout(location = 0) in vec2 inUV[];
layout(location = 1) in vec3 inNormal[];
layout(location = 2) in vec2 inTessLevelInner[];

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outVertexEye;
layout(location = 2) out vec3 outNormal;

const mat2 rotateMat = mat2(1.6, -1.2, 1.2, 1.6);

vec2 fade(vec2 t)
{
    //return t * t * (3 - 2 * t); // old curve (quadratic)
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); // new curve (quintic)
}

// derivative of fade function
vec2 dfade(vec2 t)
{
    return 30.0*t*t*(t*(t-2.0)+1.0); // new curve (quintic)
}

vec3 dnoise(vec2 p)
{
    vec2 i = floor(p);
    vec2 u = p - i;

    vec2 du = dfade(u);
    u = fade(u);

    // get neighbouring noise values
    vec4 n = textureGather(samplerNoise, i * scene.invNoiseSize)*2.0-1.0;

    // rename components for convenience
    float a = n.w;
    float b = n.z;
    float c = n.x;
    float d = n.y;

    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k3 = a - b - c + d;

    vec3 r;
    // noise derivative
    r.xy = (vec2(k1, k2) + k3*u.yx) * du;
    // noise value
    r.z = k0 + k1*u.x + k2*u.y + k3*u.x*u.y;
    return r;
}

float fBmGrad(vec2 p, int octaves, float lacunarity, float gain)
{
    float amp = 0.5;
    vec2 d = vec2(0.0);
    float sum = 0.0;
    for(int i=0; i<octaves; i++) {
        vec3 n = dnoise(p);
        d += n.xy;
        sum += n.z*amp / (1.0 + dot(d, d));   // sum scaled by gradient
        amp *= gain;
        //p *= lacunarity;
        p = rotateMat * p;
    }
    return sum;
}

float terrain(vec2 p)
{
    p *= scene.noiseFreq;

    //float h = sin(p.x)*cos(p.y);
    //float h = texture2D(randTex, p * invNoiseSize).x;
    //float h = noise(p);
    //float h = fBm(p, noiseOctaves);
    //float h = fBm(p, noiseOctaves)*0.5+0.5;
    //float h = 0.6 - turbulence(p, noiseOctaves);
    //float h = ridgedMF(p, noiseOctaves);
    //float h = dnoise(p);
    //float h = hybridTerrain(p, noiseOctaves)-0.2;
    float h = fBmGrad(p, scene.noiseOctave, 2.0, 0.5) + 0.2;

    //h = 1.0 - h;
    //h = h*h;
    //h = sqrt(h);

    h *= scene.heightScale;

    const float waterLevel = 0.05;
    float land = smoothstep(waterLevel, waterLevel+0.2, h);
    // scale down terrain at shore
    h *= 0.1 + 0.9*land;

    return h;
}

void main(){

    vec3 pos = gl_in[0].gl_Position.xyz;
    pos.xz += gl_TessCoord.xy * scene.tileSize.xz;


    // calculate terrain height procedurally
    float h = terrain(pos.xz);
    vec3 n = vec3(0, 1, 0);
    pos.y = h;

    // calculate normal
    vec2 triSize = scene.tileSize.xz / inTessLevelInner[0];
    vec3 pos_dx = pos.xyz + vec3(triSize.x, 0.0, 0.0);
    vec3 pos_dz = pos.xyz + vec3(0.0, 0.0, triSize.y);
    pos_dx.y = terrain(pos_dx.xz);
    pos_dz.y = terrain(pos_dz.xz);
    n = normalize(cross(pos_dz - pos.xyz, pos_dx - pos.xyz));

    outNormal = n;

    outPosition = pos;
    outVertexEye = vec3(scene.ModelView * vec4(pos, 1));  // eye space

    gl_Position = scene.ModelViewProjection * vec4(pos, 1);
}