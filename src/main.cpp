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


float camAngle = 0.0f;   // ángulo de rotación horizontal
float camX = 0.0f;       // desplazamiento en X
float camY = 0.0f;       // desplazamiento en Y
float camZoom = 5.0f;   // distancia de la cámara (zoom)


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

/*
    FUNCIONALIDADES CON EL MOUSE -----------------------------------------
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = 400, lastY = 300;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camAngle += xoffset * 0.005;  
        camY += -yoffset * 0.02;     
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        camX += xoffset * 0.01;       // pan horizontal
        camY += -yoffset * 0.01;      // pan vertical
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camZoom -= yoffset;  
    if (camZoom < 0.01f) camZoom = 0.01f;     // límite mínimo de zoom
    if (camZoom > 100.0f) camZoom = 100.0f; // límite máximo de zoom
}

int main() {
    // --------------------------------------------------------------------------------------
    /*    
        ### INICIALIZACIÓN DE GLFW Y GLAD
        Crea ventana y habilita profundidad para objetos 3D
    */
    // --------------------------------------------------------------------------------------

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

    glEnable(GL_DEPTH_TEST); 

    // -------------------------------------------------------------------------------------
    /*
        ### CALLBACKS DEL MOUSE
        Activa funciones para que respondan al movimiento del mouse
        que por el momento simula la camara en realidad virtual
    */
    // -------------------------------------------------------------------------------------
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // --------------------------------------------------------------------------------------
    /*
        ### CARGA DEL MODELO Y TEXTURA
        loadOBJ -> carga posiciones y texturas de vértices desde .obj
        loadTexture -> carga imagen .jpg como textura OpenGL
    */
    // -------------------------------------------------------------------------------------
    std::vector<float> vertices;

    if (!loadOBJ("star-war/anlustarwars.obj", vertices)) {
        std::cerr << "Error cargando el modelo.\n";
        return -1;
    }
    std::cout << "Total vértices cargados: " << vertices.size() / 3 << std::endl;

    GLuint texture = loadTexture("star-war/anlustarwars.jpg");


    // ------------------------------------------------------------------------------------
    /*
        ### CONFIGURACION DE BUFFERS VAO/VBO
        VAO: guarda cómo interpretar los datos (atributos como posición, textura).
        VBO: guarda los datos de los vértices (posiciones, texturas, normales...).
    */
    // ------------------------------------------------------------------------------------
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Atributo 0 -> posición del vértice (x, y, z)
    glVertexAttribPointer(
        0,                              // layout(location = 0) en vertex shader
        3,                              // 3 valores (x, y, z)
        GL_FLOAT,                       // tipo de dato
        GL_FALSE,                       // no normalizar
        5 * sizeof(float),              // stride = cada vértice tiene 5 floats: 3 posición + 2 textura
        (void*)0                        // offset = empieza desde el primer float
    );
    glEnableVertexAttribArray(0);

    // Atributo 1 -> coordenadas de textura (u, v)
    glVertexAttribPointer(
        1,                              // layout(location = 1) en vertex shader
        2,                              // 2 valores (u, v)
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))      // empieza desde el 4to float: después de x,y,z
    );
    glEnableVertexAttribArray(1);

    // --------------------------------------------------------------------------------------
    /*
        ### CREACIÓN DE SHADERS
        loadShaderProgram -> compila vertex y fragment shader
        vertex y fragment shader Son pequeños programas que corren en GPU
    */
    // --------------------------------------------------------------------------------------
    GLuint shaderProgram = loadShaderProgram("shaders/vertex_shader.glsl", 
                                             "shaders/fragment_shader.glsl");

    // -------------------------------------------------------------------------------------
    /*
        ### CÁLCULO DE LA MATRIZ MVP
        define cómo se transforma el modelo 3D desde su espacio local hasta la pantalla.
        MVP = Model * View * Projection
        Aquí se define la parte del "Model".
    */
    // -------------------------------------------------------------------------------------
    glm::mat4 model = glm::mat4(1.0f);

    // 1. Traslada el modelo 1 unidad hacia abajo en Y (para centrarlo en la vista)
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    // 2. Rota el modelo 180° en el eje X (inversión vertical: común con escaneos 3D)
    // PROFE
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // 3. Escala uniforme: reduce el modelo al 1% de su tamaño original
    model = glm::scale(model, glm::vec3(0.01f));

    // -------------------------------------------------------------------------------------
    /*
        ### PASA MATRIZ MVP COMO UNIFORM A LOS SHADERS
        Esta línea obtiene la dirección (ubicación) del uniforme 'mvp' dentro del shader,
        para poder actualizar su valor desde C++ cada vez que cambie la cámara o el modelo.
    */
    // -------------------------------------------------------------------------------------
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");

    // -------------------------------------------------------------------------------------
    /*
        ### BUCLE DE RENDERIZADO
        1. se limpian los buffers
        2. se calcula la cámara dinámica
        3. se actualiza la matriz MVP
        4. se dibuja el modelo en pantalla con la textura aplicada.
    */
    // -------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // --- LIMPIEZA DE PANTALLA ---
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);               // color de fondo: azul oscuro
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // --- ACTIVAR SHADER PROGRAM ---
        glUseProgram(shaderProgram);

        // --- CONFIGURACIÓN DE LA CÁMARA DINÁMICA ---
        glm::vec3 modelCenter(0.0f, -1.0f, 0.0f);                       // centro base del modelo
        glm::vec3 target = modelCenter + glm::vec3(camX, camY, 0.0f);   // punto al que mira la cámara

        // posición de la cámara calculada en forma circular usando camAngle
        float camPosX = target.x + camZoom * sin(camAngle);             // desplazamiento horizontal
        float camPosZ = target.z + camZoom * cos(camAngle);             // desplazamiento en profundidad
        glm::vec3 cameraPos(camPosX, target.y, camPosZ);                // posición final de la cámara

        glm::mat4 view = glm::lookAt(
            cameraPos,                  // desde dónde mira la cámara
            target,                     // hacia dónde mira
            glm::vec3(0.0f, 1.0f, 0.0f) // vector 'arriba' (Y+)
        );

        // --- MATRIZ DE PROYECCIÓN ---
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),        // ángulo de visión vertical
            800.0f / 600.0f,            // proporción de aspecto (ancho/alto)
            0.1f, 100.0f                // plano cercano y lejano
        );

        // --- MATRIZ MVP Y ENVÍO AL SHADER ---
        glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp)); 

        // --- DIBUJAR MODELO ---
        glBindTexture(GL_TEXTURE_2D, texture);              // se activa la textura
        glBindVertexArray(VAO);                             // se activa el arreglo de vértices (atributos)
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5); // se dibujan los triángulos

        // --- INTERCAMBIO DE BUFFERS Y EVENTOS ---
        glfwSwapBuffers(window); // muestra en pantalla el frame actual
        glfwPollEvents();        // escucha eventos del teclado, mouse, etc.
    }

    // Limpieza
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
