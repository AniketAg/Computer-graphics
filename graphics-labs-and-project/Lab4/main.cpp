// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Project includes
#include "maths_funcs.h"

using namespace std;

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define MESH_NAME "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/skull.dae"
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/

#pragma region SimpleTypes
typedef struct
{
	size_t mPointCount = 0;
	 vector<vec3> mVertices;
	 vector<vec3> mNormals;
	 vector<vec2> mTextureCoords;
} ModelData;
#pragma endregion SimpleTypes

using namespace std;
GLuint shaderProgramID;

ModelData mesh_data;
unsigned int mesh_vao = 0;
int width = 800;
int height = 600;

GLuint loc1, loc2, loc3;
GLfloat move_z = -30.0f;
GLfloat rotate_faster = 0.0f;
GLfloat rotate_childd = 0.0f;
mat4 temp = identity_mat4();
mat4 key = identity_mat4();
mat4 temp1 = identity_mat4();

#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name) {
	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */
	const aiScene* scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
				/* You can extract tangents and bitangents here              */
				/* Note that you might need to make Assimp generate this     */
				/* data for you. Take a look at the flags that aiImportFile  */
				/* can take.                                                 */
			}
		}
	}

	aiReleaseImport(scene);
	return modelData;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		 cerr << "Error creating shader..." <<  endl;
		 cerr << "Press enter/return to exit..." <<  endl;
		 cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		 cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog <<  endl;
		 cerr << "Press enter/return to exit..." <<  endl;
		 cin.get();
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
		 cerr << "Error creating shader program..." <<  endl;
		 cerr << "Press enter/return to exit..." <<  endl;
		 cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "C:/Users/Aniket Agarwal/Desktop/Tcd study/4th Year/Computer Graphics/Lab_1_2018/Lab 1 2018/Lab 1/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		 cerr << "Error linking shader program: " << ErrorLog <<  endl;
		 cerr << "Press enter/return to exit..." <<  endl;
		 cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		 cerr << "Invalid shader program: " << ErrorLog <<  endl;
		 cerr << "Press enter/return to exit..." <<  endl;
		 cin.get();
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
void generateObjectBufferMesh() {
	/*----------------------------------------------------------------------------
	LOAD MESH HERE AND COPY INTO BUFFERS
	----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	mesh_data = load_mesh(MESH_NAME);
	unsigned int vp_vbo = 0;
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);

	//	This is for texture coordinates which you don't currently need, so I have commented it out
	//	unsigned int vt_vbo = 0;
	//	glGenBuffers (1, &vt_vbo);
	//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	//	glBufferData (GL_ARRAY_BUFFER, monkey_head_data.mTextureCoords * sizeof (vec2), &monkey_head_data.mTextureCoords[0], GL_STATIC_DRAW);

	unsigned int vao = 0;
	glBindVertexArray(vao);

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//	This is for texture coordinates which you don't currently need, so I have commented it out
	//	glEnableVertexAttribArray (loc3);
	//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	//	glVertexAttribPointer (loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}
#pragma endregion VBO_FUNCTIONS


void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.2f, 0.2f, 0.2f, 0.6f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);


	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");


	// Root of the Hierarchy
	mat4 view = identity_mat4();
	mat4 persp_proj = perspective(30.0f, (float)width / (float)height, 0.2f, 1000.0f);
	mat4 model = identity_mat4();
	//model = rotate_x_deg(model, rotate_y);
	//model = translate(model, vec3(0.0, 0.0, -5.0f));
	view = translate(view, vec3(0.0, 0.0, -5.0f));
	mat4 global = model * key;
	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	//global = translate(global, vec3(0.0, 0.0, move_z));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	// modelchildd of global
	mat4 modelchildd = identity_mat4();
	//modelchildd = rotate_y_deg(modelchildd, rotate_faster);
	modelchildd = rotate_y_deg(modelchildd, 2.5 * rotate_faster);
	modelchildd = translate(modelchildd, vec3(0.0f, 0.60f, 0.0f));
	modelchildd = global * modelchildd;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelchildd.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	// modelchildd1 of global
	mat4 modelchildd1 = temp;
	modelchildd1 = rotate_z_deg(modelchildd1, 125);
	modelchildd1 = rotate_y_deg(modelchildd1, rotate_childd);
	//modelchildd1 = rotate_x_deg(modelchildd1, rotate_childd);
	//modelchildd1 = rotate_y_deg(modelchildd1, 2.5 * rotate_faster);
	modelchildd1 = translate(modelchildd1, vec3(-0.60f, 0.0f, 0.0f));
	modelchildd1 = global * modelchildd1;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelchildd1.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//modelchildd2 of global
	mat4 modelchildd2 = temp1;
	modelchildd2 = rotate_z_deg(modelchildd2, 240);
	//modelchildd2 = rotate_x_deg(modelchildd2, 2 * rotate_faster);
	modelchildd2 = translate(modelchildd2, vec3(0.60f, 0.0f, 0.0f));
	modelchildd2 = global * modelchildd2;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelchildd2.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);



	//childd 3 of modelchildd
	mat4 childd3 = identity_mat4();
	//childd3 = rotate_y_deg(childd3, rotate_y);
	//childd3 = rotate_z_deg(childd3, rotate_y);
	childd3 = translate(childd3, vec3(0.60f, 0.80f, 0.0f));
	childd3 = scale(childd3, vec3(.80f, 0.8f, 0.8f));
	childd3 = modelchildd * childd3;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd3.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//childd 4 of modelchildd1
	mat4 childd4 = identity_mat4();
	//childd4 = rotate_x_deg(childd4, rotate_y);
	//childd4 = rotate_z_deg(childd4, rotate_y);
	childd4 = translate(childd4, vec3(-0.60f, 0.80f, 0.0f));
	childd4 = scale(childd4, vec3(.80f, 0.8f, 0.8f));
	childd4 = modelchildd * childd4;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd4.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//childd 4.5 of modelchildd1
	mat4 childd4_5 = identity_mat4();
	//childd4 = rotate_x_deg(childd4, rotate_y);
	//childd4 = rotate_z_deg(childd4, rotate_y);
	childd4_5 = translate(childd4_5, vec3(0.0f, 0.90f, 0.0f));
	childd4_5 = scale(childd4_5, vec3(.80f, 0.8f, 0.8f));
	childd4_5 = modelchildd * childd4_5;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd4_5.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//childd 5 of modelchildd
	mat4 childd5 = temp;
	//childd5 = rotate_y_deg(childd3, rotate_y);
	//childd5 = rotate_z_deg(childd3, rotate_y);
	childd5 = translate(childd5, vec3(-0.60f, 0.80f, 0.0f));
	childd5 = scale(childd5, vec3(.80f, 0.8f, 0.8f));
	childd5 = modelchildd1 * childd5;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd5.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//childd 6 of modelchildd1
	mat4 childd6 = temp;
	//childd4 = rotate_x_deg(childd4, rotate_y);
	//childd4 = rotate_z_deg(childd4, rotate_y);
	childd6 = translate(childd6, vec3(0.60f, 0.80f, 0.0f));
	childd6 = scale(childd6, vec3(.80f, 0.8f, 0.8f));
	childd6 = modelchildd1 * childd6;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd6.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//childd 6.5 of modelchildd1
	mat4 childd6_5 = temp;
	//childd4 = rotate_x_deg(childd4, rotate_y);
	//childd4 = rotate_z_deg(childd4, rotate_y);
	childd6_5 = translate(childd6_5, vec3(0.0f, 0.90f, 0.0f));
	childd6_5 = scale(childd6_5, vec3(.80f, 0.8f, 0.8f));
	childd6_5 = modelchildd1 * childd6_5;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd6_5.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//childd 7 of modelchildd
	mat4 childd7 = temp1;
	//childd5 = rotate_y_deg(childd3, rotate_y);
	//childd5 = rotate_z_deg(childd3, rotate_y);
	childd7 = translate(childd7, vec3(0.60f, 0.80f, 0.0f));
	childd7 = scale(childd7, vec3(.80f, 0.8f, 0.8f));
	childd7 = modelchildd2 * childd7;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd7.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//childd 8 of modelchildd1
	mat4 childd8 = temp1;
	//childd4 = rotate_x_deg(childd4, rotate_y);
	//childd4 = rotate_z_deg(childd4, rotate_y);
	childd8 = translate(childd8, vec3(-0.60f, 0.80f, 0.0f));
	childd8 = scale(childd8, vec3(.80f, 0.8f, 0.8f));
	childd8 = modelchildd2 * childd8;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd8.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	//childd 8 of modelchildd1
	mat4 childd8_5 = temp1;
	//childd4 = rotate_x_deg(childd4, rotate_y);
	//childd4 = rotate_z_deg(childd4, rotate_y);
	childd8_5 = translate(childd8_5, vec3(0.0f, 0.90f, 0.0f));
	childd8_5 = scale(childd8_5, vec3(.80f, 0.8f, 0.8f));
	childd8_5 = modelchildd2 * childd8_5;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, childd8_5.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
	glutSwapBuffers();
}


void updateScene() {

	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	// Rotate the model slowly around the y axis at 20 degrees per second
	move_z += 5.0f * delta;
	if (move_z >= 0.0f)
		move_z = -100.0f;
	rotate_faster += 100.0f * delta;
	rotate_faster = fmodf(rotate_faster, 720.0f);
	rotate_childd += 20.0f * delta;

	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// load mesh into a vertex buffer array
	generateObjectBufferMesh();

}

// Placeholder code for the keypress
void keypress(unsigned char key1, int x, int y) {
	switch (key1) {
	case 'w':	//positve x
		key = translate(key, vec3(0.20f, 0.0f, 0.0f));
		break;
	case 's':	//negative x
		key = translate(key, vec3(-0.20f, 0.0f, 0.0f));
		break;
	case 'q':	//positive z
		key = translate(key, vec3(0.0f, 0.0f, 0.20f));
		break;
	case 'e':	//negative z
		key = translate(key, vec3(0.0f, 0.0f, -0.20f));
		break;
	case 'a':	//positive y
		key = translate(key, vec3(0.0f, 0.20f, 0.0f));
		break;
	case 'd':	//negative y
		key = translate(key, vec3(0.0f, -0.20f, 0.0f));
		break;
	case 'z':	//postive z
		key = rotate_z_deg(key, 10.0f);
		break;
	case 'c':	//negative z
		key = rotate_z_deg(key, -10.0f);
	case 'x':	//negative z
		temp1 = rotate_z_deg(temp1, -10.0f);
		break;
	default:
		break;
	}

}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Parent childd");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);

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
	return 0;
}