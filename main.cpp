#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
using namespace std;


//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include <thread>
#include <chrono>


#include "Images\FreeImage.h"

#include "shaders\Shader.h"

CShader* myShader;  ///shader object 
CShader* myBasicShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

float amount = 0;
float temp = 0.002f;

#include "FreeType.h"		//include font library
using namespace freetype;
Font our_font;   //FONT TYPE
	
//CThreeDModel boxLeft, boxRight, boxFront, boxNew;
CThreeDModel model, carmodel, sunmodel, moonmodel, bulbmodel; //A threeDModel object is needed for each model loaded
COBJLoader objLoader;	//this object is used to load the 3d models.
std::vector<CThreeDModel> models;
std::vector<CThreeDModel> carmodels;

std::vector<std::string> objFilePaths = { "TestModels/ferriswheel/base.obj", "TestModels/platform/platform.obj", 
	"TestModels/foliage/trees2.obj", "TestModels/ground.obj", "TestModels/wall/walls3.obj", "TestModels/wall/corners.obj", "TestModels/lightpanelreal.obj", "TestModels/lightpanelreal2.obj",
	"TestModels/tables.obj", "TestModels/bins.obj", "TestModels/signpost.obj", "TestModels/funny2.obj", "TestModels/message.obj", "TestModels/shack.obj" };

std::vector<std::string> carriageFilePaths = {
	"TestModels/ferriswheel/carriages/car0.obj", "TestModels/ferriswheel/carriages/car1.obj", "TestModels/ferriswheel/carriages/car2.obj",
	"TestModels/ferriswheel/carriages/car3.obj", "TestModels/ferriswheel/carriages/car4.obj", "TestModels/ferriswheel/carriages/car5.obj",
	"TestModels/ferriswheel/carriages/car6.obj", "TestModels/ferriswheel/carriages/car7.obj", "TestModels/ferriswheel/carriages/car8.obj",
	"TestModels/ferriswheel/carriages/car9.obj", "TestModels/ferriswheel/carriages/car10.obj", "TestModels/ferriswheel/carriages/car11.obj",
	"TestModels/ferriswheel/carriages/car12.obj", "TestModels/ferriswheel/carriages/car13.obj", "TestModels/ferriswheel/carriages/car14.obj",
	"TestModels/ferriswheel/carriages/car15.obj"
};

std::vector<glm::vec3> carinitialpositions = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f},
										{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f},
										{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f},
										{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ProjectionMatrix2D;
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

glm::mat4 objectRotation;
glm::mat4 lightRotation;
glm::vec3 rotatingLight = glm::vec3(0.0f, 20.0f, 0.0f); //light position
glm::mat4 carTranslation;
glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f); //vector for the position of the object.
glm::vec3 carpos = glm::vec3(0.0f, 0.0f, 0.0f); //vector for carriage position

//Material properties
float Material_Ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float Material_Diffuse[4] = {0.8f, 0.8f, 0.5f, 1.0f};
float Material_Specular[4] = {0.9f,0.9f,0.8f,1.0f};
float Material_Shininess = 25;
float brightness = 1.0f; //skybox brightness
bool brightnessDirection = true; //true = increase brightness, false = decrease brightness
float dayNightConst = 0.0f; //constant to control the speed of day/night cycle
bool toggledayNight = false;

//Light Properties
float Light_Ambient_And_Diffuse[4] = {2.0f, 2.0f, 2.0f, 1.0f}; //rgba i.e. light colour/intensity
float Light_Specular[4] = {1.0f,1.0f,1.0f,10.0f}; //object reflection intensity
float LightPos[4] = {0.0f, 100.0f, 0.0f, 0.0f};
//float LightPos[4] = {rotatingLight.x, rotatingLight.y, rotatingLight.z, 0.0f}

float fog_color[4] = {0.75f, 0.75f, 0.75f, 1.0f}; //rgba i.e. fog colour/intensity
float fog_maxdist = 150.0f;
float fog_mindist = 75.0f;
bool toggleFog = false;

static float blendFactor = 0.0f; // Initialize blendFactor


double minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0; //variables for bounding box collision checks


//mouse properties
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth=1280, screenHeight=720;

//booleans to handle when the arrow keys are pressed or released.
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool Home = false;
bool End = false;
bool KeyTwo = false;
bool KeyOne = false;
bool o = false, p = false, a = false, d = false, w = false, s = false;
bool exactSecondFrame = false;
bool cameraOne = true, cameraTwo = false, cameraThree = false; //camera one is freeroam, camera two is cart view

bool toggleAA = true;

float spin=180;
float speed=0;

bool collisions = true;

bool fpsCap = false;

