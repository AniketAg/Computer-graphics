#pragma once

#include <GL/glm/glm/gtc/matrix_transform.hpp>

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(3.0f,  5.0f, -15.0f),
	glm::vec3(-3.5f, -2.2f, -2.5f),
	glm::vec3(-5.8f, -2.0f, -12.3f),
	glm::vec3(5.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(4.3f, -2.0f, -2.5f),
	glm::vec3(2.1f,  1.2f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-2.9f,  1.0f, -1.5f),

	glm::vec3(2.4f, -1.6f, -2.5f),
	glm::vec3(2.6f,  2.0f, -1.5f),
	glm::vec3(-2.5f,  -1.0f, -1.5f),
	glm::vec3(-4.3f,  1.0f, -1.5f)
};

glm::vec3 wallPositions[] = {
	glm::vec3(0.0, 2.5, -26.5),
	glm::vec3(-22.5, 2.5, -4.0),
	glm::vec3(0.0, 2.5, 18.5),
	glm::vec3(22.5, 2.5, -4.0)
};

glm::vec3 lightPositions[] = {

	glm::vec3(22.5f, 9.0f, -29.0f),
	//glm::vec3(10.0f, 9.0f, -16.0f),
	glm::vec3(7.5f, 9.0f, -29.0f),
	//glm::vec3(0.0f, 9.0f, -16.0f),
	glm::vec3(-7.5f, 9.0f, -29.0f),
	//glm::vec3(-10.0f, 9.0f, -16.0f),
	glm::vec3(-22.5f, 9.0f, -29.0f),

	glm::vec3(22.5f, 9.0f, -19.0f),
	//glm::vec3(10.0f, 9.0f, -21.0f),
	glm::vec3(7.5f, 9.0f, -19.0f),
	//glm::vec3(0.0f, 9.0f, -21.0f),
	glm::vec3(-7.5f, 9.0f, -19.0f),
	//glm::vec3(-10.2f, 9.0f, -21.0f),
	glm::vec3(-22.5f, 9.0f, -19.0f),

	glm::vec3(22.5f, 9.0f, -9.0f),
	//glm::vec3(10.0f, 9.0f, -11.0f),
	glm::vec3(7.5f, 9.0f, -9.0f),
	//glm::vec3(0.0f, 9.0f, -11.0f),
	glm::vec3(-7.5f, 9.0f, -9.0f),
	//glm::vec3(-10.2f, 9.0f, -11.0f),
	glm::vec3(-22.5f, 9.0f, -9.0f),


	glm::vec3(22.5f, 9.0f, 1.0f),
	//glm::vec3(10.0f, 9.0f, -1.0f),
	glm::vec3(7.5f, 9.0f, 1.0f),
	//glm::vec3(0.0f, 9.0f, -1.0f),
	glm::vec3(-7.5f, 9.0f, 1.0f),
	//glm::vec3(-10.0f, 9.0f, -1.0f),
	glm::vec3(-22.5f, 9.0f, 1.0f),

	glm::vec3(22.5f, 9.0f, 11.0f),
	//glm::vec3(10.0f, 9.0f, 9.0f),
	glm::vec3(7.5f, 9.0f, 11.0f),
	//glm::vec3(0.0f, 9.0f, 9.0f),
	glm::vec3(-7.5f, 9.0f, 11.0f),
	//glm::vec3(-10.0f, 9.0f, 9.0f),
	glm::vec3(-22.5f, 9.0f, 11.0f),
	
	glm::vec3(22.5f, 9.0f, 21.0f),
	//glm::vec3(10.0f, 9.0f, 14.0f),
	glm::vec3(7.5f, 9.0f, 21.0f),
	//glm::vec3(0.0f, 9.0f, 14.0f),
	glm::vec3(-7.5f, 9.0f, 21.0f),
	//glm::vec3(-10.0f, 9.0f, 14.0f),
	glm::vec3(-22.5f, 9.0f, 21.0f),

};


glm::vec3 outsideShelvingPositions[] = {

	//back wall
	glm::vec3(25.0f, -2.0f, -33.5f),
	glm::vec3(15.0f, -2.0f, -33.5f),
	glm::vec3(5.0f, -2.0f, -33.5f),
	glm::vec3(-5.0f, -2.0f, -33.5f),
	glm::vec3(-15.0f, -4.5f, -33.5f),
	glm::vec3(-25.0f, -4.5f, -33.5f),


	glm::vec3(22.5f, 9.0f, -19.0f),
	//glm::vec3(10.0f, 9.0f, -21.0f),
	glm::vec3(7.5f, 9.0f, -19.0f),
	//glm::vec3(0.0f, 9.0f, -21.0f),
	glm::vec3(-7.5f, 9.0f, -19.0f),
	//glm::vec3(-10.2f, 9.0f, -21.0f),
	glm::vec3(-22.5f, 9.0f, -19.0f),

	glm::vec3(22.5f, 9.0f, -9.0f),
	//glm::vec3(10.0f, 9.0f, -11.0f),
	glm::vec3(7.5f, 9.0f, -9.0f),
	//glm::vec3(0.0f, 9.0f, -11.0f),
	glm::vec3(-7.5f, 9.0f, -9.0f),
	//glm::vec3(-10.2f, 9.0f, -11.0f),
	glm::vec3(-22.5f, 9.0f, -9.0f),


	glm::vec3(22.5f, 9.0f, 1.0f),
	//glm::vec3(10.0f, 9.0f, -1.0f),
	glm::vec3(7.5f, 9.0f, 1.0f),
	//glm::vec3(0.0f, 9.0f, -1.0f),
	glm::vec3(-7.5f, 9.0f, 1.0f),
	//glm::vec3(-10.0f, 9.0f, -1.0f),
	glm::vec3(-22.5f, 9.0f, 1.0f),

	glm::vec3(22.5f, 9.0f, 11.0f),
	//glm::vec3(10.0f, 9.0f, 9.0f),
	glm::vec3(7.5f, 9.0f, 11.0f),
	//glm::vec3(0.0f, 9.0f, 9.0f),
	glm::vec3(-7.5f, 9.0f, 11.0f),
	//glm::vec3(-10.0f, 9.0f, 9.0f),
	glm::vec3(-22.5f, 9.0f, 11.0f),

	glm::vec3(22.5f, 9.0f, 21.0f),
	//glm::vec3(10.0f, 9.0f, 14.0f),
	glm::vec3(7.5f, 9.0f, 21.0f),
	//glm::vec3(0.0f, 9.0f, 14.0f),
	glm::vec3(-7.5f, 9.0f, 21.0f),
	//glm::vec3(-10.0f, 9.0f, 14.0f),
	glm::vec3(-22.5f, 9.0f, 21.0f),

};
