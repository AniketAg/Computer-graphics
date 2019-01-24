/*----------------------------------------------------
Headers, Include files & Namespace Declarations
----------------------------------------------------*/
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <iostream>
#include <mmsystem.h>
#include <string.h>
#include <string>

// For loading textures
#include "SOIL.h"

//for transformations
#include <GL/glm/glm/glm.hpp>
#include <GL/glm/glm/gtc/matrix_transform.hpp>
#include <GL/glm/glm/gtc/type_ptr.hpp>

//list of vertices & positions
#include "cube_vertices.h"
#include "ground_vertices.h"
#include "object_positions.h"

using namespace std;


/*--------------------------------------------
Globals & Constants
--------------------------------------------*/
// Window Constants
int WINDOW_WIDTH = 1400;
int WINDOW_HEIGHT = 800;

// Shader vars
const char OBJECT_VERTEX_SHADER[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/Shaders/ObjectsVertexShader.txt";
const char OBJECT_FRAGMENT_SHADER[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/Shaders/ObjectsFragmentShader.txt";
const char LIGHTING_VERTEX_SHADER[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/Shaders/LightingVertexShader.txt";
const char LIGHTING_FRAGMENT_SHADER[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/Shaders/LightingFragmentShader.txt";

GLuint vertexShader, fragmentShader;
GLuint objectShaderProgramID;
GLuint lightingShaderProgramID;

// Vertex Objects
GLuint CUBE_VAO, GROUND_VAO, LIGHT_VAO;

//Textures
GLuint containerSpecularMapID, containerDiffuseMapID;
GLuint cubeTextID, groundTextID, lightTextID;
GLuint blankTextID, wallTextID;

// View & Camera Vars
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 4.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float pitch = 0.0f, yaw = -90.0f;

// Event array for Key presses
bool keys[256];
bool specKeys[256];

//projection
glm::mat4 projection;


/*-----------------------------------------
Shader Functions
------------------------------------------*/
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {

	FILE* fp;
	fopen_s(&fp, shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"

	if (fp == NULL) {
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}

static GLuint AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object, referenced by an ID
	GLuint ShaderObj = glCreateShader(ShaderType);

	//check if it was created successfully
	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	//load source
	const char* pShaderSource = readShaderSource(pShaderText);

	//attach the shader source code to the shader object and compile the shader
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	glCompileShader(ShaderObj);

	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);

	return ShaderObj;
}

GLuint CompileShaders(const char* VERTEX_SHADER, const char* FRAGMENT_SHADER)
{
	//Note: we will link all the shaders together into this ID

	/*A shader program object is the final linked version of multiple shaders combined.
	We have to link them to a shader program object and then activate this shader program
	when rendering objects. When linking the shaders into a program it links the outputs
	of each shader to the inputs of the next shader. */

	GLuint shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	// Create shader objects, vertex & fragment
	vertexShader = AddShader(shaderProgramID, VERTEX_SHADER, GL_VERTEX_SHADER);
	fragmentShader = AddShader(shaderProgramID, FRAGMENT_SHADER, GL_FRAGMENT_SHADER);
	
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glDetachShader(vertexShader, shaderProgramID);
	glDetachShader(fragmentShader, shaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	return shaderProgramID;
}

#pragma endregion SHADER_FUNCTIONS


/*-----------------------------------------
Vertex Object Functions
------------------------------------------*/
#pragma region VBO_FUNCTIONS

void generateObjectBuffers(GLuint* VAO, GLfloat vertices[], GLuint numVertices) {
	
	//A buffer object is created by binding a name returned by GenBuffers to 
	//a buffer target.The binding is effected by calling glBindBuffers
	GLuint VBO;
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	// Bind buffer and array objects
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// After binding, we now fill our object with data, create empty buffer first
	// if you have more data besides vertices (e.g., colours or yextures), use glBufferSubData to 
	//tell the buffer when the vertices array ends and when the colors start
	glBufferData(GL_ARRAY_BUFFER, numVertices * 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	// param is 0, location of position data in vertex shader
	//enable position attr, pointed to by attribPointer, disabled by default in opengl
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//Texture attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//.. note stride sizes above would be 0 if are tightly packed

	//bind element array and load indices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	//unbind buffers
	// unbind EBO after VAO otherwise EBO wont be associated with VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); //call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
									  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void generateLightObjectBuffers(GLuint* VAO, GLfloat vertices[], GLuint numVertices) {
	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint VBO;
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);

	// Bind buffer objects
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, numVertices * 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the normal vectors
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#pragma endregion VBO_FUNCTIONS


/*-----------------------------------------
	Mesh & Texture Functions
------------------------------------------*/
#pragma region MESH_TEXTURE_FUNCTIONS

void generateTexture(const char* texture_loc, GLuint* texture) {

	// Load and create a texture
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
											// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load image, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image(texture_loc, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	//free the image memory and unbind texture
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void generateTextureMap(const char* texture_loc, GLuint* texture) {

	// Load textures
	//GLuint diffuseMap, specularMap;
	glGenTextures(1, texture);
	//glGenTextures(1, &specularMap);
	int width, height;
	unsigned char* image;
	// Generate texture map
	image = SOIL_load_image(texture_loc, &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	
	/* Specular map
	image = SOIL_load_image("container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	SOIL_free_image_data(image);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	*/
}

#pragma endregion MESH_TEXTURE_FUNCTIONS


/*-----------------------------------------
Key Functions
------------------------------------------*/
#pragma region KEY_FUNCTIONS
void keypress(unsigned char key, int x, int y) {
	keys[int(key)] = true; // Set the state of the current key to pressed  	
}

void keyUp(unsigned char key, int x, int y) {
	keys[int(key)] = false; // Set the state of the current key to not pressed  
}

void specialKeyPress(int key, int x, int y) {
	specKeys[key] = true; // Set the state of the current key to pressed  
}

void specialKeyUp(int key, int x, int y) {
	specKeys[key] = false; // Set the state of the current key to not pressed  
}

void keyOperations(void) {

	//so we cant move up and down the y-axis, remove y value from cameraFront
	glm::vec3 camForward  = cameraFront * glm::vec3(1.0f, 0.0f, 1.0f);

	GLfloat cameraSpeed = 0.1f;
	if (specKeys[GLUT_KEY_UP])
		cameraPos += cameraSpeed * camForward;
	if (specKeys[GLUT_KEY_DOWN])
		cameraPos -= cameraSpeed * camForward;
	if (specKeys[GLUT_KEY_LEFT])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (specKeys[GLUT_KEY_RIGHT])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;


	if (keys['w'])
		//if(pitch <= 89.9f)
			pitch += 0.25;
	if (keys['s'])
		//if (pitch >= -89.9f)
			pitch -= 0.25;
	if (keys['d'])
		//if (yaw <= -0.1f)
		yaw += 0.25;
	if (keys['a'])
		//if (yaw >= -179.9f)
			yaw -= 0.25;

	// Draw the next frame
	glutPostRedisplay();
}

#pragma endregion KEY_FUNCTIONS


/*-----------------------------------------
Display Functions
------------------------------------------*/
#pragma region DISPLAY_FUNCTIONS

void display() {
	//check for key events
	keyOperations();

	// enable depth-testing - only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	//clear last display. Clear screen color buffer & depth buffer
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(objectShaderProgramID);

	GLint viewPosLoc = glGetUniformLocation(objectShaderProgramID, "viewPos");
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	
	// Create Camera/View transformations
	glm::mat4 view;
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(objectShaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(objectShaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(objectShaderProgramID, "projection");
	// Pass the matrices to the shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	// Note: currently we set the projection matrix each frame, but since the projection matrix rarely 
	//changes it's often best practice to set it outside the main loop only once.
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


	// ------ draw cube objects -----------------------------------------------------------------
	// Bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, containerDiffuseMapID);
	// Bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, containerSpecularMapID);

	glBindVertexArray(CUBE_VAO);
	for (GLuint i = 0; i < 6; i++)
	{
		//float j = i + 10.0f;
		glm::mat4 model;
		model = glm::translate(model, outsideShelvingPositions[i]);
		//GLfloat angle = 20.0f + j;
		model = glm::scale(model, glm::vec3(10.0f, 6.0f, 1.0f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, cube_vertex_count);
	}
	
	
	// ------ draw four walls -----------------------------------------------------------------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blankTextID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(GROUND_VAO);
	for (GLuint i = 0; i < 4; i++)
	{
		glm::mat4 model;
		model = glm::translate(model, wallPositions[i]);
		model = glm::rotate(model, (i*90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(60.0f, 15.0f, 15.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, ground_vertex_count);
	}

	// ------ draw ground ---------------------------------------------------------------------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTextID);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0, -35.0, -4.0));
	model = glm::rotate(model, (90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, ground_vertex_count);
	

	// ------ draw ceiling ---------------------------------------------------------------------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blankTextID);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0, -20.0, -4.0));
	model = glm::rotate(model, (90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(60.0f, 60.0f, 60.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, ground_vertex_count);


	// ------ draw light source(s) ------------------------------------------------------------
	glUseProgram(lightingShaderProgramID);

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(LIGHT_VAO);
	for (GLuint i = 0; i < 24; i++)
	{
		model = glm::mat4();
		model = glm::translate(model, lightPositions[i]);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 3.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, cube_vertex_count);
	}
	
	// Swap the screen buffers
	glutSwapBuffers();
	glutPostRedisplay();
}

void idle() {

	// Draw the next frame
	glutPostRedisplay();
}

#pragma endregion DISPLAY_FUNCTIONS



void init() {

	//wireframe mode:
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//set projection
	projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

	// make sure keyboard arrays are init to all false
	memset(keys, 0, sizeof(bool) * 256);
	memset(specKeys, 0, sizeof(bool) * 256);

	// Set up the shaders
	objectShaderProgramID = CompileShaders(OBJECT_VERTEX_SHADER, OBJECT_FRAGMENT_SHADER);
	lightingShaderProgramID = CompileShaders(LIGHTING_VERTEX_SHADER, LIGHTING_FRAGMENT_SHADER);

	// Generate textures
	// tex locations
	const char wall_texture[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/wall.jpg";
	const char cube_texture[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/container.jpg";
	const char ground_texture[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/supermarket-wall-clean.jpg";
	const char cube_specular[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/container_specular.png";
	const char cube_diffuse[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/container.png";
	const char blank[] = "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/blank.jpg";

	generateTexture(wall_texture, &wallTextID);
	generateTexture(cube_texture, &cubeTextID);
	generateTexture(ground_texture, &groundTextID);
	generateTextureMap(cube_specular, &containerSpecularMapID);
	generateTextureMap(cube_diffuse, &containerDiffuseMapID);
	generateTexture(blank, &blankTextID);
	
	// Put the vertices and colors into a vertex buffer object
	generateObjectBuffers(&CUBE_VAO, cube_vertices, cube_vertex_count);
	generateObjectBuffers(&GROUND_VAO, ground_vertices, ground_vertex_count);
	generateLightObjectBuffers(&LIGHT_VAO, cube_vertices, cube_vertex_count);

	glUseProgram(objectShaderProgramID);

	// Set lights properties
	// set light properties in objectFragShader
	glUniform1f(glGetUniformLocation(objectShaderProgramID, "material.shininess"), 32.0f);
	glUniform1i(glGetUniformLocation(objectShaderProgramID, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(objectShaderProgramID, "material.specular"), 1);

	//Pass point lights to fragment shader
	for (GLuint i = 0; i < 24; i++) {

		std::string string1 = "pointLights[i].position";
		std::string string2 = "pointLights[i].ambient";
		std::string string3 = "pointLights[i].diffuse";
		std::string string4 = "pointLights[i].specular";
		std::string string5 = "pointLights[i].constant";
		std::string string6 = "pointLights[i].linear";
		std::string string7 = "pointLights[i].quadratic";

		string1.replace(12, 1, to_string(i));
		string2.replace(12, 1, to_string(i));
		string3.replace(12, 1, to_string(i));
		string4.replace(12, 1, to_string(i));
		string5.replace(12, 1, to_string(i));
		string6.replace(12, 1, to_string(i));
		string7.replace(12, 1, to_string(i));

		glUniform3f(glGetUniformLocation(objectShaderProgramID, string1.c_str()), lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
		glUniform3f(glGetUniformLocation(objectShaderProgramID, string2.c_str()), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(objectShaderProgramID, string3.c_str()), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(objectShaderProgramID, string4.c_str()), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(objectShaderProgramID, string5.c_str()), 1.0f);
		glUniform1f(glGetUniformLocation(objectShaderProgramID, string6.c_str()), 0.09);
		glUniform1f(glGetUniformLocation(objectShaderProgramID, string7.c_str()), 0.032);
	}


}

int main(int argc, char** argv) {
	// Set up the window
	glutInit(&argc, argv);
	//glut double means we are using two buffers, one is drawn to screen,
	//other is where we render stuff to draw
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	//using version 3.3
	glutInitContextVersion(3, 3);

	//let glut know we want to use core profile
	//we'll be notified if we try use legacy functions
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//note we can't set fixed size window as with glfw

	//create window object
	glutCreateWindow("Hello Triangle");

	//glew manages function pointers for openGL, 
	//NB** need to init before calling any openGL fucntions.

	//setting glewExp to true ensures GLEW uses modern techniques
	// for managing openGL, important for core profile (default is false)
	glewExperimental = GL_TRUE;

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	// Tell glut where the display function is
	glutDisplayFunc(display);
	//glutIdleFunc(idle);

	glutKeyboardFunc(keypress);
	glutSpecialFunc(specialKeyPress);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc(specialKeyUp);

	// Can check how many vertex attribs are supported in GPU shaders
	GLint nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;


	// Set up your objects and shaders
	init();


	// Begin infinite event loop
	// draw/idle/callbacks etc will be called in here
	glutMainLoop();

	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &CUBE_VAO);
	glDeleteBuffers(1, &GROUND_VAO);

	return 0;
}