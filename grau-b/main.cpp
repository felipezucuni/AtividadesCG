#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>
#include "Shader.h"
#include "stb_image.h"

using namespace std;

struct Vertex {
	glm::vec3 position;
	glm::vec3 v_color;
};

struct Material {
	string name;
	float Ns;
	float Ka[3];
	float Ks[3];
	float Ke[3];
	float Ni;
	float d;
	int illum;
	string map_Kd;
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int loadTexture(string path);
int loadSimpleOBJ(string filePath, int& nVertices, glm::vec3 color);
vector<Material> loadMTL(string filePath);

const GLuint WIDTH = 1000, HEIGHT = 1000;

glm::vec3 cameraPos1 = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraPos2 = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraPos3 = glm::vec3(0.0f, 0.0f, 5.0f);

glm::vec3 cameraFront1 = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraFront2 = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraFront3 = glm::vec3(0.0f, 0.0f, -1.0f);

glm::vec3 cameraUp1 = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraUp2 = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraUp3 = glm::vec3(0.0f, 1.0f, 0.0f);

string mtlLibPath = "";
char rotateChar;
float deltaTime, lastFrame, lastX, lastY, yaw = -90, pitch, speed = 0.1f;
float fov = 45.0f;
bool firstMouse = true;
int selectedModel = 2;

vector <Vertex> vertices;
vector <GLuint> indices;
vector <glm::vec3> normals;
vector <glm::vec2> texCoords;

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

	int nVertices1, nVertices2, nVertices3;

	// Carregar três modelos
	GLuint VAO1 = loadSimpleOBJ("models/cube.obj", nVertices1, glm::vec3(1.0, 0.0, 0.0));
	vector<Material> mtlInfo1 = loadMTL(mtlLibPath);
	GLuint texID1 = loadTexture(mtlInfo1[0].map_Kd);

	GLuint VAO2 = loadSimpleOBJ("models/planeta.obj", nVertices2, glm::vec3(0.0, 1.0, 0.0));
	vector<Material> mtlInfo2 = loadMTL(mtlLibPath);
	GLuint texID2 = loadTexture(mtlInfo2[0].map_Kd);

	GLuint VAO3 = loadSimpleOBJ("models/SuzanneTriTextured.obj", nVertices3, glm::vec3(0.0, 0.0, 1.0));
	vector<Material> mtlInfo3 = loadMTL(mtlLibPath);
	GLuint texID3 = loadTexture(mtlInfo3[0].map_Kd);

	Shader shader1("shaders/Phong.vs", "shaders/Phong.fs");
	shader1.Use();
	glUniform1i(glGetUniformLocation(shader1.ID, "tex_coord"), 0);
	GLint viewLoc1 = glGetUniformLocation(shader1.ID, "view");
	GLint projLoc1 = glGetUniformLocation(shader1.ID, "projection");
	shader1.setFloat("ka", mtlInfo1[0].Ka[0]);
	shader1.setFloat("kd", 1.0f);
	shader1.setFloat("ks", mtlInfo1[0].Ks[0]);
	shader1.setFloat("n", 0.0f);
	shader1.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	Shader shader2("shaders/Phong.vs", "shaders/Phong.fs");
	shader2.Use();
	glUniform1i(glGetUniformLocation(shader2.ID, "tex_coord"), 0);
	GLint viewLoc2 = glGetUniformLocation(shader2.ID, "view");
	GLint projLoc2 = glGetUniformLocation(shader2.ID, "projection");
	shader2.setFloat("ka", mtlInfo2[0].Ka[0]);
	shader2.setFloat("kd", 1.0f);
	shader2.setFloat("ks", mtlInfo2[0].Ks[0]);
	shader2.setFloat("n", 0.0f);
	shader2.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	Shader shader3("shaders/Phong.vs", "shaders/Phong.fs");
	shader3.Use();
	glUniform1i(glGetUniformLocation(shader3.ID, "tex_coord"), 0);
	GLint viewLoc3 = glGetUniformLocation(shader3.ID, "view");
	GLint projLoc3 = glGetUniformLocation(shader3.ID, "projection");
	shader3.setFloat("ka", mtlInfo3[0].Ka[0]);
	shader3.setFloat("kd", 1.0f);
	shader3.setFloat("ks", mtlInfo3[0].Ks[0]);
	shader3.setFloat("n", 0.0f);
	shader3.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	glm::vec3 lightPos1 = glm::vec3(-2.0f, 10.0f, 2.0f);
	glm::vec3 lightPos2 = glm::vec3(-2.0f, 10.0f, 2.0f);
	glm::vec3 lightPos3 = glm::vec3(-2.0f, 10.0f, 2.0f);

