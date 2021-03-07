#include "gameEngine.h"

namespace MoriNoHoro
{
#pragma region CONSTRUCTORS

	gameEngine::gameEngine() { }
	gameEngine::~gameEngine() { }

#pragma endregion

#pragma region PRIVATE_VARIABLES


#pragma endregion

#pragma region PUBLIC_METHODS

	bool gameEngine::init()
	{
		initGLFW();
		configureGL();

		map = new terrain(4);

		generateMatrices();

		// generate and bind vao
		glCreateVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		map->construct(CHUNK_SIZE, NUM_CHUNKS, vMapOffset);

		// unbind vertex array
		glBindVertexArray(0);

		return true;
	}

	void gameEngine::run()
	{
		_tTotalTime = std::chrono::steady_clock::now();

		// elapsed time last frame
		float fDeltaTime = 0.;

		while (!glfwWindowShouldClose(_window))
		{

			// update current time
			_tTotalTime = std::chrono::steady_clock::now();

			// call update function
			gameEngine::onUpdate(fDeltaTime);

			// get current time
			std::chrono::steady_clock::time_point tEndFrame = std::chrono::steady_clock::now();

			// calculate elapsed frame time in seconds
			fDeltaTime = (std::chrono::duration_cast<std::chrono::milliseconds>(tEndFrame - _tTotalTime).count()) / 1000.f;

			// update total elapsed time
			_fTotalElapsedTime += fDeltaTime;
		}

		// end program
		delete map;
		glfwTerminate();
		return;
	}

#pragma endregion

#pragma region PRIVATE_METHODS

	void gameEngine::initGLFW()
	{
		glfwInit();

		// configure opengl window
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "MoriNoHoro", NULL, NULL);

		// get framebuffer and set viewport
		glfwGetFramebufferSize(_window, &_nFramebufferWidth, &_nFramebufferHeight);
		glViewport(0, 0, _nFramebufferWidth, _nFramebufferHeight);

		glfwMakeContextCurrent(_window);

		ASPECT_RATIO = (float)_nFramebufferWidth / (float)_nFramebufferHeight;

		// enable additional glew functions
		glewExperimental = GL_TRUE;

		// check for GLEW error
		if (glewInit() != GLEW_OK)
		{
			std::cout << "error at GameEngine::Init -> failed GLEW initialization.\n";
			glfwTerminate();
		}
	}

	void gameEngine::configureGL()
	{
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// polygon mode (GL_FILL, GL_LINE, GL_POINT)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		// enables per-vertex point size
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	}

	void gameEngine::generateMatrices()
	{
		glm::mat4 identity(1.f);
		// model matrix
		_mModelMatrix = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_mModelMatrix = glm::scale(_mModelMatrix, glm::vec3(1.f));

		// view matrix
		_mViewMatrix = glm::lookAt(_vCameraPosition, _vCameraPosition + _vCameraFront, CAMERA_UP);

		// projection matrix
		_mProjectionMatrix = glm::perspective(glm::radians(FOV), ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);

		// send data to gpu

		map->setUniforms(&_fTotalElapsedTime, &_mModelMatrix, &_mViewMatrix, &_mProjectionMatrix, &CHUNK_SIZE);

		glUseProgram(0);
	}

	void gameEngine::onUpdate(float fElapsedTime)
	{
		std::cout << "\nFPS: " << 1. / fElapsedTime;
		//std::cout << "\nSIZE OF PARTICLE " << sizeof(particle) << "\n\n";
		//std::cout << "\nCAMERA LOC { X = " << _vCameraPosition.x << "\tY = " << _vCameraPosition.y << "\tZ = " << _vCameraPosition.z << " }\n";

		// update input
		glfwPollEvents();

		handleInputs(fElapsedTime);

		// update matrices

		_mModelMatrix = glm::translate(_mModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_mModelMatrix = glm::scale(_mModelMatrix, glm::vec3(1.f));

		_mViewMatrix = glm::lookAt(_vCameraPosition, _vCameraPosition + _vCameraFront, CAMERA_UP);

		// clear screen & buffers
		glClearColor(0.6f, 0.6f, 0.89f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// pre-draw setup
		map->setUniforms(&_fTotalElapsedTime, &_mModelMatrix, &_mViewMatrix);

		glBindVertexArray(_vao);

		// draw
		map->draw(_bShouldAdvance);

		// end drawing
		glfwSwapBuffers(_window);
		glFlush();
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void gameEngine::handleInputs(float fElapsedTime)
	{
		if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(_window, GLFW_TRUE);

		float fTempSpeed = SPEED;
		bool move = glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS;

		if (move)
		{
			if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				fTempSpeed = SHIFT_SPEED;

			if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
				_vCameraPosition += _vCameraFront * fTempSpeed * fElapsedTime;
			if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
				_vCameraPosition -= _vCameraFront * fTempSpeed * fElapsedTime;
			if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
				this->rotateVectorY(-fElapsedTime, &_vCameraFront);
			if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
				this->rotateVectorY(fElapsedTime, &_vCameraFront);

			if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS)
				_vCameraPosition.y -= fTempSpeed * fElapsedTime;
			if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS)
				_vCameraPosition.y += fTempSpeed * fElapsedTime;

			_bShouldAdvance = glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS;


		//	if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS)
		//	{
		//		vMapOffset.x += fTempSpeed;
		//		generateRandomParticles();
		//	}

		//	if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		//	{
		//		vMapOffset.x -= fTempSpeed;
		//		generateRandomParticles();
		//	}

		//	if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
		//	{
		//		vMapOffset.y += fTempSpeed;
		//		generateRandomParticles();
		//	}

		//	if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		//	{
		//		vMapOffset.y -= fTempSpeed;
		//		generateRandomParticles();
		//	}
		}
		else
		{
			if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				this->rotateVectorX(-2.f * fElapsedTime, &_vCameraFront);
			}

			if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				this->rotateVectorX(2.f * fElapsedTime, &_vCameraFront);
			}
		}
	}

	void gameEngine::rotateVectorY(float degrees, glm::vec3 *vec)
	{
		float newX = cosf(degrees) * vec->x - sinf(degrees) * vec->z;
		float newZ = sinf(degrees) * vec->x + cosf(degrees) * vec->z;

		vec->x = newX;
		vec->z = newZ;
	}

	void gameEngine::rotateVectorX(float degrees, glm::vec3 *vec)
	{
		float newY = cosf(degrees) * vec->y - sinf(degrees) * vec->z;
		float newZ = sinf(degrees) * vec->y + cosf(degrees) * vec->z;

		vec->y = newY;
		vec->z = newZ;
	}

#pragma endregion
}
