#version 440

layout (location = 0) in uint cellState;

out vec4 vs_color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform float elapsedTime;
uniform vec3 mapSize;

float oneOverInverseLerp(float a, float b, float v)
{
	return (b - a) / (v - a);
}

void main()
{
	int index = gl_VertexID;

	float numOfPoints = mapSize.x * mapSize.y * mapSize.z;

	float hwRatio = mapSize.x / mapSize.y; 

	float mapY = int(index / (mapSize.x * mapSize.z));
	float tmpMapZ = index - (mapY * mapSize.x * mapSize.z);
	float mapZ = int(tmpMapZ / mapSize.z);
	float mapX = int(tmpMapZ - (mapZ * mapSize.z));

	float _y = mapY / 8.f;
	float _z = mapZ / 8.f;
	float _x = mapX / 8.f;

	_y += sin(_y + elapsedTime) / 16.f;
	_z += (cos(_x + _y + elapsedTime) * (_z / (_y + 15.f))) / 24.f;
	_x += (sin(_y + elapsedTime) * (_z / (_y + 15.f))) / 24.f;

	vec4 modifiedPos = vec4(_x, _y, _z, 1.f);
	vec4 pos = projection_matrix * view_matrix * model_matrix * modifiedPos;

	vec4 color = vec4(mapX / mapSize.x, mapY / mapSize.y, 1 - (mapZ / mapSize.z), 1.f);

	vs_color = (cellState >= 0.5f) ? color : vec4(1.f, 1.f, 1.f, 1.f);

	gl_Position = (cellState >= 0.5f) ? pos : vec4(0, 0, -2.f, 0);
	gl_PointSize = oneOverInverseLerp(-0.2f, 111.f, pos.z);
}