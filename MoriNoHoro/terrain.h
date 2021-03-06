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
	// describes a point in a 3D world
	struct particle
	{
		glm::vec4 position;
		glm::vec4 color;
		int index;

		particle() { }
		particle(glm::vec4 pos, glm::vec4 col, int i) : position(pos), color(col) { index = i; }
	};


	// describes an array of particles forming primitives
	struct primitive
	{
		static std::vector<glm::vec4> segmentedCircle(int nSegments, float fRadius, int index, glm::vec4 origin, glm::vec3 vXyzRotation)
		{
			std::vector<glm::vec4> vOutput;
			vOutput.reserve(nSegments);

			const float fdeltaAngle = 360.f / (float)nSegments;
			// describes a point on unit sphere
			glm::vec4 point = { 0.f, 0.f, -1.f, 0.f };

			for (int s = 0; s < nSegments; s++)
			{
				// rotate point by fDeltaAngle degrees
				point = glm::rotate(glm::mat4(1.f), glm::radians(fdeltaAngle), { 0.f, 1.f, 0.f }) * point;

				glm::vec4 vToAdd{ point.x, point.y, point.z, 0.f };

				if (vXyzRotation != glm::vec3(1.f))
				{
					vToAdd = glm::rotate(glm::mat4(1.f), glm::radians(vXyzRotation.x), { 1.f, 0.f, 0.f }) * vToAdd;
					vToAdd = glm::rotate(glm::mat4(1.f), glm::radians(vXyzRotation.y), { 0.f, 1.f, 0.f }) * vToAdd;
					vToAdd = glm::rotate(glm::mat4(1.f), glm::radians(vXyzRotation.z), { 0.f, 0.f, 1.f }) * vToAdd;
				}

				vToAdd.w = index;
				vOutput.push_back(origin + (vToAdd * fRadius));
			}

			return vOutput;
		}
	};


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
		std::vector<particle> _vParticles;
		std::vector<unsigned short> _vParticleStates;
		perlinNoise perlin;

		int nParticles;
		int _nSizeOfParticles;

		shader *_coreShader = nullptr;
		GLuint _vbo;

#pragma endregion

#pragma region PUBLIC_METHODS
	public:
		void construct(int nChunkSize, int nChunks, glm::vec3 vMapOffset);
		void advance(int nEpochs, int nChunkSize);
		void draw();
		void setUniforms(float *fTotalElapsedTime, glm::mat4 *mModel = nullptr, glm::mat4 *mView = nullptr, glm::mat4 *mProjection = nullptr);
		void bufferData();
		void bufferSubData();

#pragma endregion

#pragma region PRIVATE_METHODS
	private:

#pragma endregion
	};
}
