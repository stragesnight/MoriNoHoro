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
		float ASPECT_RATIO;
		static const unsigned NUM_VERTS = 1000000;

		const glm::vec3 CAMERA_UP { 0.0f, 1.0f, 0.0f };
		const glm::vec3 CAMERA_FRONT { 0.0f, 0.0f, -1.0f };
		const float SPEED = 0.8f;
		const float SHIFT_SPEED = 2.5f;

		const float FOV = 90.0f;
		const float NEAR_PLANE = 0.1f;
		const float FAR_PLANE = 10000.0f;

		const glm::mat4 *IDENTITY = new glm::mat4(1.f);

		// window

		GLFWwindow *_window;

		int _framebufferWidth = 0;
		int _framebufferHeight = 0;

		// time

		std::chrono::steady_clock::time_point _tTotalTime;
		float _fTotalElapsedTime = 0.f;

		// openGL buffers 

		GLuint _vao;
		GLuint _vbo;

		// matrices

		glm::mat4 _modelMatrix;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;

		// camera

		glm::vec3 _vCameraPosition { 0.0f, 0.0f, 10.0f };

		// shader

		shader *_coreShader;
		
		// graphics

		std::vector<particle> particles;
		unsigned sizeOfParticles;

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

#pragma endregion
	};
}