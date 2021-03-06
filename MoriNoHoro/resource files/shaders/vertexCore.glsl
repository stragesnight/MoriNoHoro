#version 440

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

out vec4 vs_position;
out vec4 vs_color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform float elapsedTime;

float oneOverInverseLerp(float a, float b, float v)
{
	return (b - a) / (v - a);
}

void main()
{
	float _x = position.x + ((sin(position.y + elapsedTime) * (position.w / (position.y + 2.f))) / 32.f);
	float _y = position.y + (sin(position.y + elapsedTime) / (32.f - position.w + 1.f) * 4 - 2);
	float _z = position.z + ((cos( position.x + position.y + elapsedTime) * (position.w / (position.y + 2.f))) / 32.f);

	//float _x = position.x;
	//float _y = position.y;
	//float _z = position.z;

	vec4 modifiedPos = vec4(_x, _y, _z, position.w);
	vec4 pos = projection_matrix * view_matrix * model_matrix * vec4(modifiedPos.xyz, 1.f);

	vs_position = modifiedPos;
	vs_color = color;

	gl_Position = color.a > 0.5f ? pos : vec4(0, 0, -2.f, 0);
	gl_PointSize = oneOverInverseLerp(-0.2f, 55.f, pos.z);
}