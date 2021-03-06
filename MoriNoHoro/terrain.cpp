#include "terrain.h"

namespace MoriNoHoro
{
#pragma region CONSTRUCTORS
	terrain::terrain(int nSeed) 
	{ 
		perlin = perlinNoise(nSeed);
		_coreShader = new shader("resource files/shaders/vertexCore.glsl", "resource files/shaders/fragmentCore.glsl");
	}
	terrain::~terrain()
	{
		_vParticles.clear();
		delete _coreShader;
	}

#pragma endregion

#pragma endregion

#pragma region PUBLIC_METHODS
	void terrain::construct(int nChunkSize, int nChunks, glm::vec3 vMapOffset)
	{
		_vParticles.clear();
		_vParticleStates.clear();
		//_vParticles.reserve(nChunkSize * nChunkSize * 5);

		glm::vec3 vMapSize{ nChunkSize, nChunkSize, nChunkSize };

		std::vector<float> noiseMap3D;

		for (int chunkX = 0; chunkX < nChunks; chunkX++)
		{
			for (int chunkZ = 0; chunkZ < nChunks; chunkZ++)
			{
				noiseMap3D = perlin.noiseMap3D(vMapSize, 5, 512.f, 0.25f, 4.f, { chunkZ * vMapSize.x, 0.f, chunkX * vMapSize.z });

				for (int z = 0; z < vMapSize.z; z++)
				{
					for (int y = 0; y < vMapSize.y; y++)
					{
						for (int x = 0; x < vMapSize.x; x++)
						{
							int index = (z * vMapSize.y * vMapSize.x) + (y * vMapSize.x) + x;
							float noiseValue = noiseMap3D[index];
							//noiseValue = (vMapSize.y / (float)y) * 0.01f + (noiseValue * 0.99f);
							//noiseValue = rand() % 2;

							//glm::vec4 pos{ (x + (chunkX * vMapSize.x) + (rand() / 32768.f)) / 8.f, (y + (rand() / 32768.f)) / 8.f, (z + (chunkZ * vMapSize.z) + (rand() / 32768.f)) / 8.f, rand() % 2 };
							glm::vec4 pos{ (x + (chunkX * vMapSize.x)) / 16.f, (y + (rand() / 32768.f)) / 16.f, (z + (chunkZ * vMapSize.z)) / 16.f, noiseValue };
							glm::vec4 color{ (x + (chunkX * vMapSize.x)) / ((float)vMapSize.x * nChunks), y / vMapSize.y, (z + (chunkZ * vMapSize.z)) / ((float)vMapSize.z * nChunks), noiseValue > 0.475f ? 1 : 0 };

							_vParticles.emplace_back(pos, color, index);
							_vParticleStates.push_back(color.a);
						}
					}
				}

				noiseMap3D.clear();
			}
		}

		_nSizeOfParticles = _vParticles.size() * sizeof(particle);
		nParticles = _vParticles.size();
		
		// create vbo
		glCreateBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		// input assembly
		_coreShader->setVertexAttribPointer("position", 4, GL_FLOAT, GL_FALSE, sizeof(particle), (GLvoid *)offsetof(particle, position));
		_coreShader->setVertexAttribPointer("color", 4, GL_FLOAT, GL_FALSE, sizeof(particle), (GLvoid *)offsetof(particle, color));

		bufferData();
	}

