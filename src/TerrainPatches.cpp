//  ========================================================================
//  COSC422: Advanced Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: TerrainPatches.cpp
//  Adapted from: COSC363: Computer Graphics Terrain Rendering Lab
//	Author: Kayle Ransby, 34043590
//
//	The program generates and loads the mesh data for a terrain floor (100 verts, 81 elems).
//  Requires files  TerrainPatches.vert, TerrainPatches.frag
//                  TerrainPatches.cont, TerrainPatches.eval
//					TerrainPatches.geom, Shader.h
//					loadTGA.h,			 Constants.h
//  ========================================================================
#define  GLM_FORCE_RADIANS
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"	   // loads shaders and generates shader program
#include "Constants.h" // contains call-back functions and globals
#include "loadTGA.h"   // for loading .tga textures
using namespace std;


//Loads terrain texture
void loadTextures()
{
	
	GLuint texID[6];
	string texNames[6] = { "Terrain_hm_01.tga", "Terrain_hm_02.tga", "water.tga", "grass.tga", "rock.tga", "snow.tga"};

    glGenTextures(6, texID);

	for (int i = 0; i < 6; i++)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, texID[i]);
		loadTGA("assets/" + texNames[i]);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}


//Initialise the shader program, create and load buffer data
void initialise()
{
	//--------Load terrain height map and textures-----------
	loadTextures();
	
	//--------Create Shader Program-------------
	GLuint program = createShaderProg(
		"src/shaders/TerrainPatches.vert", 
		"src/shaders/TerrainPatches.cont", 
		"src/shaders/TerrainPatches.eval", 
		"src/shaders/TerrainPatches.geom", 
		"src/shaders/TerrainPatches.frag"
	);

	//--------Compute matrices----------------------
	proj = glm::perspective(40.0f * CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
	mvMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); //view matrix
	mvpMatrix = proj * mvMatrix;  //Product matrix

	//--------Bind Uniform Variables-----------------
	// matrices
	mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	norMatrixLoc = glGetUniformLocation(program, "norMatrix");

	// controls
	eyePosLoc = glGetUniformLocation(program, "eyePos");
	waterLevelLoc = glGetUniformLocation(program, "waterLevel");
	snowLevelLoc = glGetUniformLocation(program, "snowLevel");
	fogEnabledLoc = glGetUniformLocation(program, "fogEnabled");
	wireFrameLoc = glGetUniformLocation(program, "wireframe");

	// light
	lgtLoc = glGetUniformLocation(program, "lightPos");
	glm::vec4 lightEye = mvMatrix * light;     //Light position in eye coordinates
	glUniform4fv(lgtLoc, 1, &lightEye[0]);

	// textures
	texLoc = glGetUniformLocation(program, "heightMap");
	glUniform1i(texLoc, 0);

	GLuint texLocW = glGetUniformLocation(program, "tSampler1");
	glUniform1i(texLocW, 2);

	GLuint texLocG = glGetUniformLocation(program, "tSampler2");
	glUniform1i(texLocG, 3);

	GLuint texLocR = glGetUniformLocation(program, "tSampler3");
	glUniform1i(texLocR, 4);

	GLuint texLocS = glGetUniformLocation(program, "tSampler4");
	glUniform1i(texLocS, 5);
	

	//---------Load buffer data-----------------------
	generateData();

	GLuint vboID[2];
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}


//Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{
	
	glm::mat4 mvMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); //Model-View matrix
	glm::mat4 mvpMatrix = proj * mvMatrix;   //The model-view-projection transformation
	glm::mat4 invMatrix = glm::inverse(mvMatrix);  //Inverse of model-view matrix for normal transformation

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &mvMatrix[0][0]);
	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here

	// Light rotation
	glm::mat4 rotate = glm::translate(mvMatrix, glm::vec3(0.0, 0.0, -50.0));
	rotate = glm::rotate(rotate, lgtAngle*CDR, glm::vec3(0.0, 1.0, 0.0));
	rotate = glm::translate(rotate, glm::vec3(0.0, 0.0, 50.0));
	glm::vec4 lightEye = rotate * light;     //Light position in eye coordinates
	glUniform4fv(lgtLoc, 1, &lightEye[0]);


	glUniform3f(eyePosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform1f(waterLevelLoc, waterLevel);
	glUniform1f(snowLevelLoc, snowLevel);
	glUniform1f(fogEnabledLoc, fogEnabled);
	glUniform1f(wireFrameLoc, wireframe);


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_PATCHES, 81*4, GL_UNSIGNED_SHORT, NULL);
	glPolygonMode(GL_FRONT_AND_BACK, wireframe == 0 ? GL_FILL : GL_LINE);
	glutPostRedisplay();
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 1280);
	glutCreateWindow("Assignment01");
	glutInitContextVersion (4, 2);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);
	glutMainLoop();
}

