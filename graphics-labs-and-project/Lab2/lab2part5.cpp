#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <GL/maths_funcs.h>

class Transform {
private:
	GLfloat rotate_x, rotate_y, rotate_z;
	GLfloat x, y, z;
	GLfloat posVar, scaleVar;
	GLfloat temp_x, temp_y, temp_z;

	mat4 transformation = identity_mat4();
	mat4 eye = identity_mat4();

public:
	Transform() {
		this->scaleVar = 0.0f;
		this->posVar = this->x = this->y = this->z = 0;
	}

	void uniformScale(GLfloat scale) {
		transformation.m[0] += scale;
		transformation.m[5] += scale;
		transformation.m[10] += scale;
	}

	void nonUniformScale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ) {
		transformation.m[0] += scaleX;
		transformation.m[5] += scaleY;
		transformation.m[10] += scaleZ;
	}

	void translateOrigin() {
		this->temp_x = this->x;
		this->temp_y = this->y;
		this->temp_z = this->z;

		transformation.m[3] = 0;
		transformation.m[7] = 0;
		transformation.m[11] = 0;
	}

	void resetTranslation() {
		transformation.m[3] = this->temp_x;
		transformation.m[7] = this->temp_y;
		transformation.m[11] = this->temp_z;
	}

	void translateToPos(GLfloat x, GLfloat y, GLfloat z) {
		transformation.m[3] = x;
		transformation.m[7] = y;
		transformation.m[11] = z;
	}

	void translate(GLfloat x, GLfloat y, GLfloat z) {
		transformation.m[3] += x;
		transformation.m[7] += y;
		transformation.m[11] += z;
	}

	void rotateX(GLfloat angle) {
		rotate_x += angle;
		transformation.m[5] = cos(rotate_x);
		transformation.m[6] = sin(rotate_x) * -1;
		transformation.m[9] = sin(rotate_x);
		transformation.m[10] = cos(rotate_x);
	}

	void rotateY(GLfloat angle) {
		rotate_y += angle;
		transformation.m[0] = cos(rotate_y);
		transformation.m[2] = sin(rotate_y);
		transformation.m[8] = sin(rotate_y) * -1;
		transformation.m[10] = cos(rotate_y);
	}

	void rotateZ(GLfloat angle) {
		rotate_z += angle;
		transformation.m[0] = cos(rotate_z);
		transformation.m[1] = sin(rotate_z) * -1;
		transformation.m[4] = sin(rotate_z);
		transformation.m[5] = cos(rotate_z);
	}

	void reset() {
		rotate_x = rotate_y = rotate_z = posVar = scaleVar = 0.0f;
		transformation = identity_mat4();
	}

	void combinedTransformations() {
		rotate_x += 0.005f;
		scaleVar += 0.005f;
		posVar += 0.005f;

		mat4 translationTransformation = identity_mat4();
		translationTransformation.m[3] += posVar;
		translationTransformation.m[7] += posVar;
		translationTransformation.m[11] += posVar;

		mat4 scaledTransformation = identity_mat4();
		scaledTransformation.m[0] -= scaleVar;
		scaledTransformation.m[5] -= scaleVar;
		scaledTransformation.m[10] -= scaleVar;

		mat4 rotationTransformation = identity_mat4();
		rotationTransformation.m[5] = cos(rotate_x);
		rotationTransformation.m[6] = sin(rotate_x) * -1;
		rotationTransformation.m[9] = sin(rotate_x);
		rotationTransformation.m[10] = cos(rotate_x);

		transformation = translationTransformation * scaledTransformation * rotationTransformation;
	}

	mat4 getMatrix() { return this->transformation; }
};

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

// Vertex Shader (for convenience, it is defined in the main here, but we will be using text files for shaders in future)
// Note: Input to this shader is the vertex positions that we specified for the triangle. 
// Note: gl_Position is a special built-in variable that is supposed to contain the vertex position (in X, Y, Z, W)
// Since our triangle vertices were specified as vec3, we just set W to 1.0.
static const char* pVS = "                                                  \n\
#version 330                                                                \n\
                                                                            \n\
in vec3 vPosition;															\n\
in vec4 vColor;																\n\
uniform mat4 transformation;												\n\
out vec4 color;																\n\
                                                                            \n\
void main()                                                                 \n\
{																			\n\
	gl_Position = transformation * vec4(vPosition / 2, 1.0);				\n\
	color = vColor;															\n\
}";

// Fragment Shader
// Note: no input in this shader, it just outputs the colour of all fragments, in this case set to red (format: R, G, B, A).
static const char* pFS = "													\n\
#version 330																\n\
in vec4 color;																\n\
out vec4 FragColor;															\n\
																			\n\
void main()																	\n\
{																			\n\
	FragColor = color;														\n\
}";

