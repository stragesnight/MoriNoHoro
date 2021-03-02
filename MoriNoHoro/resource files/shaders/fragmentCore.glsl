#version 440

in vec3 vs_position;
in vec3 vs_color;
in float vs_depth;

out vec4 fs_color;

void main()
{
	//fs_color = vec4(cos((vs_position.z - vs_position.x) / 8.f) - 0.1f, vs_position.y + 0.5f,  0.5f - vs_position.y, vs_depth);
	fs_color = vec4(vs_color, vs_depth);
}