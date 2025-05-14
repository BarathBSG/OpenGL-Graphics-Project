#pragma once
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
class Camera
{
public:
	glm::mat4 viewingMatrix = glm::mat4(1.0f);
	Camera();
	~Camera();
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void GetPosition(float& x, float& y, float& z);
	void GetRotation(float& x, float& y, float& z);
	void Render();
	void GetViewMatrix();
	void SetViewMatrix(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, CShader* myShader);
};