GLuint shaderProgramID, transformationID;
Transform* t1 = new Transform();
Transform* t2 = new Transform();
Transform* t3 = new Transform();

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}
	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderText, NULL);
	// compile the shader and check for errors
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
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, pVS, GL_VERTEX_SHADER);
	AddShader(shaderProgramID, pFS, GL_FRAGMENT_SHADER);

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
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
GLuint generateObjectBuffer(GLfloat vertices[], GLfloat colors[]) {
	GLuint numVertices = 9;
	// Genderate 1 generic buffer object, called VBO
	GLuint VBO;
	glGenBuffers(1, &VBO);
	// In OpenGL, we bind (make active) the handle to a target name and then execute commands on that target
	// Buffer will contain an array of vertices 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// After binding, we now fill our object with data, everything in "Vertices" goes to the GPU
	glBufferData(GL_ARRAY_BUFFER, numVertices * 7 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	// if you have more data besides vertices (e.g., vertex colours or normals), use glBufferSubData to tell the buffer when the vertices array ends and when the colors start
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * 3 * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), numVertices * 4 * sizeof(GLfloat), colors);
	return VBO;
}

void linkCurrentBuffertoShader(GLuint shaderProgramID) {
	GLuint numVertices = 9;
	// find the location of the variables that we will be using in the shader program
	GLuint positionID = glGetAttribLocation(shaderProgramID, "vPosition");
	GLuint colorID = glGetAttribLocation(shaderProgramID, "vColor");
	transformationID = glGetUniformLocation(shaderProgramID, "transformation");

	// Tell it where to find the position data in the currently active buffer (at index positionID)
	glEnableVertexAttribArray(positionID);
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Similarly, for the color data.
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(numVertices * 3 * sizeof(GLfloat)));
}

#pragma endregion VBO_FUNCTIONS

// drawing functions for displaying to buffer
#pragma region DRAWING
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	//glClearColor(1, 1, 1, 1);
	// NB: Make the call to draw the geometry in the currently activated vertex buffer. This is where the GPU starts to work!
	//glUniformMatrix4fv(transformationID, 1, GL_TRUE, &transformation.m[0]);
	//glDrawArrays(GL_TRIANGLES, 0, 9);

	glUniformMatrix4fv(transformationID, 1, GL_TRUE, &t1->getMatrix().m[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glUniformMatrix4fv(transformationID, 1, GL_TRUE, &t2->getMatrix().m[0]);
	glDrawArrays(GL_TRIANGLES, 3, 3);

	glUniformMatrix4fv(transformationID, 1, GL_TRUE, &t3->getMatrix().m[0]);
	glDrawArrays(GL_TRIANGLES, 6, 3);

	glutSwapBuffers();
	glutPostRedisplay();
}

void init()
{
	// Create 3 vertices that make up a triangle that fits on the viewport 
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	// Create a color array that identfies the colors of each vertex (format R, G, B, A)
	GLfloat colors[] = {
		//1st triangle
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		//2nd triangle
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		//3rd triangle
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
	};
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// Put the vertices and colors into a vertex buffer object
	generateObjectBuffer(vertices, colors);
	// Link the current buffer to the shader
	linkCurrentBuffertoShader(shaderProgramID);

	t1->reset();
	t2->reset();
	t3->reset();
	t1->translateToPos(-0.5, -0.5, 0.0f);
	t2->translateToPos(0.5, -0.5, 0.0f);
	t3->translateToPos(0.0f, 0.5, 0.0f);
}
#pragma endregion DRAWING

void onKeyDown(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		t1->translate(0.0f, 0.05f, 0.0f);
		t2->rotateX(0.1f);
		t3->uniformScale(0.1f);
		break;
	case 's':
		t1->translate(0.0f, -0.05f, 0.0f);
		t2->rotateX(-0.1f);
		t3->uniformScale(-0.1f);
		break;
	case 'r':
		t1->reset();
		t2->reset();
		t3->reset();

		t1->translate(-0.5, -0.5, 0.0f);
		t2->translate(0.5, -0.5, 0.0f);
		t3->translate(0.0f, 0.5, 0.0f);
		break;
	case 'q':
		glutExit();
	default:
		break;
	}
}

void cleanUp() {
	delete t1;
	delete t2;
	delete t3;
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Lab 2 - Last Part");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutKeyboardFunc(onKeyDown);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();

	// delete allocated objects in memory to prevent leaks
	cleanUp();
	return 0;
}