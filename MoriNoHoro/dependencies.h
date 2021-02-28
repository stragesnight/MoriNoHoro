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

namespace glm
{
#pragma region STRUCTS

	// describes a vertex in a 3D world
	struct vertex
	{
		glm::vec3 position;
		glm::vec3 color;

		vertex() { }

		vertex(glm::vec3 pos, glm::vec3 col) : position(pos), color(col) { }
	};

	// describes a triangle containing 3 vertices
	struct triangle
	{
		vertex v[3];

		triangle()
		{
			for (int i = 0; i < 3; i++)
				v[i] = vertex();
		}

		triangle(vertex verts[3])
		{
			for (int i = 0; i < 3; i++)
				v[i] = verts[i];
		}

		triangle(vertex a, vertex b, vertex c)
		{
			v[0] = a;
			v[1] = b;
			v[2] = c;
		}
	};

	// describes a set of triangles
	struct mesh
	{
		std::vector<triangle> m;

		mesh() { m = { triangle() }; }

		mesh(std::vector<triangle> tris) { m = tris; }
	};

#pragma endregion
}
