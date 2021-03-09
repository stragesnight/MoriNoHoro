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
	void terrain::construct(int nChunks, glm::vec3 vMapSize, glm::vec3 vMapOffset)
	{
		_nChunks = nChunks;
		_vMapSize = vMapSize;

		_vChunks.clear();

		for (int z = 0; z < nChunks; z++)
		{
			for (int x = 0; x < nChunks; x++)
			{
				chunk c = chunk();
				c.construct(perlin, _coreShader, vMapSize, vMapOffset + glm::vec3(z * vMapSize.x, 0.f, x * vMapSize.z));
				_vChunks.push_back(c);
			}
		}
	}

	void terrain::draw(bool advance)
	{
		if (advance)
		{
			_computeShader->use();

			for (auto &chunk : _vChunks)
				chunk.compute(_vMapSize);

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
	void terrain::setUniforms(float *fTotalElapsedTime, glm::mat4 *mModel, glm::mat4 *mView, glm::mat4 *mProjection, glm::vec3 *vMapSize)
	{
		_coreShader->use();

		_coreShader->setUniform1f("elapsedTime", *fTotalElapsedTime);
		if (mModel != nullptr)
			_coreShader->setUniformMatrix4fv("model_matrix", *mModel);
		if (mView != nullptr)
			_coreShader->setUniformMatrix4fv("view_matrix", *mView);
		if (mProjection != nullptr)
			_coreShader->setUniformMatrix4fv("projection_matrix", *mProjection);
		if (vMapSize != nullptr)
			_coreShader->setUniform3fv("mapSize", *vMapSize);

		_computeShader->use();
		if (vMapSize != nullptr)
			_computeShader->setUniform3fv("mapSize", *vMapSize);
	}

#pragma endregion
}

