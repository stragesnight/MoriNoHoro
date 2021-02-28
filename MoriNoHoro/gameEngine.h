#pragma once

#include "dependencies.h"
#include <chrono>

namespace MoriNoHoro
{
	class gameEngine
	{
#pragma region CONSTRUCTORS
	public:
		// constructor
		gameEngine();
		// deconstructor
		~gameEngine();

#pragma endregion

#pragma region PUBLIC_VARIABLES
	public:
		int placeholder;

#pragma endregion

#pragma region PRIVATE_VARIABLES
	private:
		// constants

		static const int WINDOW_WIDTH = 1280;
		static const int WINDOW_HEIGHT = 720;
		static const unsigned NUM_VERTS = 800000;

		// window

		GLFWwindow *_window;

		int _framebufferWidth = 0;
		int _framebufferHeight = 0;

		// time

		std::chrono::steady_clock::time_point _tTotalTime;
		float _fTotalElapsedTime = 0.f;

		// openGL buffers and programs 

		GLuint _coreProgram;
		GLuint _vao;
		GLuint _vbo;

		// matrices

		glm::mat4 _modelMatrix;

		// graphics

		std::vector<glm::vertex> vertices;
		unsigned sizeOfVertices;

#pragma endregion

#pragma region PUBLIC_METHODS
	public:
		// initialize GameEngie
		bool init();
		// start update loop
		void run();

#pragma endregion

#pragma region PRIVATE_METHODS
	private:
		// initialize shader programs
		bool loadShaders(GLuint &program);
		// generate required glm::mat4 matrices
		void generateMatrices();
		// called once per frame to update game state
		void onUpdate(float fElapsedTime);
		void generateRandomVertices();

#pragma endregion
	};
}