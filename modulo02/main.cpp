#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window);

// Protótipos das funções
int setupShader();
int setupGeometry();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const char* vertexShaderSource = R"(
                                    #version 330 core
                                    layout (location = 0) in vec3 aPos;
                                    layout (location = 1) in vec3 aColor;
                                    out vec3 ourColor;
                                    uniform mat4 model;
                                    uniform mat4 view;
                                    uniform mat4 projection;
                                    void main() {
                                        gl_Position = projection * view * model * vec4(aPos, 1.0);
                                        ourColor = aColor;
                                    }
                                )";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const char* fragmentShaderSource = R"(
                                        #version 330 core
                                        out vec4 FragColor;
                                        in vec3 ourColor;
                                        void main() {
                                            FragColor = vec4(ourColor, 1.0);
                                        }
                                    )";

GLfloat rotateX = 0.0f, rotateY = 0.0f, rotateZ = 0.0f, zoom = 45.0f;

int main() {
    // Inicialização da GLFW
    if (!glfwInit()) {
        cerr << "Não foi possivel inicializar o GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

    // Compilando e buildando o programa de shader
    GLuint shaderID = setupShader();

    // Gerando um buffer simples, com a geometria de um triângulo
    GLuint VAO = setupGeometry();

    glUseProgram(shaderID);

    glm::mat4 model1 = glm::mat4(1); //matriz identidade;
    glm::mat4 model2 = glm::mat4(1); //matriz identidade;
    
    model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // Posicionar o primeiro cubo para a esquerda
    model2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // Posicionar o segundo cubo para a direita

    GLuint modelLoc = glGetUniformLocation(shaderID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderID, "projection");

    glEnable(GL_DEPTH_TEST);

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window)) {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();
        key_callback(window);

        // Limpa o buffer de cor
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(10);

        // Construir matrizes de rotação
        glm::mat4 rotateXMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotateYMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotateZMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotateZ), glm::vec3(0.0f, 0.0f, 1.0f));

        // Aplicar rotações aos modelos
        glm::mat4 model1Rotated = model1 * rotateXMatrix * rotateYMatrix * rotateZMatrix;
        glm::mat4 model2Rotated = model2 * rotateXMatrix * rotateYMatrix * rotateZMatrix;

        // Configuração da câmera
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(zoom), 800.0f / 600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);

        // Desenhar o primeiro cubo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1Rotated));
        glDrawArrays(GL_TRIANGLES, 0 , 36);
        glDrawArrays(GL_POINTS, 0, 36);

        // Desenhar o segundo cubo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2Rotated));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDrawArrays(GL_POINTS, 0, 36);

        glBindVertexArray(0);

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }

    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);

    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();

    return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        rotateX += 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        rotateY += 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        rotateZ += 0.01f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        zoom -= 0.01f;
        if (zoom < 1.0f) {
            zoom = 1.0f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        zoom += 0.01f;
        if (zoom > 45.0f) {
            zoom = 45.0f;
        }
    }
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
    // Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
    // sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
    // Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
    // Pode ser armazenado em um VBO único ou em VBOs separados
    GLfloat vertices[] = {
        // x    y     z    r    g    b
        -0.5, -0.5, -0.5, 1.0, 0.0, 0.0, // pos0 = V1
         0.0, -0.5, -0.5, 1.0, 0.0, 0.0, // pos1 = V2 Base
         0.0,  0.0, -0.5, 1.0, 0.0, 0.0, // pos2 = V3

        -0.5, -0.5, -0.5, 1.0, 1.0, 0.0, // pos0 = V1
         0.0,  0.0, -0.5, 1.0, 1.0, 0.0, // pos2 = V3 Base
        -0.5,  0.0, -0.5, 1.0, 1.0, 0.0, // pos3 = V4

        -0.5, -0.5,  0.0, 1.0, 0.0, 0.0, // pos4 = V5
         0.0, -0.5,  0.0, 1.0, 0.0, 0.0, // pos5 = V6 Topo
         0.0,  0.0,  0.0, 1.0, 0.0, 0.0, // pos6 = V7

        -0.5, -0.5,  0.0, 1.0, 1.0, 0.0, // pos4 = V5
         0.0,  0.0,  0.0, 1.0, 1.0, 0.0, // pos6 = V7 Topo
        -0.5,  0.0,  0.0, 1.0, 1.0, 0.0, // pos7 = V8

        -0.5, -0.5, -0.5, 0.0, 0.0, 1.0, // pos0 = V1
        -0.5,  0.0, -0.5, 0.0, 0.0, 1.0, // pos3 = V4 Lado
        -0.5,  0.0,  0.0, 0.0, 0.0, 1.0, // pos7 = V8

        -0.5, -0.5, -0.5, 0.0, 1.0, 0.0, // pos0 = V1
        -0.5, -0.5,  0.0, 0.0, 1.0, 0.0, // pos4 = V5 Lado
        -0.5,  0.0,  0.0, 0.0, 1.0, 0.0, // pos7 = V8

         0.0, -0.5, -0.5, 0.0, 0.0, 1.0, // pos1 = V2
         0.0, -0.5,  0.0, 0.0, 0.0, 1.0, // pos5 = V6 Lado
         0.0,  0.0,  0.0, 0.0, 0.0, 1.0, // pos6 = V7

         0.0, -0.5, -0.5, 0.0, 1.0, 0.0, // pos1 = V2
         0.0,  0.0, -0.5, 0.0, 1.0, 0.0, // pos2 = V3 Lado
         0.0,  0.0,  0.0, 0.0, 1.0, 0.0, // pos6 = V7

        -0.5, -0.5, -0.5, 0.0, 1.0, 1.0, // pos0 = V1
         0.0, -0.5, -0.5, 0.0, 1.0, 1.0, // pos1 = V2 Frente
         0.0, -0.5,  0.0, 0.0, 1.0, 1.0, // pos5 = V6

        -0.5, -0.5, -0.5, 1.0, 0.0, 1.0, // pos0 = V1
        -0.5, -0.5,  0.0, 1.0, 0.0, 1.0, // pos4 = V5 Frente
         0.0, -0.5,  0.0, 1.0, 0.0, 1.0, // pos5 = V6

        -0.5,  0.0, -0.5, 1.0, 0.0, 1.0, // pos3 = V4
         0.0,  0.0, -0.5, 1.0, 0.0, 1.0, // pos2 = V3 Atras
         0.0,  0.0,  0.0, 1.0, 0.0, 1.0, // pos6 = V7

        -0.5,  0.0, -0.5, 0.0, 1.0, 1.0, // pos3 = V4
         0.0,  0.0,  0.0, 0.0, 1.0, 1.0, // pos6 = V7 Atras
        -0.5,  0.0,  0.0, 0.0, 1.0, 1.0, // pos7 = V8
    };

    GLuint VBO, VAO;

    //Geração do identificador do VBO, EBO e VAO
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    //Faz a conexão (vincula) do buffer como um buffer de array e buffer de index
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //Envia os dados do array de floats para o buffer da OpenGl
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
    // e os ponteiros para os atributos 
    glBindVertexArray(VAO);

    // Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
    // Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
    // Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
    // Tipo do dado
    // Se está normalizado (entre zero e um)
    // Tamanho em bytes 
    // Deslocamento a partir do byte zero 

    //Atributo posição (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Atributo cor (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
    // atualmente vinculado - para que depois possamos desvincular com segurança
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
    glBindVertexArray(0);

    return VAO;
}

// Esta função está bastante hardcoded - objetivo é compilar e "buildar" um programa de
// shader simples e único neste exemplo de código
// O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
// fragmentShader source no inicio deste arquivo
// A função retorna o identificador do programa de shader
int setupShader()
{
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Checando erros de compilação (exibição via log no terminal)
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Checando erros de compilação (exibição via log no terminal)
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Linkando os shaders e criando o identificador do programa de shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Checando por erros de linkagem
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}