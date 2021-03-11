#version 440

layout( local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout (std430, binding = 0) buffer particleState { uint state[]; };
layout (std430, binding = 1) buffer particleStateProxy { uint proxy[]; };
layout (std430, binding = 2) buffer chunkEdgeBuffer { uint edge[]; };

uniform vec3 mapSize;
uniform uint nChunks;
uniform uint chunkIndex;


bool fEquals(uint v1, uint v2)
{
	return (v1 > v2 - 0.005f && v1 < v2 + 0.005f);
}

void main()
{
	uint nNeighbors = 0;
	
	uvec3 umapSize = uvec3(mapSize);

	uint index = (gl_GlobalInvocationID.y * umapSize.x * umapSize.z) + (gl_GlobalInvocationID.z * umapSize.x) + gl_GlobalInvocationID.x;
	
	uint mapSizeSquared = umapSize.x * umapSize.z;
	uint index2D = index % mapSizeSquared;

	// adjacent particle indices
	uint f = index + 1;
	uint b = index - 1;
	uint r = index + umapSize.x;
	uint l = index - umapSize.x;
	uint u = index + mapSizeSquared;
	uint d = index - mapSizeSquared;

	// adjacent chunk starting particle indices
	uint cC = chunkIndex * mapSizeSquared * 6;
	uint cF = (chunkIndex + 1) * mapSizeSquared * 6;
	uint cB = (chunkIndex - 1) * mapSizeSquared * 6;
	uint cR = (chunkIndex + nChunks) * mapSizeSquared * 6;
	uint cL = (chunkIndex - nChunks) * mapSizeSquared * 6;

	
	// front
	if (!fEquals(gl_GlobalInvocationID.x, umapSize.x - 1))
	{
		if (fEquals(proxy[f], 1))
			nNeighbors++;	
	}
	else if (fEquals(edge[cF + (f % mapSizeSquared)], 1))
		nNeighbors++;


	// back
	if (!fEquals(gl_GlobalInvocationID.x, 0))
	{
		if (fEquals(proxy[b], 1))
			nNeighbors++;	
	}
	else if (fEquals(edge[cB + mapSizeSquared + (b % mapSizeSquared)], 1))
		nNeighbors++;

	// right
	if (!fEquals(gl_GlobalInvocationID.z, umapSize.z - 1))
	{
		if (fEquals(proxy[r], 1))
			nNeighbors++;	
	}
	else if (fEquals(edge[cR + (mapSizeSquared * 2) + (r % mapSizeSquared)], 1))
		nNeighbors++;

	// left
	if (!fEquals(gl_GlobalInvocationID.z, 0))
	{
		if (fEquals(proxy[l], 1))
			nNeighbors++;	
	}
	else if (fEquals(edge[cL + (mapSizeSquared * 3) + (l % mapSizeSquared)], 1))
		nNeighbors++;


	// up
	if (!fEquals(gl_GlobalInvocationID.y, umapSize.y - 1))
	{
		if (fEquals(proxy[u], 1))
			nNeighbors++;	
	}

	// down
	if (!fEquals(gl_GlobalInvocationID.y, 0))
	{
		if (fEquals(proxy[d], 1))
			nNeighbors++;	
	}

	
	uint fl = f - umapSize.z;
	uint fr = f + umapSize.z;
	uint bl = b - umapSize.z;
	uint br = b + umapSize.z;
	uint ul = u - umapSize.z;
	uint ur = u + umapSize.z;
	uint dl = d - umapSize.z;
	uint dr = d + umapSize.z;
	
	// front left
	if (!fEquals(gl_GlobalInvocationID.x, umapSize.x - 1) && !fEquals(gl_GlobalInvocationID.z, 0))
	{
		if (fEquals(proxy[fl], 1))
			nNeighbors++;	
	}
	else if (fEquals(edge[cF + (fl % mapSizeSquared)], 1))
		nNeighbors++;

	// front right
	if (!fEquals(gl_GlobalInvocationID.x, umapSize.x - 1) && !fEquals(gl_GlobalInvocationID.z, umapSize.z - 1))
	{
		if (fEquals(proxy[fr], 1))
			nNeighbors++;	
	}
	else if (fEquals(edge[cF + (fr % mapSizeSquared)], 1))
		nNeighbors++;

	// back left
	if (!fEquals(gl_GlobalInvocationID.x, 0) && !fEquals(gl_GlobalInvocationID.z, 0))
	{
		if (fEquals(proxy[bl], 1))
			nNeighbors++;	
	}
	else if (fEquals(edge[cB + mapSizeSquared + (bl % mapSizeSquared)], 1))
		nNeighbors++;

	// back right
	if (!fEquals(gl_GlobalInvocationID.x, 0) && !fEquals(gl_GlobalInvocationID.z, umapSize.z - 1))
	{
		if (fEquals(proxy[br], 1))
			nNeighbors++;	
	}
	else if (fEquals(edge[cB + mapSizeSquared + (br % mapSizeSquared)], 1))
		nNeighbors++;

	// up left
	if (!fEquals(gl_GlobalInvocationID.y, umapSize.y - 1) && !fEquals(gl_GlobalInvocationID.z, 0))
	{
		if (fEquals(proxy[ul], 1))
			nNeighbors++;
	}

	// up right
	if (!fEquals(gl_GlobalInvocationID.y, umapSize.y - 1) && !fEquals(gl_GlobalInvocationID.z, umapSize.z - 1))
	{
		if (fEquals(proxy[ur], 1))
			nNeighbors++;	
	}

	// down left
	if (!fEquals(gl_GlobalInvocationID.y, 0) && !fEquals(gl_GlobalInvocationID.z, 0))
	{
		if (fEquals(proxy[dl], 1))
			nNeighbors++;	
	}

	// down right
	if (!fEquals(gl_GlobalInvocationID.y, 0) && !fEquals(gl_GlobalInvocationID.z, umapSize.z - 1))
	{
		if (fEquals(proxy[dr], 1))
			nNeighbors++;	
	}


	
	if (fEquals(state[index], 1))
		state[index] = (fEquals(nNeighbors, 4) || fEquals(nNeighbors, 5)) ? 1 : 0;
	else
		state[index] = (fEquals(nNeighbors, 3)) ? 1 : 0;


	if (fEquals(gl_GlobalInvocationID.x, umapSize.x - 1))			// front
		edge[cC + mapSizeSquared + index2D] = state[index];
	else if (fEquals(gl_GlobalInvocationID.x, 0))					// back
		edge[cC + index2D] = state[index];
	if (fEquals(gl_GlobalInvocationID.z, umapSize.z - 1))			// right
		edge[cC + (mapSizeSquared * 3) + index2D] = state[index];
	else if (fEquals(gl_GlobalInvocationID.z, 0))					// left
		edge[cC + (mapSizeSquared * 2) + index2D] = state[index];
}