//for camera movement
bool firstMouse = true;
glm::vec3 cameraPos = glm::vec3(0.0f, -17.0f, 40.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraThreePos = glm::vec3(25.0f, -18.0f, 55.0f);

glm::vec3 previousCameraPos = cameraPos;

//camera speed will vary based on how many frames you can render, so I am counting the time taken so I can multiply the
//speed value by it, keeping the move speed of freeroam camera constant on all computers.
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame (value assigned in game loop)

//for camera direction
glm::vec3 direction;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 1280.0f / 2.0;
float lastY = 720.0 / 2.0;
float fov = 45.0f;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void updateTransform(float xinc, float yinc, float zinc, float xmove, float ymove, float zmove);
void mouseFunc(int xposIn, int yposIn);
void mouseScrollerFunc(int wheel, int direction, int x, int y);
void calculateFPS();
void checkCollisions(CThreeDModel model, double aabbCenter[3], double aabbHalfSize[3]);
void menuHandler(int option);
void createMenu();

float angles[16] = { M_PI + glm::radians(22.5f * 0), M_PI + glm::radians(22.5f * 1), M_PI + glm::radians(22.5f * 2), M_PI + glm::radians(22.5f * 3), M_PI + glm::radians(22.5f * 4), M_PI + glm::radians(22.5f * 5), M_PI + glm::radians(22.5f * 6), M_PI + glm::radians(22.5f * 7),
M_PI + glm::radians(22.5f * 8), M_PI + glm::radians(22.5f * 9), M_PI + glm::radians(22.5f * 10), M_PI + glm::radians(22.5f * 11), M_PI + glm::radians(22.5f * 12), M_PI + glm::radians(22.5f * 13), M_PI + glm::radians(22.5f * 14), M_PI + glm::radians(22.5f * 15) };
//float angle = M_PI + glm::radians(22.5f * 15);  // Angle for circular motion
float angle = 90.0f;

GLuint skyboxVAO, skyboxVBO;
CShader* skyboxShader = new CShader();
CShader* planetShader = new CShader();
unsigned int dayCubemapTexture;
unsigned int nightCubemapTexture;

//for loop in init() initialising 16 objects, for loop in display() drawing them all,
//for loop in updateTransform() updating their positions by accessing their angles from angles array

// Point light properties
glm::vec3 pointLightPosition = glm::vec3(60.0f, 10.0f, 0.0f); // Position in world space
glm::vec4 pointLightAmbient = glm::vec4(11.5f, 11.5f, 11.5f, 1.0f);
glm::vec4 pointLightDiffuse = glm::vec4(12.8f, 12.8f, 12.8f, 1.0f);
glm::vec4 pointLightSpecular = glm::vec4(16.0f, 16.0f, 16.0f, 1.0f);
float pointLightConstant = 1.0f;
float pointLightLinear = 0.22f;
float pointLightQuadratic = 0.20f;
float lightBrightness = 0.0f; // Current brightness factor
float brightnessSpeed = 3.0f; // Speed of brightness oscillation

float spotlightDiff = 0.0f; //20.0f
float spotlightSpec = 0.0f; //30.0f

// Spotlight properties
glm::vec3 spotlightPosition = glm::vec3(20.8f, -21.5f, 20.5f); // Position in world space
//glm::vec3 spotlightDirection = glm::vec3(-1.0f, 0.0f, 1.0f); // Direction in world space
glm::vec3 target = glm::vec3(0.0f, 10.0f, 0.0f); // Target point (e.g., origin)
glm::vec3 spotlightDirection = glm::normalize(target - spotlightPosition); // Direction from position to target
glm::vec4 spotlightAmbient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
glm::vec4 spotlightDiffuse = glm::vec4(spotlightDiff, spotlightDiff, spotlightDiff, 1.0f);
glm::vec4 spotlightSpecular = glm::vec4(spotlightSpec, spotlightSpec, spotlightSpec, 1.0f);
float spotlightCutOff = glm::cos(glm::radians(65.0f)); // Inner cutoff angle
float spotlightConstant = 1.0f;
float spotlightLinear = 0.005f;
float spotlightQuadratic = 0.005f;

//2nd spotlight
glm::vec3 spotlightPosition2 = glm::vec3(-spotlightPosition.x, spotlightPosition.y, spotlightPosition.z); 
glm::vec3 spotlightDirection2 = glm::normalize(target - spotlightPosition2); 


/*************    START OF OPENGL FUNCTIONS   ****************/
void display()
{
	ProjectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 10000.0f);

	float currentFrame = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;


	//cout << "\rfps: " << round(1 / deltaTime) << flush;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render skybox
	glDepthFunc(GL_LEQUAL);  // Change depth function so skybox is rendered behind everything
	glUseProgram(skyboxShader->GetProgramObjID());

	glm::mat4 view;
	if (cameraTwo) {
		// Use the cart's position and orientation for the skybox view
		glm::vec3 tempCameraPos = carinitialpositions[1];
		tempCameraPos.y -= 0.8f;
		view = glm::mat4(glm::mat3(glm::lookAt(tempCameraPos, tempCameraPos + cameraFront, cameraUp)));
	}
	else {
		// Default free-roam camera view
		view = glm::mat4(glm::mat3(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
	}

	if (toggledayNight)
	{
		dayNightConst = 0.25f;
	}
	else
	{
		dayNightConst = 0.0f;
	}

	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgramObjID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->GetProgramObjID(), "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
	// Brightness
	// Bind day and night skybox textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dayCubemapTexture);
	glUniform1i(glGetUniformLocation(skyboxShader->GetProgramObjID(), "daySkybox"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, nightCubemapTexture);
	glUniform1i(glGetUniformLocation(skyboxShader->GetProgramObjID(), "nightSkybox"), 1);

	// Set the blend factor (e.g., based on time of day)
	glUniform1f(glGetUniformLocation(skyboxShader->GetProgramObjID(), "blendFactor"), blendFactor);

	// Render the skybox
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // Reset depth function

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//Part for displacement shader.
	amount += temp;
	if (amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "displacement"), amount);

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix = glm::mat4(1.0f);

	if (cameraPos.y < -22.0f)
	{
		cameraPos.y = -22.0f;
	}

	double aabbCenter[3] = { cameraPos.x, cameraPos.y, cameraPos.z };
	double aabbHalfSize[3] = { 1.0, 1.0, 1.0 };

	if (cameraTwo)
	{
		glm::vec3 tempCameraPos = carinitialpositions[1];
		tempCameraPos.y -= 0.8f;
		viewingMatrix = glm::lookAt(tempCameraPos, tempCameraPos + cameraFront, cameraUp);
	}
	else if (cameraThree) {
		cameraPos.x = cameraThreePos.x;
		cameraPos.y = cameraThreePos.y;
		cameraPos.z = cameraThreePos.z;
		viewingMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	else
	{
		viewingMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);

	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "fog_maxdist"), fog_maxdist);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "fog_mindist"), fog_mindist);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "fog_color"), 1, fog_color);	




	// Transform point light position to eye space
	glm::vec3 pointLightPositionEye = glm::vec3(viewingMatrix * glm::vec4(pointLightPosition, 1.0f));

	// Update brightness factor using a sine wave
	lightBrightness = (sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f * brightnessSpeed) + 1.0f) / 2.0f; // Oscillates between 0 and 1

	// Scale the light properties by the brightness factor
	glm::vec4 dynamicAmbient = pointLightAmbient * lightBrightness;
	glm::vec4 dynamicDiffuse = pointLightDiffuse * lightBrightness;
	glm::vec4 dynamicSpecular = pointLightSpecular * lightBrightness;

	// Pass the dynamically updated light properties to the shader
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].ambient"), 1, glm::value_ptr(dynamicAmbient));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].diffuse"), 1, glm::value_ptr(dynamicDiffuse));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].specular"), 1, glm::value_ptr(dynamicSpecular));


	// Pass point light properties to the shader
	glUniform3fv(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].position"), 1, glm::value_ptr(pointLightPositionEye));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].ambient"), 1, glm::value_ptr(dynamicAmbient));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].diffuse"), 1, glm::value_ptr(dynamicDiffuse));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].specular"), 1, glm::value_ptr(dynamicSpecular));
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].constant"), pointLightConstant);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].linear"), pointLightLinear);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "pointLights[0].quadratic"), pointLightQuadratic);

	for (int i = 3, lightIndex = 1; i < carinitialpositions.size(); i += 2, ++lightIndex) {
		glm::vec3 pointLightPositionEye = glm::vec3(viewingMatrix * glm::vec4(carinitialpositions[i], 1.0f));

		glUniform3fv(glGetUniformLocation(myShader->GetProgramObjID(), ("pointLights[" + std::to_string(lightIndex) + "].position").c_str()), 1, glm::value_ptr(pointLightPositionEye));
		glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), ("pointLights[" + std::to_string(lightIndex) + "].ambient").c_str()), 1, glm::value_ptr(dynamicAmbient));
		glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), ("pointLights[" + std::to_string(lightIndex) + "].diffuse").c_str()), 1, glm::value_ptr(dynamicDiffuse));
		glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), ("pointLights[" + std::to_string(lightIndex) + "].specular").c_str()), 1, glm::value_ptr(dynamicSpecular));
		glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), ("pointLights[" + std::to_string(lightIndex) + "].constant").c_str()), pointLightConstant);
		glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), ("pointLights[" + std::to_string(lightIndex) + "].linear").c_str()), pointLightLinear);
		glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), ("pointLights[" + std::to_string(lightIndex) + "].quadratic").c_str()), pointLightQuadratic);

		//glm::mat4 bulbsModelMatrix = glm::translate(glm::mat4(1.0f), carinitialpositions[i]);
		glm::mat4 bulbsModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(carinitialpositions[i].x + 0.5, carinitialpositions[i].y - 0.6, carinitialpositions[i].z));
		bulbsModelMatrix = glm::rotate(bulbsModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate 90 degrees on the Z-axis
		bulbsModelMatrix = glm::scale(bulbsModelMatrix, glm::vec3(0.125f, 0.125f, 0.125f));
		ModelViewMatrix = viewingMatrix * bulbsModelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		bulbmodel.DrawElementsUsingVBO(myShader);
	}


	// Set the number of point lights
	glUniform1i(glGetUniformLocation(myShader->GetProgramObjID(), "num_point_lights"), 8);


	// Calculate the direction vector pointing toward the origin (0, 0, 0)
	//spotlightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f) - spotlightPosition);

	spotlightDiffuse = glm::vec4(spotlightDiff, spotlightDiff, spotlightDiff, 1.0f);
	spotlightSpecular = glm::vec4(spotlightSpec, spotlightSpec, spotlightSpec, 1.0f);

	//----------------------------SPOTLIGHT 1

	// Transform spotlight position and direction to eye space
	glm::vec3 spotlightPositionEye = glm::vec3(viewingMatrix * glm::vec4(spotlightPosition, 1.0f));
	glm::vec3 spotlightDirectionEye = glm::normalize(glm::mat3(viewingMatrix) * spotlightDirection);

	// Pass spotlight properties to the shader
	glUniform3fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].position"), 1, glm::value_ptr(spotlightPositionEye));
	glUniform3fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].direction"), 1, glm::value_ptr(spotlightDirectionEye));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].ambient"), 1, glm::value_ptr(spotlightAmbient));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].diffuse"), 1, glm::value_ptr(spotlightDiffuse));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].specular"), 1, glm::value_ptr(spotlightSpecular));
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].cutOff"), spotlightCutOff);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].constant"), spotlightConstant);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].linear"), spotlightLinear);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[0].quadratic"), spotlightQuadratic);

	//----------------------------SPOTLIGHT 2

	// Transform spotlight position and direction to eye space
	glm::vec3 spotlightPositionEye2 = glm::vec3(viewingMatrix * glm::vec4(spotlightPosition2, 1.0f));
	glm::vec3 spotlightDirectionEye2 = glm::normalize(glm::mat3(viewingMatrix) * spotlightDirection2);

	// Pass spotlight properties to the shader
	glUniform3fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].position"), 1, glm::value_ptr(spotlightPositionEye2));
	glUniform3fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].direction"), 1, glm::value_ptr(spotlightDirectionEye2));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].ambient"), 1, glm::value_ptr(spotlightAmbient));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].diffuse"), 1, glm::value_ptr(spotlightDiffuse));
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].specular"), 1, glm::value_ptr(spotlightSpecular));
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].cutOff"), spotlightCutOff);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].constant"), spotlightConstant);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].linear"), spotlightLinear);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "spotLights[1].quadratic"), spotlightQuadratic);


	// Set the number of spotlights
	glUniform1i(glGetUniformLocation(myShader->GetProgramObjID(), "num_spot_lights"), 2);

	glm::mat4 modelmatrix = glm::translate(glm::mat4(1.0f), pos);
	ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	model.DrawElementsUsingVBO(myShader); //ferris wheel
	//model.DrawAllBoxesForOctreeNodes(myShader);
	if (collisions)
	{
		checkCollisions(model, aabbCenter, aabbHalfSize);
	}

	for (size_t i = 0; i < carmodels.size(); i++)
	{
		glm::mat4 modelmatrix2 = glm::translate(glm::mat4(1.0f), carinitialpositions[i]);
		ModelViewMatrix = viewingMatrix * modelmatrix2;
		glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		carmodels[i].DrawElementsUsingVBO(myShader);
		//carmodels[i].DrawBoundingBox(myShader);
		if (collisions)
		{
			checkCollisions(carmodels[i], aabbCenter, aabbHalfSize);
		}
	}

	glm::mat4 modelmatrix3 = glm::translate(glm::mat4(1.0f), glm::vec3(-LightPos[0], -LightPos[1], -LightPos[2]));
	modelmatrix3 = glm::scale(modelmatrix3, glm::vec3(2.5f, 2.5f, 2.5f)); // Scale the moonmodel

	ModelViewMatrix = viewingMatrix * modelmatrix3;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	moonmodel.DrawElementsUsingVBO(myShader);
	
	glUseProgram(planetShader->GetProgramObjID()); //use planets shader

	// Set the uniform for the sun color
	glUniform3f(glGetUniformLocation(planetShader->GetProgramObjID(), "sunColor"), 1.0f, 1.0f, 0.75f); // Bright yellow

	// Set the projection and model-view matrices
	glUniformMatrix4fv(glGetUniformLocation(planetShader->GetProgramObjID(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(planetShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	glm::mat4 modelmatrix4 = glm::translate(glm::mat4(1.0f), glm::vec3(LightPos[0], LightPos[1], LightPos[2]));
	modelmatrix4 = glm::scale(modelmatrix4, glm::vec3(2.5f, 2.5f, 2.5f)); // Scale the sunmodel

	ModelViewMatrix = viewingMatrix * modelmatrix4;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	sunmodel.DrawElementsUsingVBO(myShader);

	//lightbulb

	glm::mat4 modelmatrix5 = glm::translate(glm::mat4(1.0f), glm::vec3(carinitialpositions[1].x + 0.5, carinitialpositions[1].y - 0.6, carinitialpositions[1].z));
	pointLightPosition.x = carinitialpositions[1].x + 0.5;
	pointLightPosition.y = carinitialpositions[1].y - 0.6;
	pointLightPosition.z = carinitialpositions[1].z;
	modelmatrix5 = glm::rotate(modelmatrix5, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate 90 degrees on the Z-axis
	modelmatrix5 = glm::scale(modelmatrix5, glm::vec3(0.125f, 0.125f, 0.125f)); // Scale the model

	ModelViewMatrix = viewingMatrix * modelmatrix5;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	bulbmodel.DrawElementsUsingVBO(myShader);


	//model2.DrawElementsUsingVBO(myShader);
	
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	//Switch to basic shader to draw the lines for the bounding boxes
	glUseProgram(myBasicShader->GetProgramObjID());
	projMatLocation = glGetUniformLocation(myBasicShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//switch back to the shader for textures and lighting on the objects.
	glUseProgram(myShader->GetProgramObjID());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	
	//got it to load using a forloop, loading from a vector that has objects already initialised by a loop inside
	//of the init() function below.
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //for wireframe view
	for (CThreeDModel& model : models)
	{
		model.DrawElementsUsingVBO(myShader);
		model.CalcBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
		if (collisions)
		{
			if (cameraPos.x > minX && cameraPos.x < maxX && cameraPos.y > minY && cameraPos.y < maxY && cameraPos.z > minZ && cameraPos.z < maxZ)
			{
				checkCollisions(model, aabbCenter, aabbHalfSize); //the intent of this was to make collisions efficient - but my bounding boxes are far too large to be very effective.
			}
		}
	}

	calculateFPS();
	
	glFlush();
	glutSwapBuffers();
}

void checkCollisions(CThreeDModel model, double aabbCenter[3], double aabbHalfSize[3]) {
	for (int i = 0; i < model.GetOctreeTriangleListSize(); i++) { //code to check for collisions by triangles
		if (model.IsTriangleIntersectingAABB(aabbCenter, aabbHalfSize, i)) {
			cameraPos = previousCameraPos;
			break;
		}
	}
}

void calculateFPS() {
	static float framesPerSecond = 0.0f;
	static double fps;
	static float lastTime = 0.0f;
	float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	++framesPerSecond;
	//cout << "\rfps: " << fps << flush;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		fps = framesPerSecond;
		framesPerSecond = 0;
	}
	glDisable(GL_CULL_FACE);
	print(ProjectionMatrix2D, our_font, 20, screenHeight - 30, "fps: %.0f", fps);
	glEnable(GL_CULL_FACE);
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(fov), (GLfloat)screenWidth/(GLfloat)screenHeight, 0.1f, 1.0f);

	//calculate a projection matrix based on ortho graphic view using screen size.
	//This will be used for rendering text to the screen.
	ProjectionMatrix2D = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight), -1.0f, 1.0f);
}

GLuint loadCubemap(std::vector<std::string> faces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < faces.size(); i++) {
		FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(faces[i].c_str(), 0), faces[i].c_str());
		if (!bitmap) {
			std::cerr << "Failed to load texture: " << faces[i] << std::endl;
			continue;
		}

		FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
		FreeImage_FlipVertical(pImage); // Flip the image vertically lol, idk why it loads upside down

		int width = FreeImage_GetWidth(pImage);
		int height = FreeImage_GetHeight(pImage);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(pImage));
		FreeImage_Unload(pImage);
		FreeImage_Unload(bitmap);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //this is to make lines of the skybox not visible
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


