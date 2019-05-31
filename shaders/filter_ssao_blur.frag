#version 450

layout (binding = 0) uniform sampler2D samplerSsao;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outSsao;

void main()
{
	const int blurRange = 2;
	int n = 0;
	vec2 texelSize = 1.0 / vec2(textureSize(samplerSsao, 0));
	float result = 0.0;
	for (int x = -blurRange; x < blurRange; x++) 
	{
		for (int y = -blurRange; y < blurRange; y++) 
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(samplerSsao, inUV + offset).r;
			n++;
		}
	}
	float color = result / (float(n));
	outSsao = vec4(color, color, color, 1.0f);
}