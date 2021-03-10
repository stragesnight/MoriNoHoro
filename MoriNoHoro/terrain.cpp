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
		_vChunks.clear();

		delete _coreShader;
		delete _computeShader;
	}

#pragma endregion

#pragma endregion

#pragma region PUBLIC_METHODS
	void terrain::construct()
	{
		_vChunks.clear();
		_vChunkEdges.clear();

		for (int z = 0; z < _nChunks; z++)
		{
			for (int x = 0; x < _nChunks; x++)
			{
				chunk c = chunk();
				c.construct(perlin, _coreShader, _vMapSize, _vMapOffset + glm::vec3(z * _vMapSize.x, 0.f, x * _vMapSize.z), &_vChunkEdges);
				_vChunks.push_back(c);
			}
		}

		glCreateBuffers(1, &_chunkEdgeBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, _chunkEdgeBuffer);
		glBufferData(GL_COPY_READ_BUFFER, _vChunkEdges.size() * sizeof(unsigned), _vChunkEdges.data(), GL_DYNAMIC_COPY);
	}

	void terrain::draw(bool advance)
	{
		if (advance)
		{
			_computeShader->use();

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _chunkEdgeBuffer);

			for (int i = 0; i < _vChunks.size(); i++)
			{
				_computeShader->setUniform1ui("chunkIndex", i);
				_vChunks[i].compute(_vMapSize);
			}

			_computeShader->unuse();
		}

		_coreShader->use();

		for (int z = 0; z < _nChunks; z++)
		{
			for (int x = 0; x < _nChunks; x++)
			{
				_vChunks[z * _nChunks + x].draw(_coreShader, { x * _vMapSize.x, 0.f, z * _vMapSize.z });
			}
		}

		_coreShader->unuse();
	}

#pragma endregion

#pragma region PRIVATE_METHODS
	void terrain::setUniforms(float *fTotalElapsedTime, glm::mat4 *mModel, glm::mat4 *mView, glm::mat4 *mProjection, bool setMapSize)
	{
		_coreShader->use();

		_coreShader->setUniform1f("elapsedTime", *fTotalElapsedTime);
		if (mModel != nullptr)
			_coreShader->setUniformMatrix4fv("model_matrix", *mModel);
		if (mView != nullptr)
			_coreShader->setUniformMatrix4fv("view_matrix", *mView);
		if (mProjection != nullptr)
			_coreShader->setUniformMatrix4fv("projection_matrix", *mProjection);
		if (setMapSize)
			_coreShader->setUniform3fv("mapSize", _vMapSize);

		_computeShader->use();
		if (setMapSize)
		{
			_computeShader->setUniform3fv("mapSize", _vMapSize);
			_computeShader->setUniform1ui("nChunks", _nChunks);
		}

	}

#pragma endregion
}

