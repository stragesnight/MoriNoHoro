#version 440

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in int index;

out vec3 vs_position;
out vec3 vs_color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform float elapsedTime;

void main()
{
	float _x = position.x + (sin(elapsedTime - position.y) * position.y) - cos(position.x + index);
	float _y = position.y + (cos(elapsedTime - position.x) * position.y) + sin(position.y - index);

	vec4 pos = projection_matrix * view_matrix * model_matrix * vec4(_x, _y, position.z, 1.f);

	vs_position = pos.xyz;
	vs_color = color;

	gl_Position = pos;
	gl_PointSize = max(10.f - ((pos.z)), 0.1f);
}