 #pragma once

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include "noise.h"
#include "shader.h"

namespace MoriNoHoro
{
	class chunk
	{
#pragma region CONSTRUCTORS
	public:
		chunk() { };
		~chunk()
		{
			_vParticleStates.clear();
			_vParticleStatesProxy.clear();
		}

#pragma endregion

#pragma region PRIVATE_VARIABLES
	private:
		std::vector<unsigned> _vParticleStates;
		std::vector<unsigned> _vParticleStatesProxy;

		int nParticles;
		int _nSizeOfParticles;

		GLuint _particleStateBuffer;
		GLuint _particleStateProxyBuffer;

#pragma endregion

#pragma region PUBLIC_METHODS
	public:
		void construct(perlinNoise perlin, shader *coreShader, glm::vec3 vMapSize, glm::vec3 vMapOffset, std::vector<unsigned> *edgeBuffer)
		{
			_vParticleStates.clear();
			_vParticleStatesProxy.clear();

			std::vector<float> noiseMap3D;

			noiseMap3D = perlin.noiseMap3D(vMapSize, 5, 512.f, 0.25f, 4.f, vMapOffset);

			for (int y = 0; y < vMapSize.y; y++)
			{
				for (int z = 0; z < vMapSize.z; z++)
				{
					for (int x = 0; x < vMapSize.x; x++)
					{
						int index = (y * vMapSize.x * vMapSize.z) + (z * vMapSize.x) + x;

						float noiseValue = noiseMap3D[index];
						noiseValue = (1 - (y / (float)vMapSize.y)) * 0.25f + (noiseValue * 0.75f);

						unsigned state = noiseValue > 0.475f ? 1 : 0;

						_vParticleStates.push_back(state);
						_vParticleStatesProxy.push_back(state);

						// push state into the edge buffer if current particle is on edge
						if (x == 0 || x == vMapSize.x - 1 ||
							y == 0 || y == vMapSize.y - 1 ||
							z == 0 || z == vMapSize.z - 1)
							edgeBuffer->push_back(0);
					}
				}
			}

			noiseMap3D.clear();

			_nSizeOfParticles = _vParticleStates.size() * sizeof(unsigned);
			nParticles = _vParticleStates.size();

			// state buffer
			glCreateBuffers(1, &_particleStateBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, _particleStateBuffer);

			coreShader->setVertexAttribPointer("cellState", 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned), 0);

			glBufferData(GL_ARRAY_BUFFER, _nSizeOfParticles, _vParticleStates.data(), GL_DYNAMIC_COPY);

			_vParticleStates.clear();

			// proxy buffer
			glCreateBuffers(1, &_particleStateProxyBuffer);
			glBindBuffer(GL_COPY_WRITE_BUFFER, _particleStateProxyBuffer);

			glBufferData(GL_COPY_WRITE_BUFFER, _nSizeOfParticles, _vParticleStatesProxy.data(), GL_DYNAMIC_COPY);

			_vParticleStatesProxy.clear();
		}

		void compute(glm::vec3 vMapSize)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _particleStateBuffer);
			glBindBuffer(GL_COPY_WRITE_BUFFER, _particleStateProxyBuffer);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _particleStateBuffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _particleStateProxyBuffer);

			glDispatchCompute(vMapSize.x / 8, vMapSize.y / 8, vMapSize.z / 8);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

			glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, _nSizeOfParticles);
		}

		void draw(shader *coreShader, glm::vec3 offset)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _particleStateBuffer);

			coreShader->setVertexAttribPointer("cellState", 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned), 0);
			coreShader->setUniform3fv("offset", offset);

			glDrawArrays(GL_POINTS, 0, nParticles);
		}

#pragma endregion
	};


	class terrain
	{
#pragma region CONSTRUCTORS
	public:
		terrain() { };
		terrain(int nSeed);
		~terrain();

#pragma endregion

#pragma region PRIVATE_VARIABLES
	private:
		glm::vec3 _vMapSize { 128, 128, 128 };
		glm::vec3 _vMapOffset{ 0.f, 0.f, 0.f };
		int _nChunks = 4;

		perlinNoise perlin;

		std::vector<chunk> _vChunks;
		std::vector<unsigned> _vChunkEdges;

		shader *_coreShader = nullptr;
		shader *_computeShader = nullptr;

		GLuint _chunkEdgeBuffer;

#pragma endregion

#pragma region PUBLIC_METHODS
	public:
		void construct();
		void draw(bool advance);
		void setUniforms
		(
			float *fTotalElapsedTime,
			glm::mat4 *mModel = nullptr,
			glm::mat4 *mView = nullptr,
			glm::mat4 *mProjection = nullptr,
			bool setMapSize = false
		);

#pragma endregion
	};
}
