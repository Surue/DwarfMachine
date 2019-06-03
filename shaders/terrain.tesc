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

layout(location = 1) in vec2 inUV[];
layout(location = 2) in vec3 inNormal[];

layout(location = 0) out vec2 outUV[];
layout(location = 1) out vec2 outTessLevelInner[];

in gl_PerVertex {
    vec4 gl_Position;
} gl_in[];


out gl_PerVertex {
    vec4 gl_Position;
} gl_out[];

// test if sphere is entirely contained within frustum planes
bool sphereInFrustum(vec3 pos, float r, vec4 plane[6])
{
    for(int i=0; i<6; i++) {
        if (dot(vec4(pos, 1.0), plane[i]) + r < 0.0) {
            // sphere outside plane
            return false;
        }
    }
    return true;
}

// transform from world to screen coordinates
vec2 worldToScreen(vec3 p)
{
    vec4 r = scene.ModelViewProjection * vec4(p, 1.0);   // to clip space
    r.xy /= r.w;            // project
    r.xy = r.xy*0.5 + 0.5;  // to NDC
    r.xy *= scene.viewport.zw;    // to pixels
    return r.xy;
}

// calculate edge tessellation level from two edge vertices in screen space
float calcEdgeTessellation(vec2 s0, vec2 s1)
{
    float d = distance(s0, s1);
    return clamp(d / scene.triSize, 1, 64);
}

vec2 eyeToScreen(vec4 p)
{
    vec4 r = scene.Projection * p;   // to clip space
    r.xy /= r.w;            // project
    r.xy = r.xy*0.5 + 0.5;  // to NDC
    r.xy *= scene.viewport.zw;    // to pixels
    return r.xy;
}

// calculate tessellation level by fitting sphere to edge
float calcEdgeTessellationSphere(vec3 w0, vec3 w1, float diameter)
{
    vec3 centre = (w0 + w1) * 0.5;
    vec4 view0 = scene.ModelView * vec4(centre, 1.0);
    vec4 view1 = view0 + vec4(diameter, 0, 0, 0);
    vec2 s0 = eyeToScreen(view0);
    vec2 s1 = eyeToScreen(view1);
    return calcEdgeTessellation(s0, s1);
}

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    outUV[gl_InvocationID] = inUV[gl_InvocationID];

    // bounding sphere for patch
    vec3 spherePos = gl_in[gl_InvocationID].gl_Position.xyz + vec3(scene.tileSize.x, scene.heightScale, scene.tileSize.z)*0.5;

    // test if patch is visible
    bool visible = sphereInFrustum(spherePos, scene.tileBoundingSphereR, scene.frustumPlanes);

    if (!visible) {
        // cull patch
        gl_TessLevelOuter[0] = -1;
        gl_TessLevelOuter[1] = -1;
        gl_TessLevelOuter[2] = -1;
        gl_TessLevelOuter[3] = -1;

        gl_TessLevelInner[0] = -1;
        gl_TessLevelInner[1] = -1;
    } else {
        
            // compute automatic LOD

            // calculate edge tessellation levels
            // see tessellation diagram in OpenGL 4 specification for vertex order details
            vec3 v0 = gl_in[0].gl_Position.xyz;
            vec3 v1 = v0 + vec3(scene.tileSize.x, 0, 0);
            vec3 v2 = v0 + vec3(scene.tileSize.x, 0, scene.tileSize.z);
            vec3 v3 = v0 + vec3(0, 0, scene.tileSize.z);
#if 0
            // use screen-space length of each edge
            // this method underestimates tessellation level when looking along edge
            vec2 s0 = worldToScreen(v0);
            vec2 s1 = worldToScreen(v1);
            vec2 s2 = worldToScreen(v2);
            vec2 s3 = worldToScreen(v3);

            gl_TessLevelOuter[0] = calcEdgeTessellation(s3, s0);
            gl_TessLevelOuter[1] = calcEdgeTessellation(s0, s1);
            gl_TessLevelOuter[2] = calcEdgeTessellation(s1, s2);
            gl_TessLevelOuter[3] = calcEdgeTessellation(s2, s3);
#else
            // use screen space size of sphere fit to each edge
            float sphereD = scene.tileSize.x*2.0f;
            gl_TessLevelOuter[0] = calcEdgeTessellationSphere(v3, v0, sphereD);
            gl_TessLevelOuter[1] = calcEdgeTessellationSphere(v0, v1, sphereD);
            gl_TessLevelOuter[2] = calcEdgeTessellationSphere(v1, v2, sphereD);
            gl_TessLevelOuter[3] = calcEdgeTessellationSphere(v2, v3, sphereD);
#endif

            // calc interior tessellation level - use average of outer levels
            gl_TessLevelInner[0] = 0.5 * (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]);
            gl_TessLevelInner[1] = 0.5 * (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]);

            outTessLevelInner[gl_InvocationID] = vec2(gl_TessLevelInner[0], gl_TessLevelInner[1]);
    }
}