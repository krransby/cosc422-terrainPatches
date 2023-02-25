//  ========================================================================
//  COSC422: Computer Graphics (2014);  University of Canterbury.
//
//  FILE NAME: Shader.h
//  This file contains the essential functions for loading shader code.
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#ifndef SHADER_H
#define SHADER_H

// Loads a shader file and returns the reference to a shader object
GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

GLuint createShaderProg(string vertShader, string contShader, string evalShader, string geomShader, string fragShader)
{
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, vertShader);
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, contShader);
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, evalShader);
	GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, geomShader);
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, fragShader);

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);
	glAttachShader(program, shaderg);
	glAttachShader(program, shaderf);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
		program = 0;
	}
	glUseProgram(program);
	return program;
}

#endif