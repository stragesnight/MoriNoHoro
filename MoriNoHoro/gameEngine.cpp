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
		glfwGetFramebufferSize(_window, &_framebufferWidth, &_framebufferHeight);
		glViewport(0, 0, _framebufferWidth, _framebufferHeight);

		glfwMakeContextCurrent(_window);

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

		// load shaders
		if (!loadShaders(_coreProgram))
			glfwTerminate();

		generateRandomVertices();

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
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vertex), vertices.data(), GL_STREAM_DRAW);

		// generate and bind ebo
		//glGenBuffers(1, &_ebo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// set vertexAttribPointers (input assembly)

		// position
		GLuint attribLoc = glGetAttribLocation(_coreProgram, "vertex_position");
		glVertexAttribPointer(attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vertex), (GLvoid *)offsetof(glm::vertex, position));
		glEnableVertexAttribArray(attribLoc);
		// color
		attribLoc = glGetAttribLocation(_coreProgram, "vertex_color");
		glVertexAttribPointer(attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vertex), (GLvoid *)offsetof(glm::vertex, color));
		glEnableVertexAttribArray(attribLoc);

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
			fDeltaTime = (std::chrono::duration_cast<std::chrono::milliseconds>(tEndFrame - _tTotalTime).count()) / 1000.;

			// update total elapsed time
			_fTotalElapsedTime += fDeltaTime;
		}

		// end program
		glfwTerminate();
		return;
	}

#pragma endregion

#pragma region PRIVATE_METHODS

	bool gameEngine::loadShaders(GLuint &program)
	{
		bool succeded = true;
		// log for shader loading succession
		char infoLog[512];
		GLint success;

		// line holder
		std::string temp = "";
		// file holder
		std::string src = "";
		std::ifstream file;

		// VERTEX SHADER

		file.open("resource files/shaders/vertexCore.glsl");

		// if vertex shader file was successfully open
		if (file.is_open())
		{
			// add each line of file to src
			while (std::getline(file, temp))
				src += temp + "\n";
		}
		else
		{
			std::cout << "error at GameEngine::Init::loadShader -> vertex shader file could not be open.\n";
			succeded = false;
		}

		file.close();

		// create vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		// load vertex shader source file into vertex shader and compile
		const GLchar *vertSrc = src.c_str();
		glShaderSource(vertexShader, 1, &vertSrc, NULL);
		glCompileShader(vertexShader);

		// check for compilation error
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "error at GameEngine::Init::loadShader -> error compiling vertex shader.\n";
			std::cout << infoLog << std::endl;

			succeded = false;
		}

		// reset variables for further use
		temp = "";
		src = "";

		// FRAGMENT SHADER

		file.open("resource files/shaders/fragmentCore.glsl");

		// if fragment shader file was successfully open
		if (file.is_open())
		{
			// add each line of file to src
			while (std::getline(file, temp))
				src += temp + "\n";
		}
		else
		{
			std::cout << "error at GameEngine::Init::loadShader -> fragment shader file could not be open.\n";
			succeded = false;
		}

		file.close();

		// create fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		// load fragment shader source file into fragment shader and compile
		const GLchar *fragSrc = src.c_str();
		glShaderSource(fragmentShader, 1, &fragSrc, NULL);
		glCompileShader(fragmentShader);

		// check for compilation error
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "error at GameEngine::Init::loadShader -> error compiling fragment shader.\n";
			std::cout << infoLog << std::endl;

			succeded = false;
		}

		// PROGRAM

		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		glLinkProgram(program);

		// check linking errors
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cout << "error at GameEngine::Init::loadShader -> error linking program.\n";
			std::cout << infoLog << std::endl;

			succeded = false;
		}

		// end loading
		glUseProgram(0);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return succeded;
	}

	void gameEngine::generateRandomVertices()
	{
		for (int i = 0; i < NUM_VERTS; i++)
		{
			//vertices[i] = glm::vertex(glm::vec3(rand() / 16383.f - 1.f, rand() / 16383.f - 1.f, 0), glm::vec3(rand() / 32767.f, rand() / 32000.f, rand() / 32000.f));
			vertices.emplace_back(glm::vec3(rand() / 16383.f - 1.f, rand() / 16383.f - 1.f, -rand() / 8192.f), glm::vec3(rand() / 32767.f, rand() / 32000.f, rand() / 32000.f));
		}

		sizeOfVertices = vertices.size() * sizeof(glm::vertex);
	}

	void gameEngine::onUpdate(float fElapsedTime)
	{
		std::cout << "\nFPS: " << 1. / fElapsedTime;

		// update input
		glfwPollEvents();

		// clear screen & buffers
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// pre-draw setup
		glUseProgram(_coreProgram);
		glBindVertexArray(_vao);

		// draw
		for (int i = 0; i < NUM_VERTS; i++)
		{
			vertices[i].position.x += sinf(_fTotalElapsedTime + vertices[i].position.x) / ((i + 1.f) / 50.f);
			//vertices[i].position.y += cosf(_fTotalElapsedTime + vertices[i].position.y) / ((i + 1.f) / 50.f);
			//vertices[i].position.z += tanf(_fTotalElapsedTime + vertices[i].position.z) / ((i + 1.f) / 5.f);
			//glPointSize(-vertices[i].position.z);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_POINTS, 0, vertices.size());

		// swap buffers
		glfwSwapBuffers(_window);
		glFlush();
	}

#pragma endregion
}
