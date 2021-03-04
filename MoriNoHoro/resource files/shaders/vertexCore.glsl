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
	float _x = position.x + ((sin(position.y + elapsedTime) * (position.w / (position.y + 1.f))) / 32.f);
	float _z = position.z + ((cos( position.x + position.y + elapsedTime) * (position.w / (position.y + 1.f))) / 32.f);

	vec4 modifiedPos = vec4(_x, position.y, _z, position.w);
	vec4 pos = projection_matrix * view_matrix * model_matrix * vec4(modifiedPos.xyz, 1.f);

	vs_position = modifiedPos;
	vs_color = color;

	gl_Position = pos;
	gl_PointSize = oneOverInverseLerp(-0.2f, 15.f, pos.z);
}