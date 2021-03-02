#version 440

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in int index;

out vec3 vs_position;
out vec3 vs_color;
out float vs_depth;

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
	float _y = position.y + (sin(position.y - position.x + elapsedTime) / 8.f);
	vec3 modifiedPos = vec3(position.x, _y, position.z);
	vec4 pos = projection_matrix * view_matrix * model_matrix * vec4(modifiedPos, 1.f);

	vs_position = modifiedPos;
	vs_color = color;
	vs_depth = oneOverInverseLerp(-0.2f, 10.f, pos.z);

	gl_Position = pos;
	gl_PointSize = oneOverInverseLerp(-0.2f, 15.f, pos.z);
}