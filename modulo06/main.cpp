#include <iostream>
#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>
#include "Shader.h"

using namespace std;

//TO LOAD TEXTURE
#include "stb_image.h"

//TO CREATE TRAJECTORY 
#include <random>
#include <algorithm>
#include "Bezier.h"


struct Vertex {
	glm::vec3 position;
	glm::vec3 v_color;
};

struct Material {
	std::string name;
	float Ns;
	float Ka[3];
	float Ks[3];
	float Ke[3];
	float Ni;
	float d;
	int illum;
	std::string map_Kd;
};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//TEXTURE
int loadTexture(string path);
//TEXTURE

int loadSimpleOBJ(string filePath, int& nVertices, glm::vec3 color);
vector<Material> loadMTL(string filePath);
string mtlLibPath = "";
const GLuint WIDTH = 750, HEIGHT = 750;

char rotateChar;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime, lastFrame, lastX, lastY, yaw = -90, pitch;
float fov = 45.0f;
bool firstMouse = true;

vector <Vertex> vertices;
vector <GLuint> indices;
vector <glm::vec3> normals;
vector <glm::vec2> texCoords;

//FUN��ES TRAGET�RIA
std::vector<glm::vec3> readControlPointsFromFile(const std::string& filename);

int main() {
	// Inicialização da GLFW
	if (!glfwInit()) {
		cerr << "Não foi possivel inicializar o GLFW" << endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Olá 3D -- Luis Felipe Zucuni Trindade!", nullptr, nullptr);
	if (!window) {
		cerr << "Não foi possível abrir janela com a GLFW" << endl;
		glfwTerminate();
		return -2;
	}
	glfwMakeContextCurrent(window);

	// GLAD: carrega todos os ponteiros das funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cerr << "Failed to initialize GLAD" << endl;
		return -1;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "*** Dados OpenGL ************" << endl;
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("shaders/Phong.vs", "shaders/Phong.fs");
	shader.Use();

	int nVertices;
	
	//POINTS AND CURVES
	std::vector<glm::vec3> controlPoints = readControlPointsFromFile("controlPoints.txt");

	int pointsPerSegment = 2000;
	Bezier bezier;
	bezier.setControlPoints(controlPoints);
	bezier.setShader(&shader);
	bezier.generateCurve(pointsPerSegment);
	
	bool forward = true;

	int nbCurvePoints = bezier.getNbCurvePoints();
	int i = 0;

	//POINTS AND CURVES

	//TEXTURE
	GLuint VAO = loadSimpleOBJ("models/suzanneTriTextured.obj", nVertices, glm::vec3(0,0,0));
	glUniform1i(glGetUniformLocation(shader.ID, "tex_coord"), 0);
	//TEXTURE
	vector<Material> mtlInfo = loadMTL(mtlLibPath);

	//LOAD TEXTURE
	GLuint texID = loadTexture(mtlInfo[0].map_Kd); //LOAD FROM MTL FILE
	//LOAD TEXTURE

	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	GLint projLoc = glGetUniformLocation(shader.ID, "projection");

	shader.setFloat("ka", mtlInfo[0].Ka[0]*1.5);
	shader.setFloat("kd", 1.0f);
	shader.setFloat("ks", mtlInfo[0].Ks[0]*1.5);
	shader.setFloat("n", 0.0f);

	shader.setVec3("lightPos", 0.0, 0.0, 32.0);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float angle = (GLfloat) glfwGetTime();

		glm::mat4 model = glm::mat4(1);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glm::mat4 view = glm::lookAt(cameraPos,
			cameraPos + cameraFront,
			cameraUp);

		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
		
		glm::mat4 projection = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// MOVE OBJECT ON CURVE
		
		glm::vec3 pointOnCurve = bezier.getPointOnCurve(i);
		model = glm::translate(model, pointOnCurve);
		
		if (i == 0) forward = true;
		
		if(forward)
			i = (i + 1) % nbCurvePoints;
		else
			i = (i - 1) % nbCurvePoints;

		if (i == 500) forward = false;
		
		// MOVE OBJECT ON CURVE

		switch (rotateChar) {
			case 'X':
				model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case 'Y':
				model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
				break;

			case 'Z':
				model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
				break;

			case '1':
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case '2':
				model = glm::rotate(model, glm::radians(90.0f) * 2, glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case '3':
				model = glm::rotate(model, glm::radians(90.0f) * 3, glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case '4':
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				break;

			case '5':
				model = glm::rotate(model, glm::radians(90.0f) * 3, glm::vec3(0.0f, 1.0f, 0.0f));
				break;
		}

		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//TEXTURE
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		//TEXTURE

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, nVertices);
		
		glBindVertexArray(0);

		//TEXTURE
		glBindVertexArray(0); //unbind - desconecta
		glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura
		//TEXTURE

		glfwSwapBuffers(window);
	}
	
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	
	float cameraSpeed = 100.0f * deltaTime;

	switch (key) {

		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_X:
			rotateChar = 'X';
			break;

		case GLFW_KEY_Y:
			rotateChar = 'Y';
			break;

		case GLFW_KEY_Z:
			rotateChar = 'Z';
			break;

		case GLFW_KEY_W:
			cameraPos += cameraSpeed * cameraFront;
			break;

		case GLFW_KEY_S:
			cameraPos -= cameraSpeed * cameraFront;
			break;

		case GLFW_KEY_A:
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;

		case GLFW_KEY_D:
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;

		case GLFW_KEY_1:
			rotateChar = '1';
			break;

		case GLFW_KEY_2:
			rotateChar = '2';
			break;

		case GLFW_KEY_3:
			rotateChar = '3';
			break;

		case GLFW_KEY_4:
			rotateChar = '4';
			break;

		case GLFW_KEY_5:
			rotateChar = '5';
			break;
		default:
			rotateChar = NULL;
	}
}

std::vector<glm::vec3> readControlPointsFromFile(const std::string& filename)
{
	std::vector<glm::vec3> controlPoints;

	std::ifstream file(filename);
	if (!file)
	{
		std::cerr << "Falha ao abrir o arquivo " << filename << std::endl;
		return controlPoints;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("Point: (") != std::string::npos)
		{
			// Extrair os valores das coordenadas
			size_t startPos = line.find("(") + 1;
			size_t endPos = line.find(")");
			std::string pointStr = line.substr(startPos, endPos - startPos);

			std::stringstream ss(pointStr);
			glm::vec3 point;
			char comma;
			ss >> point.x >> comma >> point.y >> comma >> point.z;
			controlPoints.push_back(point);
		}
	}

	file.close();
	return controlPoints;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset*0.1;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

vector<Material> loadMTL(string filepath) {
	vector<Material> materials;
	ifstream file(filepath);

	if (!file.is_open()) {
		cout << "Failed to open MTL file: " << filepath << std::endl;
		return materials;
	}

	Material currentMaterial;
	string line;

	while (std::getline(file, line)) {
		istringstream iss(line);
		string prefix;
		iss >> prefix;

		if (prefix == "newmtl") {
			if (!currentMaterial.name.empty()) {
				materials.push_back(currentMaterial);
			}
			iss >> currentMaterial.name;
		}
		else if (prefix == "Ns") {
			iss >> currentMaterial.Ns;
		}
		else if (prefix == "Ka") {
			iss >> currentMaterial.Ka[0] >> currentMaterial.Ka[1] >> currentMaterial.Ka[2];
		}
		else if (prefix == "Ks") {
			iss >> currentMaterial.Ks[0] >> currentMaterial.Ks[1] >> currentMaterial.Ks[2];
		}
		else if (prefix == "Ke") {
			iss >> currentMaterial.Ke[0] >> currentMaterial.Ke[1] >> currentMaterial.Ke[2];
		}
		else if (prefix == "Ni") {
			iss >> currentMaterial.Ni;
		}
		else if (prefix == "d") {
			iss >> currentMaterial.d;
		}
		else if (prefix == "illum") {
			iss >> currentMaterial.illum;
		}
		else if (prefix == "map_Kd") {
			iss >> currentMaterial.map_Kd;
		}
	}

	if (!currentMaterial.name.empty()) {
		materials.push_back(currentMaterial);
	}

	file.close();

	return materials;
}

int loadTexture(string path)
{
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color)
{
	vector <glm::vec3> vertices;
	vector <GLuint> indices;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> normals;
	vector <GLfloat> vbuffer;

	ifstream inputFile;
	inputFile.open(filepath.c_str());
	if (inputFile.is_open())
	{
		char line[100];
		string sline;



		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			sline = line;

			string word;

			istringstream ssline(line);
			ssline >> word;
			if (word == "mtllib") 
			{
				//LOAD MTL FILE ADDRESS
				ssline >> mtlLibPath;
				cout << mtlLibPath << std::endl;
			}
			if (word == "v")
			{
				glm::vec3 v;
				ssline >> v.x >> v.y >> v.z;


				vertices.push_back(v);
			}
			if (word == "vt")
			{
				glm::vec2 vt;
				ssline >> vt.s >> vt.t;

				texCoords.push_back(vt);
			}
			if (word == "vn")
			{
				glm::vec3 vn;
				ssline >> vn.x >> vn.y >> vn.z;
				normals.push_back(vn);
			}
			if (word == "f")
			{
				string tokens[3];

				ssline >> tokens[0] >> tokens[1] >> tokens[2];

				for (int i = 0; i < 3; i++)
				{
					int pos = tokens[i].find("/");
					string token = tokens[i].substr(0, pos);
					int index = atoi(token.c_str()) - 1;
					indices.push_back(index);

					vbuffer.push_back(vertices[index].x);
					vbuffer.push_back(vertices[index].y);
					vbuffer.push_back(vertices[index].z);
					vbuffer.push_back(color.r);
					vbuffer.push_back(color.g);
					vbuffer.push_back(color.b);

					tokens[i] = tokens[i].substr(pos + 1);
					pos = tokens[i].find("/");
					token = tokens[i].substr(0, pos);
					index = atoi(token.c_str()) - 1;
					vbuffer.push_back(texCoords[index].s);
					vbuffer.push_back(texCoords[index].t);

					tokens[i] = tokens[i].substr(pos + 1);
					index = atoi(tokens[i].c_str()) - 1;
					vbuffer.push_back(normals[index].x);
					vbuffer.push_back(normals[index].y);
					vbuffer.push_back(normals[index].z);
				}
			}
		}
	}
	else
	{
		cout << "Problema ao encontrar o arquivo " << filepath << endl;
	}
	inputFile.close();
	GLuint VBO, VAO;
	nVerts = vbuffer.size() / 11; // 3 pos + 3 cor + 3 normal + 2 texcoord

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return VAO;
}

