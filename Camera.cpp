#include "Camera.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
using namespace std;
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "GL\freeglut.h"
#include "shaders\Shader.h"

Camera::Camera() //constructor
{
}

Camera::~Camera() //desctructor
{
}

void Camera::SetPosition(float x, float y, float z)
{
}

void Camera::SetRotation(float x, float y, float z)
{
}

void Camera::GetPosition(float& x, float& y, float& z)
{
}

void Camera::GetRotation(float& x, float& y, float& z)
{
}

void Camera::Render()
{
}

void Camera::GetViewMatrix()
{
}

void Camera::SetViewMatrix(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, CShader* myShader)
{
	viewingMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);
}