	void terrain::advance(int nEpochs, int nChunkSize)
	{
		for (int z = 0; z < nChunkSize; z++)
		{
			for (int y = 0; y < nChunkSize; y++)
			{
				for (int x = 0; x < nChunkSize; x++)
				{
					int nNeighbors = 0;

					int index = (z * nChunkSize * nChunkSize) + (y * nChunkSize) + x;

					int f = index + 1;
					int b = index - 1;
					int u = index + nChunkSize;
					int d = index - nChunkSize;
					int l = index + (nChunkSize * nChunkSize);
					int r = index - (nChunkSize * nChunkSize);

					// max neighbors - 32

					// front
					if (f < _vParticles.size() - 1)
						if (_vParticles[f].color.a > 0.5f)
							nNeighbors++;
					// back
					if (b >= 0)
						if (_vParticles[b].color.a > 0.5f)
							nNeighbors++;
					// left
					if (l < _vParticles.size() - 1)
						if (_vParticles[l].color.a > 0.5f)
							nNeighbors++;
					// right
					if (r >= 0)
						if (_vParticles[r].color.a > 0.5f)
							nNeighbors++;
					// up
					if (u < _vParticles.size() - 1)
						if (_vParticles[u].color.a > 0.5f)
							nNeighbors++;
					// down
					if (d >= 0)
						if (_vParticles[d].color.a > 0.5f)
							nNeighbors++;

					int fl = f + l - index;
					int fr = f + r - index;
					int bl = b + l - index;
					int br = b + r - index;
					int ul = u + l - index;
					int ur = u + r - index;
					int dl = d + l - index;
					int dr = d + r - index;

					// front left
					if (fl >= 0 && fl < _vParticles.size() - 1)
						if (_vParticles[fl].color.a > 0.5f)
							nNeighbors++;
					// front right
					if (fr >= 0 && fr < _vParticles.size() - 1)
						if (_vParticles[fr].color.a > 0.5f)
							nNeighbors++;
					// back left
					if (bl >= 0 && bl < _vParticles.size() - 1)
						if (_vParticles[bl].color.a > 0.5f)
							nNeighbors++;
					// back right
					if (br >= 0 && br < _vParticles.size() - 1)
						if (_vParticles[br].color.a > 0.5f)
							nNeighbors++;
					// up left
					if (ul >= 0 && ul < _vParticles.size() - 1)
						if (_vParticles[ul].color.a > 0.5f)
							nNeighbors++;
					// up right
					if (ur >= 0 && ur < _vParticles.size() - 1)
						if (_vParticles[ur].color.a > 0.5f)
							nNeighbors++;
					// down left
					if (dl >= 0 && dl < _vParticles.size() - 1)
						if (_vParticles[dl].color.a > 0.5f)
							nNeighbors++;
					// down right
					if (dr >= 0 && dr < _vParticles.size() - 1)
						if (_vParticles[dr].color.a > 0.5f)
							nNeighbors++;

					int flu = fl + u - index;
					int fld = fl + d - index;
					int fru = fr + u - index;
					int frd = fr + d - index;
					int blu = bl + u - index;
					int bld = bl + d - index;
					int bru = br + u - index;
					int brd = br + d - index;

					if (_vParticleStates[index] > 0.5f)
						_vParticleStates[index] = nNeighbors == 4;
					else
						_vParticleStates[index] = nNeighbors == 2 && rand() % 32 < 15;
				}
			}
		}

		for (int z = 0; z < nChunkSize; z++)
		{
			for (int y = 0; y < nChunkSize; y++)
			{
				for (int x = 0; x < nChunkSize; x++)
				{
					int index = (z * nChunkSize * nChunkSize) + (y * nChunkSize) + x;
					_vParticles[index].color.a = _vParticleStates[index];
				}
			}
		}

		_nSizeOfParticles = _vParticles.size() * sizeof(particle);
		nParticles = _vParticles.size();

		bufferData();
	}

	void terrain::draw()
	{
		glDrawArrays(GL_POINTS, 0, nParticles);
	}

	void terrain::setUniforms(float *fTotalElapsedTime, glm::mat4 *mModel, glm::mat4 *mView, glm::mat4 *mProjection)
	{
		_coreShader->use();

		_coreShader->setUniform1f("elapsedTime", *fTotalElapsedTime);
		if (mModel != nullptr)
			_coreShader->setUniformMatrix4fv("model_matrix", *mModel);
		if (mView != nullptr)
			_coreShader->setUniformMatrix4fv("view_matrix", *mView);
		if (mProjection != nullptr)
			_coreShader->setUniformMatrix4fv("projection_matrix", *mProjection);
	}

	void terrain::bufferData()
	{
		// send data to buffer
		// draw modes:
		//		GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
		//		GL_STATIC_DRAW: the data is set only once and used many times.
		//		GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
		glBufferData(GL_ARRAY_BUFFER, _nSizeOfParticles, _vParticles.data(), GL_DYNAMIC_DRAW);
		//_vParticles.clear();
		std::cout << "\nBUFFERED " << _vParticles.size() << " PARTICLES\n";
	}

#pragma endregion

#pragma region PRIVATE_METHODS

	void terrain::bufferSubData()
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, _nSizeOfParticles, _vParticles.data());
		std::cout << "\nSUB-BUFFERED " << _vParticles.size() << " PARTICLES\n";
	}

#pragma endregion
}

