#version 440

layout( local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout (std430, binding = 0) buffer particleState
{
	uint state[];
};

layout (std430, binding = 1) buffer particleStateProxy
{
	uint proxy[];
};

uniform vec3 mapSize;

bool fEquals(uint v1, uint v2)
{
	return (v1 > v2 - 0.005f && v1 < v2 + 0.005f);
}

void main()
{
	uint nNeighbors = 0;
	
	uint index = uint((gl_GlobalInvocationID.y * mapSize.x * mapSize.z) + (gl_GlobalInvocationID.z* mapSize.x) + gl_GlobalInvocationID.x);
	uint nParticleArraySize = uint(mapSize.x * mapSize.y * mapSize.z);
	
	uint f = index + 1;
	uint b = index - 1;
	uint u = uint(index + mapSize.x);
	uint d = uint(index - mapSize.x);
	uint l = uint(index + (mapSize.x * mapSize.z));
	uint r = uint(index - (mapSize.x * mapSize.z));
	
	// front
	if (f < nParticleArraySize - 1)
		if (proxy[f] >= 0.5f)
			nNeighbors++;
	// back
	if (b >= 0)
		if (proxy[b] >= 0.5f)
			nNeighbors++;
	// left
	if (l < nParticleArraySize - 1)
		if (proxy[l] >= 0.5f)
			nNeighbors++;
	// right
	if (r >= 0)
		if (proxy[r] >= 0.5f)
			nNeighbors++;
	// up
	if (u < nParticleArraySize - 1)
		if (proxy[u] >= 0.5f)
			nNeighbors++;
	// down
	if (d >= 0)
		if (proxy[d] >= 0.5f)
			nNeighbors++;
	
	uint fl = f + l - index;
	uint fr = f + r - index;
	uint bl = b + l - index;
	uint br = b + r - index;
	uint ul = u + l - index;
	uint ur = u + r - index;
	uint dl = d + l - index;
	uint dr = d + r - index;
	
	// front left
	if (fl > 0 && fl < nParticleArraySize - 1)
		if (proxy[fl] >= 0.5f)
			nNeighbors++;
	// front right
	if (fr > 0 && fr < nParticleArraySize - 1)
		if (proxy[fr] >= 0.5f)
			nNeighbors++;
	// back left
	if (bl > 0 && bl < nParticleArraySize - 1)
		if (proxy[bl] >= 0.5f)
			nNeighbors++;
	// back right
	if (br > 0 && br < nParticleArraySize - 1)
		if (proxy[br] >= 0.5f)
			nNeighbors++;
	// up left
	if (ul > 0 && ul < nParticleArraySize - 1)
		if (proxy[ul] >= 0.5f)
			nNeighbors++;
	// up right
	if (ur > 0 && ur < nParticleArraySize - 1)
		if (proxy[ur] >= 0.5f)
			nNeighbors++;
	// down left
	if (dl > 0 && dl < nParticleArraySize - 1)
		if (proxy[dl] >= 0.5f)
			nNeighbors++;
	// down right
	if (dr > 0 && dr < nParticleArraySize )
		if (proxy[dr] >= 0.5f)
			nNeighbors++;
	
	if (state[index] >= 0.5f)
		state[index] = (fEquals(nNeighbors, 4) || fEquals(nNeighbors, 5)) ? 1 : 0;
	else
		state[index] = (fEquals(nNeighbors, 3)) ? 1 : 0;
}