void init()
{
	glClearColor(1.0,1.0,1.0,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// Skybox VAO and VBO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<string> dayfaces = {
		"skybox/right.png",
		"skybox/left.png",
		"skybox/top.png",
		"skybox/bottom.png",
		"skybox/front.png",
		"skybox/back.png"
	};

	dayCubemapTexture = loadCubemap(dayfaces);

	vector<string> nightfaces = {
	"skybox/right2.png",
	"skybox/left2.png",
	"skybox/top2.png",
	"skybox/bottom2.png",
	"skybox/front2.png",
	"skybox/back2.png"
	};

	nightCubemapTexture = loadCubemap(nightfaces);


	if (!skyboxShader->CreateShaderProgram("Skybox", "skybox.vert", "skybox.frag")) {
		cout << "failed to load skybox shader" << endl;
	}

	if (!planetShader->CreateShaderProgram("planetShader", "planet.vert", "planet.frag")) {
		cout << "Failed to load planet shader" << endl;
	}


	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);

	myShader = new CShader();
	//if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag"))
	if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	myBasicShader = new CShader();
	if(!myBasicShader->CreateShaderProgram("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	glEnable(GL_TEXTURE_2D);

	//lets initialise our object's rotation transformation 
	//to the identity matrix
	objectRotation = glm::mat4(1.0f);

	cout << " loading model " << endl;
	if(objLoader.LoadModel("TestModels/ferriswheel/wheel.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;		

		//copy data from the OBJLoader object to the threedmodel class
		model.ConstructModelFromOBJLoader(objLoader);

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		model.CalcCentrePoint();
		model.CentreOnZero();
		model.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.LoadModel("TestModels/lightbulb2.obj"))//returns true if the model is loaded
	{
		bulbmodel.ConstructModelFromOBJLoader(objLoader);
		bulbmodel.CalcCentrePoint();
		bulbmodel.CentreOnZero();
		bulbmodel.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	for (int i = 0; i < carriageFilePaths.size(); i++)
	{
		CThreeDModel tempmodel;
		if (objLoader.LoadModel(carriageFilePaths[i]))//returns true if the model is loaded
		{
			cout << " model loaded " << endl;
			tempmodel.ConstructModelFromOBJLoader(objLoader);
			tempmodel.InitVBO(myShader);
			carmodels.push_back(tempmodel);
		}
		else
		{
			cout << " model failed to load " << endl;
		}
	}

	if (objLoader.LoadModel("TestModels/moon2.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		moonmodel.ConstructModelFromOBJLoader(objLoader);
		moonmodel.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.LoadModel("TestModels/sun2.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;
		sunmodel.ConstructModelFromOBJLoader(objLoader);
		sunmodel.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	
	for (const std::string& dir : objFilePaths) //loop through objects and display them
	{
		CThreeDModel currentObj;
		if (objLoader.LoadModel(dir))//returns true if the model is loaded
		{
			cout << " model loaded HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH" << endl;
			//copy data from the OBJLoader object to the threedmodel class
			currentObj.ConstructModelFromOBJLoader(objLoader);
			currentObj.InitVBO(myShader);
			models.push_back(currentObj);
		}
		else
		{
			cout << " FAAAAAAAAAAAAAAIIIIIIIIIIILLLLLLLLLLLLLLL" << endl;
		}
	}
	our_font.init("BRLNSDB.TTF", 15);
}

void special(int key, int x, int y) //currently not in use
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = true;
		break;
	case GLUT_KEY_RIGHT:
		Right = true;
		break;
	case GLUT_KEY_UP:
		Up = true;
		break;
	case GLUT_KEY_DOWN:
		Down = true;
		break;
	case GLUT_KEY_HOME:
		Home = true;
		break;
	case GLUT_KEY_END:
		End = true;
		break;
	}
}

void specialUp(int key, int x, int y) //currently not in use
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = false;
		break;
	case GLUT_KEY_RIGHT:
		Right = false;
		break;
	case GLUT_KEY_UP:
		Up = false;
		break;
	case GLUT_KEY_DOWN:
		Down = false;
		break;
	case GLUT_KEY_HOME:
		Home = false;
		break;
	case GLUT_KEY_END:
		End = false;
		break;		
	}
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key) {
	case '2':
		KeyTwo = true;
		break;
	case '1':
		//KeyOne = true;
		if (KeyOne == true)
		{
			KeyOne = false;
		}
		else
		{
			KeyOne = true;
		}
		break;
	case '3': //toggle between camera views
		if (cameraOne == true && cameraTwo == false)
		{
			cameraOne = false;
			cameraTwo = true;
		}
		else if (cameraOne == false && cameraTwo == true)
		{
			cameraOne = true;
			cameraTwo = false;
		}
		break;
	case 'o':
		o = true;
		break;
	case 'p':
		p = true;
		break;


	case 'w':
		if (cameraOne) //only allow freeroam to work if cameraOne is active
		{
			w = true;
		}
		else
		{
			w = false;
		}
		break;
	case 'a':
		if (cameraOne) //putting all the cases into an if(cameraOne) didn't work so this will have to do :(
		{
			a = true;
		}
		else
		{
			a = false;
		}
		break;
	case 's':
		if (cameraOne)
		{
			s = true;
		}
		else
		{
			s = false;
		}
		break;
	case 'd':
		if (cameraOne)
		{
			d = true;
		}
		else
		{
			d = false;
		}
		break;
	}
}

void keyboardFuncUp(unsigned char key, int x, int y)
{
	switch (key) {
	case '2':
		KeyTwo = false;
		break;
	case '1':
		//KeyOne = false;
		break;

	case 'o':
		o = false;
		break;
	case 'p':
		p = false;
		break;
		

	case 'w':
		w = false;
		break;
	case 'a':
		a = false;
		break;
	case 's':
		s = false;
		break;
	case 'd':
		d = false;
		break;
	}
}

void processKeys()
{
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f, moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
	float cameraSpeed = 40.0f * deltaTime;

	previousCameraPos = cameraPos;
	

	if (KeyOne)
	{
		spinZinc = 0.25f * deltaTime;
	}
	if (KeyTwo)
	{
		spinZinc = -1.0f * deltaTime;
	}
	if (w)
	{
		//moveY = -0.025f;
		cameraPos += cameraSpeed * cameraFront;
	}
	if (a)
	{
		//moveY = 0.025f;
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (s)
	{
		//moveZ = -0.025f;
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (d)
	{
		//moveZ = 0.025f;
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	updateTransform(spinXinc, spinYinc, spinZinc, moveX, moveY, moveZ);
}

void mouseFunc(int xposIn, int yposIn) {
	//cout << "\rMouse dragged to: (" << xposIn << ", " << yposIn << ")" << flush;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		//cout << "first mouse" << endl;
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		return;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.5f; //sensitivity!
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// to make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void mouseCallback(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		//printf("Left mouse button released at (%d, %d)\n", x, y);
		firstMouse = true;
	}
}

void mouseScrollerFunc(int wheel, int direction, int x, int y) {
	fov -= (float)direction;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 90.0f)
		fov = 90.0f;
	if (direction > 0) {
		//std::cout << "Scrolled UP at (" << x << ", " << y << ")" << std::endl;
		//cout << "direction: (" << direction << ")" << endl;
	}
	else {
		//std::cout << "Scrolled DOWN at (" << x << ", " << y << ")" << std::endl;
		//cout << "direction: (" << direction << ")" << endl;
	}
}

void updateTransform(float xinc, float yinc, float zinc, float xmove, float ymove, float zmove)
{
	objectRotation = glm::rotate(objectRotation, zinc, glm::vec3(0,0,1)); //rotate wheel

	float radius = 15.728f; // Radius of the circular path
	float radius2 = 450.0f;
	float centerX = 0.0f; // Center of the circle on X axis
	float centerY = 0.0f; // Center of the circle on Y axis

	for (int i = 0; i < 16; i++)
	{
		float x = (radius * cos(angles[i]));
		float y = (radius * sin(angles[i]));
		carinitialpositions[i] = glm::vec3(x, y, 0);


		angles[i] += zinc;
		if (angles[i] > 360)
		{
			angles[i] = 0;
		}
	}


	float x2 = radius2 * cos(angle);
	float y2 = radius2 * sin(angle);
	float z2 = radius2 * -sin(angle); // Circular motion in the Z-axis
	LightPos[0] = x2;
	LightPos[1] = y2;
	LightPos[2] = z2;
	LightPos[3] = 1.0f;
	angle += (0.001 * deltaTime * 300) * dayNightConst;
	if (angle > 2 * M_PI)
	{
		angle -= 2 * M_PI;
	}
	if (LightPos[1] < -0.0f)
	{
		if (Light_Ambient_And_Diffuse[0] > 1.0f)
		{
			Light_Ambient_And_Diffuse[0] -= 0.010f * deltaTime;
			Light_Ambient_And_Diffuse[1] -= 0.010f * deltaTime;
			Light_Ambient_And_Diffuse[2] -= 0.010f * deltaTime;
			Light_Ambient_And_Diffuse[3] -= 0.010f * deltaTime;
		}
	}
	else
	{
		Light_Ambient_And_Diffuse[0] = 2.0f;
		Light_Ambient_And_Diffuse[1] = 2.0f;
		Light_Ambient_And_Diffuse[2] = 2.0f;
		Light_Ambient_And_Diffuse[3] = 2.0f;
	}

	if (brightnessDirection)
	{
		brightness += (0.001 * deltaTime * 100) * dayNightConst;
		if (brightness > 1.0f)
		{
			brightness = 1.0f;
			brightnessDirection = false;
		}
	}
	else
	{
		brightness -= (0.001 * deltaTime * 100) * dayNightConst;
		if (brightness < 0.0f)
		{
			brightness = 0.0f;
			brightnessDirection = true;
		}
	}

	// Smooth transition for blendFactor
	if (LightPos[1] < -50.0f)
	{
		blendFactor += 0.1f * deltaTime; //transition to night
		if (blendFactor > 1.0f) blendFactor = 1.0f;

		spotlightDiff += 10.0f * deltaTime; //15.0f
		if (spotlightDiff > 15.0f) spotlightDiff = 15.0f;
		spotlightSpec += 10.0f * deltaTime; //20.0f
		if (spotlightSpec > 20.0f) spotlightSpec = 20.0f;
	}
	else
	{
		blendFactor -= 0.1f * deltaTime; //transition to day
		if (blendFactor < 0.0f) blendFactor = 0.0f;

		spotlightDiff -= 10.0f * deltaTime; //0.0f
		if (spotlightDiff < 0.0f) spotlightDiff = 0.0f;
		spotlightSpec -= 10.0f * deltaTime; //0.0f
		if (spotlightSpec < 0.0f) spotlightSpec = 0.0f;
	}
}

void idle()
{
	static int lastTime = 0; //time of the last frame
	int currentTime = glutGet(GLUT_ELAPSED_TIME); //current time in ms

	//calculate the time elapsed since the last frame
	int elapsedTime = currentTime - lastTime;

	//60 FPS (1s = 1000ms)
	const int targetFrameTime = 1000 / 60;

	if (elapsedTime < targetFrameTime)
	{
		//add delay if frame rendered too fast
		int delay = targetFrameTime - elapsedTime;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}

	//update last frame time
	if (fpsCap)
	{
		lastTime = glutGet(GLUT_ELAPSED_TIME);
	}
	

	processKeys();
	glutPostRedisplay();
}

void menuHandler(int option)
{
	if (option == 1) //cart view
	{
		cameraOne = false;
		cameraTwo = true;
		cameraThree = false;
	}
	else if (option == 2) //free roam
	{
		cameraOne = true;
		cameraTwo = false;
		cameraThree = false;
	}
	else if (option == 3) //fixed camera
	{
		cameraThree = true;
		cameraTwo = false;
		cameraOne = false;
	}
	else if (option == 4) //toggle anti-aliasing
	{
		if (toggleAA)
		{
			glDisable(GL_MULTISAMPLE);
			toggleAA = false;
		}
		else
		{
			glEnable(GL_MULTISAMPLE);
			toggleAA = true;
		}
	}
	else if (option == 5) //toggle collisions
	{
		collisions = !collisions;
	}
	else if (option == 6)
	{
		fpsCap = !fpsCap;
	}
	else if (option == 7) 
	{
		toggleFog = !toggleFog;
		if (toggleFog)
		{
			//toggleFog = false;
			fog_maxdist = 12500.0f;
			fog_mindist = 75.0f;
		}
		else
		{
		    //toggleFog = true;
			fog_maxdist = 150.0f;
			fog_mindist = 75.0f;
		}
	}
	else if (option == 8)
	{
		toggledayNight = !toggledayNight;
	}
	else if (option == 9)
	{
		exit(0);
	}
	glutPostRedisplay();  // Redraw after menu action
}

void createMenu() 
{
	int menu = glutCreateMenu(menuHandler);
	glutAddMenuEntry("Cart View", 1);
	glutAddMenuEntry("Free Roam", 2);
	glutAddMenuEntry("Fixed View", 3);
	glutAddMenuEntry("Toggle Anti-Aliasing", 4);
	glutAddMenuEntry("Toggle collisions", 5);
	glutAddMenuEntry("Toggle FPS cap", 6);
	glutAddMenuEntry("Toggle Fog", 7);
	glutAddMenuEntry("Toggle Day/Night Cycle", 8);
	glutAddMenuEntry("Exit Program", 9);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**************** END OPENGL FUNCTIONS *************************/

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutSetOption(GLUT_MULTISAMPLE, 16);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Ferris Wheel Scene");
	glutFullScreen();

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;


	//initialise the objects for rendering
	init();

	createMenu();

	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);

	glutKeyboardFunc(keyboardFunc);
	glutKeyboardUpFunc(keyboardFuncUp);

	glutMotionFunc(mouseFunc);
	glutMouseWheelFunc(mouseScrollerFunc);
	glutMouseFunc(mouseCallback);

	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}