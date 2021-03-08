#pragma once

#include <iostream>
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <chrono>

#include "terrain.h"


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
		// seems like 256x256x256 / 512x64x512 are optimal sizes, bigger numbers will overflow buffer
		glm::vec3 MAP_SIZE = { 256, 256, 256 };
		static const int NUM_CHUNKS = 1;

		const glm::vec3 CAMERA_UP { 0.0f, 1.0f, 0.0f };
		const float SPEED = 2.5f;
		const float SHIFT_SPEED = 7.75f;

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

		// openGL buffers 

		GLuint _vao;

		// matrices

		glm::mat4 _mModelMatrix;
		glm::mat4 _mViewMatrix;
		glm::mat4 _mProjectionMatrix;

		// camera

		//glm::vec3 _vCameraPosition { 21.94f, 3.17f, 29.0f };
		glm::vec3 _vCameraPosition { 15.f, 12.f, -15.f };
		glm::vec3 _vCameraFront{ 0.f, 0.f, 1.f };
		
		// graphics

		terrain *map;
		glm::vec3 vMapOffset { 0.f, 0.f, 0.f };

		bool _bShouldAdvance = false;
		float timer = 1.f;

#pragma endregion

#pragma region PUBLIC_METHODS
	public:
		// initialize GameEngie
		bool init(int seed);
		// start update loop
		void run();

#pragma endregion

#pragma region PRIVATE_METHODS
	private:
		void initGLFW();
		void configureGL();
		// generate required glm::mat4 matrices
		void generateMatrices();
		// called once per frame to update game state
		void onUpdate(float fElapsedTime);
		void handleInputs(float fElapsedTime);
		void rotateVectorY(float degrees, glm::vec3 *vec);
		void rotateVectorX(float degrees, glm::vec3 *vec);

#pragma endregion
	};
}