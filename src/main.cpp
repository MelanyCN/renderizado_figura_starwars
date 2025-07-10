#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "model_loader.hpp"
#include "shader_loader.hpp"
#include "texture_loader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // --------------------------------------------
    // INICIALIZACIÓN DE GLFW Y GLAD
    // --------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Render STARWARS", nullptr, nullptr);
    if (!window) {
        std::cerr << "No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "No se pudo inicializar GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST); // Necesario para objetos 3D

    // --------------------------------------------
    // 1. CARGAR EL MODELO Y TEXTURA
    // --------------------------------------------
    std::vector<float> vertices;

    if (!loadOBJ("star-war/anlustarwars.obj", vertices)) {
        std::cerr << "Error cargando el modelo.\n";
        return -1;
    }
    std::cout << "Total vértices cargados: " << vertices.size() / 3 << std::endl;

    GLuint texture = loadTexture("star-war/anlustarwars.jpg");


    // --------------------------------------------
    // 2. CONFIGURAR VAO/VBO
    // --------------------------------------------
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --------------------------------------------
    // 3. CREAR SHADERS
    // --------------------------------------------
    GLuint shaderProgram = loadShaderProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    glUseProgram(shaderProgram);

    // --------------------------------------------
    // 4. CALCULAR MATRIZ MVP
    // --------------------------------------------
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // gira en eje X
    model = glm::scale(model, glm::vec3(0.05f)); // lo hace más pequeño
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f),       // Cámara en Z+
                                 glm::vec3(0.0f, 0.0f, 0.0f),       // Mira al origen
                                 glm::vec3(0.0f, 1.0f, 0.0f));      // Arriba = +Y
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            800.0f / 600.0f,
                                            0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;

    // --------------------------------------------
    // 5. PASAR MATRIZ MVP COMO UNIFORM A LOS SHADERS
    // --------------------------------------------
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    // --------------------------------------------
    // 6. BUCLE DE RENDERIZADO
    // --------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // Limpiar pantalla
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Dibujar
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);

        // Actualizar ventana
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
