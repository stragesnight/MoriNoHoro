#version 440

layout (location = 0) in uint cellState;

out vec4 vs_color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform float elapsedTime;
uniform int mapSize;

float oneOverInverseLerp(float a, float b, float v)
{
	return (b - a) / (v - a);
}

void main()
{
	//float _x = position.x + ((sin(position.y + elapsedTime) * (position.w / (position.y + 2.f))) / 32.f);
	//float _y = position.y + (sin(position.y + elapsedTime) / (32.f - position.w + 1.f) * 4 - 2);
	//float _z = position.z + ((cos( position.x + position.y + elapsedTime) * (position.w / (position.y + 2.f))) / 32.f);

	int index = gl_VertexID;

	float mapZ = int(index / (mapSize * mapSize));
	float tmpMapY = int(index - (mapZ * mapSize * mapSize));
	float mapY = int(tmpMapY / mapSize);
	float mapX = int(tmpMapY - (mapY * mapSize));

	float _z = mapZ * 0.03125f;
	float _y = mapY * 0.03125f;
	float _x = mapX * 0.03125f;

	vec4 modifiedPos = vec4(_x, _y, _z, 1.f);
	vec4 pos = projection_matrix * view_matrix * model_matrix * modifiedPos;

	vec4 color = vec4(mapX / mapSize, mapY / mapSize, 1 - (mapZ / mapSize), 1.f);

	vs_color = (cellState >= 0.5f) ? color : vec4(1.f, 1.f, 1.f, 1.f);

	gl_Position = (cellState >= 0.5f) ? pos : vec4(0, 0, -2.f, 0);
	gl_PointSize = oneOverInverseLerp(-0.2f, 55.f, pos.z);
}