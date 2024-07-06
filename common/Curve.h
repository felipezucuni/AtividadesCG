//GLM
#pragma once

#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>

#include <vector> 

#include "Shader.h"

using namespace std;

class Curve
{
public:
	Curve() {}
	inline void setControlPoints(vector <glm::vec3> controlPoints) { this->controlPoints = controlPoints; }
	void setShader(Shader* shader);
	void generateCurve(int pointsPerSegment);
	void drawCurve(glm::vec4 color);
	int getNbCurvePoints() { return curvePoints.size(); }
	glm::vec3 getPointOnCurve(int i) { return curvePoints[i]; }
protected:
	vector <glm::vec3> controlPoints;
	vector <glm::vec3> curvePoints;
	glm::mat4 M; //Matriz de base
	GLuint VAO;
	Shader* shader;
};

