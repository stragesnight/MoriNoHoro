#include "terrain.h"

namespace MoriNoHoro
{
#pragma region CONSTRUCTORS
	terrain::terrain(int nSeed) 
	{ 
		perlin = perlinNoise(nSeed);

		_coreShader = new shader("resource files/shaders/vertexCore.glsl", "resource files/shaders/fragmentCore.glsl");
		_computeShader = new shader("resource files/shaders/cellularAutomataEpoch.glsl");
	}
	terrain::~terrain()
	{
		_vParticleStates.clear();
		_vParticleStatesProxy.clear();

		delete _coreShader;
		delete _computeShader;
	}

#pragma endregion

#pragma endregion

#pragma region PUBLIC_METHODS
	void terrain::construct(int nChunkSize, int nChunks, glm::vec3 vMapOffset)
	{
		_vParticleStates.clear();
		_vParticleStatesProxy.clear();

		glm::vec3 vMapSize{ nChunkSize, nChunkSize, nChunkSize };

		std::vector<float> noiseMap3D;

		noiseMap3D = perlin.noiseMap3D(vMapSize, 5, 512.f, 0.25f, 4.f, { 0.f, 0.f, 0.f });

		for (int z = 0; z < vMapSize.z; z++)
		{
			for (int y = 0; y < vMapSize.y; y++)
			{
				for (int x = 0; x < vMapSize.x; x++)
				{
					int index = (z * vMapSize.y * vMapSize.x) + (y * vMapSize.x) + x;
					float noiseValue = noiseMap3D[index];
					//noiseValue = (vMapSize.y / (float)y) * 0.15f + (noiseValue * 0.85f);

					unsigned state = noiseValue > 0.475f ? 1 : 0;

					_vParticleStates.push_back(state);
					_vParticleStatesProxy.push_back(state);
				}
			}
		}

		noiseMap3D.clear();

		_nSizeOfParticles = _vParticleStates.size() * sizeof(unsigned);
		nParticles = _vParticleStates.size();
		
		// state buffer
		glCreateBuffers(1, &_particleStateBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _particleStateBuffer);

		_coreShader->setVertexAttribPointer("cellState", 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned), 0);

		glBufferData(GL_ARRAY_BUFFER, _nSizeOfParticles, _vParticleStates.data(), GL_DYNAMIC_COPY);

		// proxy buffer
		glCreateBuffers(1, &_particleStateProxyBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, _particleStateProxyBuffer);

		glBufferData(GL_COPY_WRITE_BUFFER, _nSizeOfParticles, _vParticleStatesProxy.data(), GL_DYNAMIC_COPY);
	}

	void terrain::draw(bool advance)
	{
		if (advance)
		{
			_computeShader->use();

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _particleStateBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _particleStateProxyBuffer);

			glDispatchCompute(32, 32, 32);
			//glMemoryBarrier(GL_ALL_BARRIER_BITS);

			glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, _nSizeOfParticles);

			_computeShader->unuse();
		}

		_coreShader->use();

		glBindBuffer(GL_ARRAY_BUFFER, _particleStateBuffer);
		_coreShader->setVertexAttribPointer("cellState", 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned), 0);
		glDrawArrays(GL_POINTS, 0, nParticles);

		_coreShader->unuse();
	}

	void terrain::setUniforms(float *fTotalElapsedTime, glm::mat4 *mModel, glm::mat4 *mView, glm::mat4 *mProjection, int *nMapSize)
	{
		_coreShader->use();

		_coreShader->setUniform1f("elapsedTime", *fTotalElapsedTime);
		if (mModel != nullptr)
			_coreShader->setUniformMatrix4fv("model_matrix", *mModel);
		if (mView != nullptr)
			_coreShader->setUniformMatrix4fv("view_matrix", *mView);
		if (mProjection != nullptr)
			_coreShader->setUniformMatrix4fv("projection_matrix", *mProjection);
		if (nMapSize != 0)
			_coreShader->setUniform1i("mapSize", *nMapSize);

		_computeShader->use();
		if (nMapSize != 0)
			_computeShader->setUniform1i("mapSize", *nMapSize);
	}

#pragma endregion

#pragma region PRIVATE_METHODS
	void terrain::bufferData()
	{
		// send data to buffer
		// draw modes:
		//		GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
		//		GL_STATIC_DRAW: the data is set only once and used many times.
		//		GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
		glBufferData(GL_ARRAY_BUFFER, _nSizeOfParticles, _vParticleStates.data(), GL_DYNAMIC_COPY);
		//_vParticles.clear();
		std::cout << "\nBUFFERED " << _vParticleStates.size() << " PARTICLES\n";
	}

	void terrain::bufferSubData()
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, _nSizeOfParticles, _vParticleStates.data());
		std::cout << "\nSUB-BUFFERED " << _vParticleStates.size() << " PARTICLES\n";
	}

#pragma endregion
}

