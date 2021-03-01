#version 440

in vec3 vs_position;
in vec3 vs_color;

out vec4 fs_color;

void main()
{
	float alpha = 1.f / (abs(vs_position.z) / 4.f);
	fs_color = vec4(vs_color, alpha);
}