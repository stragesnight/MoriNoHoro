#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "shader.h"

namespace MoriNoHoro
{
#pragma region STRUCTS

	// describes a vertex in a 3D world
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
