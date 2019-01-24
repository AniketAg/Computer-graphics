#pragma once
float ground_vertex_count = 6;

float ground_vertices[] = {

	// First triangle		//norms				//tex
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0,		0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0,		1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,	 0.0f, 0.0f, -1.0,		 1.0f, 1.0f,

	// Second triangle
	0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0,		1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0,	0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0,	0.0f, 0.0f,

	/*
	// First triangle		//tex
	0.5f,  0.5f, 0.0f,		1.0f, 1.0f, // Top Right
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f, // Bottom Right
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,  // Top Left 
	// Second triangle
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f, // Bottom Right
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, // Bottom Left
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f  // Top Left
	*/

};

float ground_colors[] = {
	
	1.0f, 1.0f, 0.0f,	// Top Right
	1.0f, 0.0f, 0.0f,	// Bottom Right
	1.0f, 0.0f, 0.0f,	// Top Left

	1.0f, 0.0f, 0.0f,	// Bottom Right
	1.0f, 1.0f, 0.0f,	// Bottom Left
	1.0f, 1.0f, 0.0f	// Top left
};
