#pragma once

#include "shader.h"
#include "noise.h"
#include <glm.hpp>

namespace MoriNoHoro
{
#pragma region STRUCTS

	// describes a point in a 3D world
	struct particle
	{
		glm::vec3 position;
		glm::vec3 color;
		int index;

		particle() { }

		particle(glm::vec3 pos, glm::vec3 col, int i) : position(pos), color(col) { index = i; }
	};

#pragma endregion
}

