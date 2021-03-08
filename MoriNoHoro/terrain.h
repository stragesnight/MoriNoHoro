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
	class terrain
	{
#pragma region CONSTRUCTORS
	public:
		terrain() { };
		terrain(int nSeed);
		~terrain();

#pragma endregion

#pragma region PUBLIC_VARIABLES
	public:

#pragma endregion

#pragma region PRIVATE_VARIABLES
	private:
		std::vector<unsigned> _vParticleStates;
		std::vector<unsigned> _vParticleStatesProxy;

		perlinNoise perlin;

		int nParticles;
		int _nSizeOfParticles;

		shader *_coreShader = nullptr;
		shader *_computeShader = nullptr;

		GLuint _particleStateBuffer;
		GLuint _particleStateProxyBuffer;

#pragma endregion

#pragma region PUBLIC_METHODS
	public:
		void construct(glm::vec3 vMapSize, glm::vec3 vMapOffset);
		void draw(bool advance, glm::vec3 vMapSize);
		void setUniforms
		(
			float *fTotalElapsedTime,
			glm::mat4 *mModel = nullptr,
			glm::mat4 *mView = nullptr,
			glm::mat4 *mProjection = nullptr,
			glm::vec3 *vMapSize = nullptr
		);

#pragma endregion

#pragma region PRIVATE_METHODS
	private:
		void bufferData();
		void bufferSubData();

#pragma endregion
	};
}
