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

	shader(const char *cVertexFilename, const char *cFragmentFilename)
	{
		// init shaders to 0
		GLuint vertex = 0;
		GLuint fragment = 0;

		vertex = loadShader(GL_VERTEX_SHADER, cVertexFilename);
		fragment = loadShader(GL_FRAGMENT_SHADER, cFragmentFilename);

		this->linkProgram(vertex, fragment, 0);
	}

	shader(const char *cComputeFilename)
	{
		GLuint compute = loadShader(GL_COMPUTE_SHADER, cComputeFilename);
		this->linkProgram(0, 0, compute);
	}

	shader(const char *cVertexFilename, const char *cFragmentFilename, const char *cComputeFilename)
	{
		// init shaders to 0
		GLuint vertex = 0;
		GLuint fragment = 0;
		GLuint compute = 0;

		vertex = loadShader(GL_VERTEX_SHADER, cVertexFilename);
		fragment = loadShader(GL_FRAGMENT_SHADER, cFragmentFilename);
		compute = loadShader(GL_COMPUTE_SHADER, cComputeFilename);

		this->linkProgram(vertex, fragment, compute);
	}

	~shader() { glDeleteProgram(this->_programID); }

#pragma endregion

#pragma region PRIVATE_VARIABLES
private:
	GLuint _programID;
	const bool coutLocations = false;

#pragma endregion

#pragma region PUBLIC_METHODS
public:
	void use() 
	{ 
		glUseProgram(_programID);
	}
	void unuse() 
	{ 
		glUseProgram(0);
	}

	void setUniformMatrix4fv(const GLchar *cName, glm::mat4 mValue)
	{
		GLuint uniformLoc = glGetUniformLocation(this->_programID, cName);
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mValue));
		if (coutLocations)
			std::cout << "\nuniform loc " << uniformLoc;
	}

	void setUniform3fv(const GLchar *cName, glm::vec3 vValue)
	{
		GLuint uniformLoc = glGetUniformLocation(this->_programID, cName);
		glUniform3fv(uniformLoc, 1, glm::value_ptr(vValue));
		if (coutLocations)
			std::cout << "\nuniform loc " << uniformLoc;
	}

	void setUniform1f(const GLchar *cName, GLfloat fValue)
	{
		GLuint uniformLoc = glGetUniformLocation(this->_programID, cName);
		glUniform1f(uniformLoc, fValue);
		if (coutLocations)
			std::cout << "\nuniform loc " << uniformLoc;
	}

	void setUniform1i(const GLchar *cName, GLint nValue)
	{
		GLuint uniformLoc = glGetUniformLocation(this->_programID, cName);
		glUniform1i(uniformLoc, nValue);
		if (coutLocations)
			std::cout << "\nuniform loc " << uniformLoc;
	}

	void setVertexAttribPointer(const GLchar *cName, GLint nCount, GLenum eType, GLboolean _b, GLsizei size, GLvoid *offset)
	{
		GLuint attribLoc = glGetAttribLocation(this->_programID, cName);
		glVertexAttribPointer(attribLoc, nCount, eType, _b, size, offset);
		glEnableVertexAttribArray(attribLoc);
		if (coutLocations)
			std::cout << "\nattrib loc " << attribLoc;
	}

	void setVertexAttribPointer(GLint nLocation, GLint nCount, GLenum eType, GLboolean _b, GLsizei size, GLvoid *offset)
	{
		glVertexAttribPointer(nLocation, nCount, eType, _b, size, offset);
		glEnableVertexAttribArray(nLocation);
		if (coutLocations)
			std::cout << "\nattrib loc " << nLocation;
	}

#pragma endregion

#pragma region PRIVATE_METHODS
private:
	std::string loadShaderSource(const char *cFilename)
	{
		// line holder
		std::string sTemp = "";
		// file holder
		std::string sSrc = "";
		std::ifstream file;

		file.open(cFilename);

		// if shader file was successfully open
		if (file.is_open())
		{
			// add each line of file to src
			while (std::getline(file, sTemp))
				sSrc += sTemp + "\n";
		}
		else
			std::cout << "error at shader::loadShaderSource -> shader file \"" << cFilename << "\" could not be open.\n";

		file.close();

		return sSrc;
	}

	GLuint loadShader(GLenum eType, const char *cFilename)
	{
		// log for shader loading succession
		char cInfoLog[512];
		GLint success;

		// create shader
		GLuint shader = glCreateShader(eType);
		// load shader source file into shader and compile
		std::string sSrc = this->loadShaderSource(cFilename);
		const GLchar *cSrc = sSrc.c_str();
		glShaderSource(shader, 1, &cSrc, NULL);
		glCompileShader(shader);

		// check for compilation error
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, cInfoLog);
			std::cout << "error at shader::loadShader -> error compiling shader \"" << cFilename << "\".\n";
			std::cout << cInfoLog << std::endl;
		}

		return shader;
	}

	void linkProgram(GLuint vertex, GLuint fragment, GLuint compute)
	{
		// log for shader loading succession
		char cInfoLog[512];
		GLint success;

		this->_programID = glCreateProgram();

		if (vertex != 0)
			glAttachShader(this->_programID, vertex);
		if (fragment != 0)
			glAttachShader(this->_programID, fragment);
		if (compute != 0)
			glAttachShader(this->_programID, compute);

		glLinkProgram(this->_programID);

				// check linking errors
		glGetProgramiv(this->_programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->_programID, 512, NULL, cInfoLog);
			std::cout << "error at shader::linkProgram -> error linking program.\n";
			std::cout << cInfoLog << std::endl;
		}

		// end linking
		if (vertex != 0)
			glDeleteShader(vertex);
		if (fragment != 0)
			glDeleteShader(fragment);
		if (compute != 0)
			glDeleteShader(compute);

		glUseProgram(0);
	}

#pragma endregion
};