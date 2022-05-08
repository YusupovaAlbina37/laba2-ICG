﻿#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
using namespace glm;

GLuint VBO;
GLuint gWorldLocation;

//вершинный шейдер
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
     gl_Position = gWorld * vec4(Position, 1.0);                                    \n\
}";

//пиксельный шейдер
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
     FragColor = vec4(1.0, 0.0, 0.0, 1.0);                                          \n\
}";

void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f; 

	Scale += 0.001f; 

	//перемещение треугольника по координате X
	mat4x4 translate1;
	translate1[0][0] = 1.0f; translate1[0][1] = 0.0f; translate1[0][2] = 0.0f; translate1[0][3] = sinf(Scale);
	translate1[1][0] = 0.0f; translate1[1][1] = 1.0f; translate1[1][2] = 0.0f; translate1[1][3] = 0.0f;
	translate1[2][0] = 0.0f; translate1[2][1] = 0.0f; translate1[2][2] = 1.0f; translate1[2][3] = 0.0f;
	translate1[3][0] = 0.0f; translate1[3][1] = 0.0f; translate1[3][2] = 0.0f; translate1[3][3] = 1.0f;

	//вращение треугольника вокруг оси Z
	mat4x4 translate2;
	translate2[0][0] = cosf(Scale);  translate2[0][1] = -sinf(Scale);  translate2[0][2] = 0.0f; translate2[0][3] = 0.0f;
	translate2[1][0] = sinf(Scale);  translate2[1][1] = cosf(Scale);   translate2[1][2] = 0.0f; translate2[1][3] = 0.0f;
	translate2[2][0] = 0.0f;         translate2[2][1] = 0.0f;          translate2[2][2] = 1.0f; translate2[2][3] = 0.0f;
	translate2[3][0] = 0.0f;		 translate2[3][1] = 0.0f;          translate2[3][2] = 0.0f; translate2[3][3] = 1.0f;

	//преобразования масштаба треугольника
	mat4x4 translate3;
	translate3[0][0] = sinf(Scale); translate3[0][1] = 0.0f;        translate3[0][2] = 0.0f;        translate3[0][3] = 0.0f;
	translate3[1][0] = 0.0f;        translate3[1][1] = cosf(Scale); translate3[1][2] = 0.0f;        translate3[1][3] = 0.0f;
	translate3[2][0] = 0.0f;        translate3[2][1] = 0.0f;        translate3[2][2] = sinf(Scale); translate3[2][3] = 0.0f;
	translate3[3][0] = 0.0f;        translate3[3][1] = 0.0f;        translate3[3][2] = 0.0f;        translate3[3][3] = 1.0f;

	mat4x4 translate = translate1 * translate2 * translate3; //объединение преобразований

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &translate[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
	glutSwapBuffers();
}

static void InitializeGlutCallbacks() 
{
	glutDisplayFunc(RenderSceneCB); 
	glutIdleFunc(RenderSceneCB); 
}

static void CreateVertexBuffer()
{
	vec3 Vertices[3];
	Vertices[0] = vec3(-1.0f, -1.0f, 0.0f);
	Vertices[1] = vec3(1.0f, -1.0f, 0.0f);
	Vertices[2] = vec3(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) 
{
	GLuint ShaderObj = glCreateShader(ShaderType); 

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* p[1]; 
	p[0] = pShaderText;
	GLint Lengths[1]; 
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths); 

	glCompileShader(ShaderObj); 

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	} 

	glAttachShader(ShaderProgram, ShaderObj); 
}

static void CompileShaders() 
{
	GLuint ShaderProgram = glCreateProgram(); 

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}
	AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER); 
	AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER); 

	glLinkProgram(ShaderProgram); 

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	} 

	glValidateProgram(ShaderProgram); 

	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	} 

	glUseProgram(ShaderProgram); 

	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld"); 
	assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial 06");

	InitializeGlutCallbacks();

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	CreateVertexBuffer();

	CompileShaders();

	glutMainLoop();

	return 0;
}