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

#include <chrono>

#include "internal.h"


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

		static const int WINDOW_WIDTH = 1920;
		static const int WINDOW_HEIGHT = 1080;
		float ASPECT_RATIO;
		static const int MAP_SIZE = 1500;
		static const unsigned NUM_VERTS = MAP_SIZE * MAP_SIZE;

		const glm::vec3 CAMERA_UP { 0.0f, 1.0f, 0.0f };
		const float SPEED = 0.8f;
		const float SHIFT_SPEED = 2.5f;

		const float FOV = 70.0f;
		const float NEAR_PLANE = 0.1f;
		const float FAR_PLANE = 10000.0f;

		// window

		GLFWwindow *_window;

		int _nFramebufferWidth = 0;
		int _nFramebufferHeight = 0;

		// time

		std::chrono::steady_clock::time_point _tTotalTime;
		float _fTotalElapsedTime = 0.f;
		int nFrame = 0;

		// openGL buffers 

		GLuint _vao;
		GLuint _vbo;

		// matrices

		glm::mat4 _mModelMatrix;
		glm::mat4 _mViewMatrix;
		glm::mat4 _mProjectionMatrix;

		// camera

		glm::vec3 _vCameraPosition { 0.0f, 0.0f, 10.0f };
		glm::vec3 _vCameraFront{ 0.0f, 0.0f, -1.0f };

		// shader

		shader *_coreShader;
		
		// graphics

		std::vector<particle> particles;
		glm::vec2 vMapOffset { 0.0f, 0.0f };
		unsigned _nSizeOfParticles;

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
		// generate required glm::mat4 matrices
		void generateMatrices();
		// called once per frame to update game state
		void onUpdate(float fElapsedTime);
		void handleInputs(float fElapsedTime);
		void generateRandomParticles();
		void rotateVectorY(float degrees, glm::vec3 *vec);
		void rotateVectorX(float degrees, glm::vec3 *vec);

#pragma endregion
	};
}