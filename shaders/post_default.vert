#version 450

layout(location = 0) out vec2 outUV;

out gl_PerVertex
{
	vec4 glPosition;
};

void main()
{
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUv * 2.0f - 1.0f, 0.0f, 1.0f);
}