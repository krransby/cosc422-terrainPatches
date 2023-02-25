#pragma once

using namespace std;

#ifndef CONSTANTS_H
#define CONSTANTS_H

//-----------Globals--------------
GLuint vaoID;
GLuint mvMatrixLoc, mvpMatrixLoc, norMatrixLoc;
float CDR = 3.14159265 / 180.0;     //Conversion from degrees to rad (required in GLM 0.9.6)
float verts[100 * 3];				//10x10 grid (100 vertices)
GLushort elems[81 * 4];				//Element array for 81 quad patches
glm::mat4 proj, mvMatrix, mvpMatrix;		//Matrices


// Camera controls
float yaw = -90.0f;
float pitch = 0.0f;

// Vectors for camera position (view matrix)
glm::vec3 cameraPos = glm::vec3(0.0, 20.0, 30.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -40.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

// Light position
glm::vec4 light = glm::vec4(50.0, 30.0, 0, 1.0);
float lgtAngle = 0;

// extra uniform variables
GLuint eyePosLoc, waterLevelLoc, snowLevelLoc, fogEnabledLoc, lgtLoc, texLoc, wireFrameLoc;
float waterLevel = 0.0, snowLevel = (3 * 10) * 0.25f, fogEnabled = 0.0, wireframe = 0.0;


//Generate vertex and element data for the terrain floor
void generateData()
{
	int indx, start;
	//verts array
	for (int i = 0; i < 10; i++)   //100 vertices on a 10x10 grid
	{
		for (int j = 0; j < 10; j++)
		{
			indx = 10 * i + j;
			verts[3 * indx] = 10 * i - 45;		//x
			verts[3 * indx + 1] = 0;			//y
			verts[3 * indx + 2] = -10 * j;		//z
		}
	}

	//elems array
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			indx = 9 * i + j;
			start = 10 * i + j;
			elems[4 * indx] = start;
			elems[4 * indx + 1] = start + 10;
			elems[4 * indx + 2] = start + 11;
			elems[4 * indx + 3] = start + 1;
		}
	}
}

void processNormalKeys(unsigned char key, int xx, int yy)
{
	switch (key)
	{
	case 'q': // Increase water level
		waterLevel += 0.1f;
		if (waterLevel > 10.0f)
		{
			waterLevel = 10.0f;
		}
		break;

	case 'a': // decrease water level
		waterLevel -= 0.1f;
		if (waterLevel < 0.0)
		{
			waterLevel = 0.0f;
		}
		break;

	case ' ': // switch rendering style
		wireframe = remainder(wireframe + 1, 2.0);
		break;

	case 'w': // increase snow level
		snowLevel += 0.1f;
		if (snowLevel > 10.0f)
		{
			snowLevel = 10.0f;
		}
		break;

	case 's': // decrease snow level
		snowLevel -= 0.1f;
		if (snowLevel < 0.0)
		{
			snowLevel = 0.0f;
		}
		break;

	case 'f': // toggle fog
		fogEnabled = remainder(fogEnabled + 1, 2.0);
		break;

	case '1': // heightmap 1
		glUniform1i(texLoc, 0);
		break;

	case '2': // heightmap 2
		glUniform1i(texLoc, 1);
		break;

	case 'e': // Rotate light left
		lgtAngle++;
		if (lgtAngle >= 360.0)
		{
			lgtAngle = 0.0;
		}
		break;

	case 'r': // Rotate light right
		lgtAngle--;
		if (lgtAngle <= 0)
		{
			lgtAngle = 360.0;
		}
		break;

	}
	glutPostRedisplay();
}

// Function to handle keyboard controlls from special keys
void processSpecialKeys(int key, int xx, int yy) {

	const float cameraSpeed = 1.0f;
	glm::vec3 direction;

	switch (key) {
	case GLUT_KEY_UP: // Move Forward
		cameraPos += cameraSpeed * cameraFront;
		break;

	case GLUT_KEY_DOWN: // Movebackward
		cameraPos -= cameraSpeed * cameraFront;
		break;

	case GLUT_KEY_LEFT: // Rotate left
		yaw -= 1;
		break;

	case GLUT_KEY_RIGHT: // Rotate right
		yaw += 1;
		break;

	case GLUT_KEY_PAGE_DOWN: // Rotate down
		pitch -= 1;
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
		break;

	case GLUT_KEY_PAGE_UP: // Rotate up
		pitch += 1;
		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		break;

	}

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	glutPostRedisplay();
}

#endif