	glEnable(GL_DEPTH_TEST);

	glm::mat4 model1 = glm::mat4(1);
	glm::mat4 model2 = glm::mat4(1);
	glm::mat4 model3 = glm::mat4(1);

	model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.0f, 0.0f)); // Posicionar o primeiro cubo para a esquerda
	model2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Posicionar o segundo cubo para a direita
	model3 = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -1.0f, 0.0f));  // Posicionar o segundo cubo para a direita

	glm::mat4 view1;
	glm::mat4 view2;
	glm::mat4 view3;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float angle = (GLfloat)glfwGetTime();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();

		glm::mat4* selectedModelMatrix = nullptr;

		glm::mat4 projection1 = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glm::mat4 projection2 = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glm::mat4 projection3 = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		GLint modelLoc1 = glGetUniformLocation(shader1.ID, "model");
		GLint modelLoc2 = glGetUniformLocation(shader2.ID, "model");
		GLint modelLoc3 = glGetUniformLocation(shader3.ID, "model");

		switch (selectedModel) {
			case 1:
				view1 = glm::lookAt(cameraPos1, cameraPos1 + cameraFront1, cameraUp1);
				shader1.setVec3("cameraPos", cameraPos1.x, cameraPos1.y, cameraPos1.z);
				shader1.setVec3("lightPos", lightPos1.x + cameraPos1.x, lightPos1.y + cameraPos1.y, lightPos1.z + cameraPos1.z);
				break;
			case 2:
				view2 = glm::lookAt(cameraPos2, cameraPos2 + cameraFront2, cameraUp2);
				shader2.setVec3("cameraPos", cameraPos2.x, cameraPos2.y, cameraPos2.z);
				shader2.setVec3("lightPos", lightPos2.x + cameraPos1.x, lightPos2.y + cameraPos1.y, lightPos2.z + cameraPos2.z);
				break;
			case 3:
				view3 = glm::lookAt(cameraPos3, cameraPos3 + cameraFront3, cameraUp3);
				shader3.setVec3("cameraPos", cameraPos3.x, cameraPos3.y, cameraPos3.z);
				shader3.setVec3("lightPos", lightPos3.x + cameraPos1.x, lightPos3.y + cameraPos3.y, lightPos3.z + cameraPos3.z);
				break;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			switch (selectedModel) {
				case 1:
					cameraPos1 += cameraFront1 * deltaTime;
					break;
				case 2:
					cameraPos2 += cameraFront2 * deltaTime;
					break;
				case 3:
					cameraPos3 += cameraFront3 * deltaTime;
					break;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			switch (selectedModel) {
				case 1:
					cameraPos1 -= cameraFront1 * deltaTime;
					break;
				case 2:
					cameraPos2 -= cameraFront2 * deltaTime;
					break;
				case 3:
					cameraPos3 -= cameraFront3 * deltaTime;
					break;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			switch (selectedModel) {
				case 1:
					cameraPos1 -= glm::normalize(glm::cross(cameraFront1, cameraUp1)) * deltaTime;
					break;
				case 2:
					cameraPos2 -= glm::normalize(glm::cross(cameraFront2, cameraUp2)) * deltaTime;
					break;
				case 3:
					cameraPos3 -= glm::normalize(glm::cross(cameraFront3, cameraUp3)) * deltaTime;
					break;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			switch (selectedModel) {
				case 1:
					cameraPos1 += glm::normalize(glm::cross(cameraFront1, cameraUp1)) * deltaTime;
					break;
				case 2:
					cameraPos2 += glm::normalize(glm::cross(cameraFront2, cameraUp2)) * deltaTime;
					break;
				case 3:
					cameraPos3 += glm::normalize(glm::cross(cameraFront3, cameraUp3)) * deltaTime;
					break;
			}
		}

		// Aplica rotação ao modelo selecionado
		switch (selectedModel) {
			case 1:
				selectedModelMatrix = &model1;
				break;
			case 2:
				selectedModelMatrix = &model2;
				break;
			case 3:
				selectedModelMatrix = &model3;
				break;
		}

		if (selectedModelMatrix) {
			switch (rotateChar) {
				case 'X':
					*selectedModelMatrix = glm::rotate(*selectedModelMatrix, glm::radians(speed), glm::vec3(1.0f, 0.0f, 0.0f));
					break;
				case 'Y':
					*selectedModelMatrix = glm::rotate(*selectedModelMatrix, glm::radians(speed), glm::vec3(0.0f, 1.0f, 0.0f));
					break;
				case 'Z':
					*selectedModelMatrix = glm::rotate(*selectedModelMatrix, glm::radians(speed), glm::vec3(0.0f, 0.0f, 1.0f));
					break;
			}
		}

		glUniformMatrix4fv(projLoc1, 1, GL_FALSE, glm::value_ptr(projection1));
		glUniformMatrix4fv(viewLoc1, 1, GL_FALSE, glm::value_ptr(view1));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID1);
		glUniformMatrix4fv(modelLoc1, 1, GL_FALSE, glm::value_ptr(model1));
		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, nVertices1);
		glBindVertexArray(0);

		glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection2));
		glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view2));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID2);
		glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, nVertices2);
		glBindVertexArray(0);

		glUniformMatrix4fv(projLoc3, 1, GL_FALSE, glm::value_ptr(projection3));
		glUniformMatrix4fv(viewLoc3, 1, GL_FALSE, glm::value_ptr(view3));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID3);
		glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model3));
		glBindVertexArray(VAO3);
		glDrawArrays(GL_TRIANGLES, 0, nVertices3);
		glBindVertexArray(0);

		//TEXTURE
		glBindVertexArray(0); //unbind - desconecta
		glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura
		//TEXTURE

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteVertexArrays(1, &VAO3);
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float cameraSpeed = 10.0f * deltaTime;

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_1:
			selectedModel = 1;
			break;

		case GLFW_KEY_2:
			selectedModel = 2;
			break;

		case GLFW_KEY_3:
			selectedModel = 3;
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
			switch (selectedModel) {
				case 1:
					cameraPos1 += cameraSpeed * cameraFront1;
					break;
				case 2:
					cameraPos2 += cameraSpeed * cameraFront2;
					break;
				case 3:
					cameraPos3 += cameraSpeed * cameraFront3;
					break;
			}
			
			break;
		case GLFW_KEY_S:
			switch (selectedModel) {
				case 1:
					cameraPos1 -= cameraSpeed * cameraFront1;
					break;
				case 2:
					cameraPos2 -= cameraSpeed * cameraFront2;
					break;
				case 3:
					cameraPos3 -= cameraSpeed * cameraFront3;
					break;
			}

			break;
		case GLFW_KEY_A:
			switch (selectedModel) {
				case 1:
					cameraPos1 -= glm::normalize(glm::cross(cameraFront1, cameraUp1)) * cameraSpeed;
					break;
				case 2:
					cameraPos2 -= glm::normalize(glm::cross(cameraFront2, cameraUp3)) * cameraSpeed;
					break;
				case 3:
					cameraPos3 -= glm::normalize(glm::cross(cameraFront3, cameraUp3)) * cameraSpeed;
					break;
			}
			
			break;
		case GLFW_KEY_D:
			switch (selectedModel) {
				case 1:
					cameraPos1 += glm::normalize(glm::cross(cameraFront1, cameraUp1)) * cameraSpeed;
					break;
				case 2:
					cameraPos2 += glm::normalize(glm::cross(cameraFront2, cameraUp3)) * cameraSpeed;
					break;
				case 3:
					cameraPos3 += glm::normalize(glm::cross(cameraFront3, cameraUp3)) * cameraSpeed;
					break;
			}

			break;
		}
	}
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

	switch (selectedModel) {
	case 1:
		cameraFront1 = glm::normalize(front);
		break;
	case 2:
		cameraFront2 = glm::normalize(front);
		break;
	case 3:
		cameraFront3 = glm::normalize(front);
		break;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset * 0.1;
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