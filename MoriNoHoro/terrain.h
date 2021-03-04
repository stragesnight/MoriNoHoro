 #pragma once

#include <glm.hpp>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include "noise.h"

namespace MoriNoHoro
{
	// describes a point in a 3D world
	struct particle
	{
		glm::vec4 position;
		glm::vec4 color;

		particle() { }
		particle(glm::vec3 pos, glm::vec4 col, int i) : position({pos.x, pos.y, pos.z, (float)i}), color(col) { }
		particle(glm::vec4 pos, glm::vec4 col) : position(pos), color(col) { }
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
		perlinNoise perlin;

		int nParticles;
		int _nSizeOfParticles;
		GLuint _vbo;

#pragma endregion

#pragma region PUBLIC_METHODS
	public:
		void construct(int nMapSize, glm::vec2 vMapOffset);
		void draw();

#pragma endregion

#pragma region PRIVATE_METHODS
	private:
		void bufferData(bool bGenVbo);
		void bufferSubData();

#pragma endregion
	};
}
