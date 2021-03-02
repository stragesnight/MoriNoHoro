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
			return false;
		}


		// opengl configuration

		glEnable(GL_DEPTH_TEST);
		// backface culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		// polygon mode (GL_FILL, GL_LINE, GL_POINT)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glPointSize(10.0f);
		// color blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// enables per-vertex point size
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		// load shaders
		//shader core("resource files/shaders/vertexCore.glsl", "resource files/shaders/fragmentCore.glsl");
		_coreShader = new shader("resource files/shaders/vertexCore.glsl", "resource files/shaders/fragmentCore.glsl");

		this->generateRandomParticles();

		this->generateMatrices();

		// vao, vbo

		// generate and bind vao
		glCreateVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// gereate and bind vbo
		glCreateBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		// send data to buffer
		// draw modes:
		//		GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
		//		GL_STATIC_DRAW: the data is set only once and used many times.
		//		GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
		glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(particle), particles.data(), GL_STREAM_DRAW);

		// generate and bind ebo
		//glGenBuffers(1, &_ebo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// set vertexAttribPointers (input assembly)

		_coreShader->setVertexAttribPointer("position", 3, GL_FLOAT, GL_FALSE, sizeof(particle), (GLvoid *)offsetof(particle, position));
		_coreShader->setVertexAttribPointer("color", 3, GL_FLOAT, GL_FALSE, sizeof(particle), (GLvoid *)offsetof(particle, color));
		_coreShader->setVertexAttribPointer("index", 1, GL_INT, GL_FALSE, sizeof(int), (GLvoid *)offsetof(particle, index));

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
		delete _coreShader;
		glfwTerminate();
		return;
	}

#pragma endregion

#pragma region PRIVATE_METHODS

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

		_coreShader->use();

		_coreShader->setUniformMatrix4fv("model_matrix", _mModelMatrix);
		_coreShader->setUniformMatrix4fv("view_matrix", _mViewMatrix);
		_coreShader->setUniformMatrix4fv("projection_matrix", _mProjectionMatrix);

		glUseProgram(0);
	}

	void gameEngine::generateRandomParticles()
	{
		//for (int i = 0; i < NUM_VERTS; i++)
		//{
		//	//particles[i] = vertex(glm::vec3(rand() / 16383.f - 1.f, rand() / 16383.f - 1.f, 0), glm::vec3(rand() / 32767.f, rand() / 32000.f, rand() / 32000.f));
		//	particles.emplace_back(glm::vec3(rand() / 16383.f-1.f, rand() / 16383.f-1.f, -rand() / 8192.f), glm::vec3(rand() / 32767.f, rand() / 32000.f, rand() / 32000.f), i);
		//}

		perlinNoise perlin(1);

		std::vector<float> noiseMap = perlin.noiseMap(MAP_SIZE, 6, 256.f, 0.2f, 4.f, vMapOffset);
		particles.clear();
		particles.resize(MAP_SIZE * MAP_SIZE);

		for (int x = 0; x < MAP_SIZE; x++)
		{
			for (int y = 0; y < MAP_SIZE; y++)
			{
				glm::vec3 vPos { x / 100.0f, noiseMap[y * MAP_SIZE + x] * 2.f - 1.f, y / 100.0f };
				glm::vec3 vCol { 0.1f, noiseMap[y * MAP_SIZE + x], 1.f - noiseMap[y * MAP_SIZE + x] };
				particles.emplace_back(vPos, vCol, y * MAP_SIZE + x);
			}
		}

		_nSizeOfParticles = particles.size() * sizeof(particle);
	}

	void gameEngine::onUpdate(float fElapsedTime)
	{
		nFrame++;
		std::cout << "\nFPS: " << 1. / fElapsedTime;

		// update input
		glfwPollEvents();

		this->handleInputs(fElapsedTime);

		// update matrices

		_mModelMatrix = glm::translate(_mModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		_mModelMatrix = glm::rotate(_mModelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_mModelMatrix = glm::scale(_mModelMatrix, glm::vec3(1.f));

		_mViewMatrix = glm::lookAt(_vCameraPosition, _vCameraPosition + _vCameraFront, CAMERA_UP);

		//for (int i = 0; i < 200000; i++)
		//{
		//	particles[i].position.x += (sinf(_fTotalElapsedTime + particles[i].position.y) + cosf(_fTotalElapsedTime - particles[i].position.y)) / ((i + 1.f) / 500.f);
		//	particles[i].position.y += (cosf(_fTotalElapsedTime - particles[i].position.x) - sinf(particles[i].position.x - _fTotalElapsedTime)) / ((i + 1.f) / 500.f);
		//	//particles[i].position.z += sinf(_fTotalElapsedTime + particles[i].position.z);
		//}

		// clear screen & buffers
		glClearColor(0.6f, 0.6f, 0.89f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// pre-draw setup
		_coreShader->use();

		// update uniforms
		_coreShader->setUniformMatrix4fv("model_matrix", _mModelMatrix);
		_coreShader->setUniformMatrix4fv("view_matrix", _mViewMatrix);
		//_coreShader.setUniformMatrix4fv("projection_matrix", _mProjectionMatrix);
		_coreShader->setUniform1f("elapsedTime", _fTotalElapsedTime);

		glBindVertexArray(_vao);

		// draw
		glBufferData(GL_ARRAY_BUFFER, _nSizeOfParticles, particles.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_POINTS, 0, particles.size());

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

			if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS)
			{
				vMapOffset.x += fTempSpeed;
				generateRandomParticles();
			}

			if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			{
				vMapOffset.x -= fTempSpeed;
				generateRandomParticles();
			}

			if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				vMapOffset.y += fTempSpeed;
				generateRandomParticles();
			}

			if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				vMapOffset.y -= fTempSpeed;
				generateRandomParticles();
			}
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
