#include "terrain.h"

namespace MoriNoHoro
{
#pragma region CONSTRUCTORS
	terrain::terrain(int nSeed) 
	{ 
		perlin = perlinNoise(nSeed);
	}
	terrain::~terrain()
	{
		_vParticles.clear();
	}

#pragma endregion

#pragma endregion

#pragma region PUBLIC_METHODS
	void terrain::construct(int nMapSize, glm::vec2 vMapOffset)
	{
		std::vector<float> noiseMap = perlin.noiseMap(nMapSize, 6, 1024.f, 0.4f, 4.f, vMapOffset);
		_vParticles.clear();
		_vParticles.reserve(nMapSize * nMapSize * 5);

		for (int x = 0; x < nMapSize; x++)
		{
			for (int y = 0; y < nMapSize; y++)
			{
				for (int i = 0; i < 16; i++)
				{
					if (rand() % 3 == 0)
						break;

					glm::vec3 vPos{ x / 64.0f, noiseMap[y * nMapSize + x] * 4.f - 1.f + (i / 16.f + (rand() / 400000.f)), y / 64.0f };
					glm::vec3 vCol{ i / 8.f, noiseMap[y * nMapSize + x], 1.f - noiseMap[y * nMapSize + x] };
					_vParticles.emplace_back(vPos, vCol, i);
				}
			}
		}

		_nSizeOfParticles = _vParticles.size() * sizeof(particle);
		nParticles = _vParticles.size();

		bufferData(true);
	}

	void terrain::draw()
	{
		glDrawArrays(GL_POINTS, 0, nParticles);
	}

#pragma endregion

#pragma region PRIVATE_METHODS
	void terrain::bufferData(bool bGenVbo = false)
	{
		if (bGenVbo)
		{
			// gereate and bind vbo
			glCreateBuffers(1, &_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		}

		// send data to buffer
		// draw modes:
		//		GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
		//		GL_STATIC_DRAW: the data is set only once and used many times.
		//		GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
		glBufferData(GL_ARRAY_BUFFER, _nSizeOfParticles, _vParticles.data(), GL_DYNAMIC_DRAW);
	}

	void terrain::bufferSubData()
	{

	}

#pragma endregion
}

