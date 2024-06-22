#pragma once

//GLM
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>

#include "Shader.h"


class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	void initialize(GLuint VAO, int nVertices, Shader* shader, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0), float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0));
	void update();
	void draw();

protected:
	GLuint VAO; //Identificador do Vertex Array Object - V rtices e seus atributos
	int nVertices;

	//Informa  es sobre as transforma  es a serem aplicadas no objeto
	glm::vec3 position;
	glm::vec3 scale;
	float angle;
	glm::vec3 axis;

	//Refer ncia (endere o) do shader
	Shader* shader;

};

