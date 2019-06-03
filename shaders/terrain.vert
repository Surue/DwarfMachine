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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {

    // position patch in 2D grid based on instance ID
    int ix = gl_InstanceID % scene.gridW;
    int iy = gl_InstanceID / scene.gridH;

    outUV = vec2(float(ix) / float(scene.gridW), float(iy) / float(scene.gridH));

    //gl_Position = vertex;
    outPosition = scene.gridOrigin + vec3(float(ix)*scene.tileSize.x, 0, float(iy)*scene.tileSize.z);
    gl_Position = vec4(outPosition, 1.0f);
}