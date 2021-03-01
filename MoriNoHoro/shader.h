#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <glew.h>
#include <glfw3.h>

class shader
{
#pragma region CONSTRUCTORS
public:
	shader() { }
	shader(const char *vertexFilename, const char *fragmentFilename)
	{
		// init shaders to 0
		GLuint vertex = 0;
		GLuint fragment = 0;

		vertex = loadShader(GL_VERTEX_SHADER, vertexFilename);
		fragment = loadShader(GL_FRAGMENT_SHADER, fragmentFilename);

		this->linkProgram(vertex, fragment);
	}

	~shader() { glDeleteProgram(this->_programID); }

#pragma endregion

#pragma region PRIVATE_VARIABLES
private:
	GLuint _programID;

#pragma endregion

#pragma region PUBLIC_METHODS
public:
	void use() { glUseProgram(_programID); }
	void unuse() { glUseProgram(0); }

	void setUniformMatrix4fv(const GLchar *name, glm::mat4 value)
	{
		GLuint uniformLoc = glGetUniformLocation(this->_programID, name);
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value));
		std::cout << "\nuniform loc " << uniformLoc;
	}

	void setUniformFloat(const GLchar *name, GLfloat value)
	{
		GLuint uniformLoc = glGetUniformLocation(this->_programID, name);
		glUniform1f(uniformLoc, value);
		std::cout << "\nuniform loc " << uniformLoc;
	}

	void setVertexAttribPointer(const GLchar *name, GLint count, GLenum type, GLboolean _b, GLsizei size, GLvoid *offset)
	{
		GLuint attribLoc = glGetAttribLocation(this->_programID, name);
		glVertexAttribPointer(attribLoc, count, type, _b, size, offset);
		glEnableVertexAttribArray(attribLoc);
		std::cout << "\nattrib loc " << attribLoc;
	}

	void setVertexAttribPointer(GLint location, GLint count, GLenum type, GLboolean _b, GLsizei size, GLvoid *offset)
	{
		glVertexAttribPointer(location, count, type, _b, size, offset);
		glEnableVertexAttribArray(location);
		std::cout << "\nattrib loc " << location;
	}

#pragma endregion

#pragma region PRIVATE_METHODS
private:
	std::string loadShaderSource(const char *filename)
	{
		// line holder
		std::string temp = "";
		// file holder
		std::string src = "";
		std::ifstream file;

		file.open(filename);

		// if shader file was successfully open
		if (file.is_open())
		{
			// add each line of file to src
			while (std::getline(file, temp))
				src += temp + "\n";
		}
		else
			std::cout << "error at shader::loadShaderSource -> shader file \"" << filename << "\" could not be open.\n";

		file.close();

		return src;
	}

	GLuint loadShader(GLenum type, const char *filename)
	{
		// log for shader loading succession
		char infoLog[512];
		GLint success;

		// create shader
		GLuint shader = glCreateShader(type);
		// load shader source file into shader and compile
		std::string strSrc = this->loadShaderSource(filename);
		const GLchar *src = strSrc.c_str();
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);

		// check for compilation error
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "error at shader::loadShader -> error compiling shader \"" << filename << "\".\n";
			std::cout << infoLog << std::endl;
		}

		return shader;
	}

	void linkProgram(GLuint vertex, GLuint fragment)
	{
		// log for shader loading succession
		char infoLog[512];
		GLint success;

		this->_programID = glCreateProgram();

		if (vertex)
			glAttachShader(this->_programID, vertex);
		if (fragment)
			glAttachShader(this->_programID, fragment);

		glLinkProgram(this->_programID);

				// check linking errors
		glGetProgramiv(this->_programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->_programID, 512, NULL, infoLog);
			std::cout << "error at shader::linkProgram -> error linking program.\n";
			std::cout << infoLog << std::endl;
		}

		// end linking
		if (vertex)
			glDeleteShader(vertex);
		if (fragment)
			glDeleteShader(fragment);
		glUseProgram(0);
	}

#pragma endregion
};