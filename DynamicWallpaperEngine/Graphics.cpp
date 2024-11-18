#include "Graphics.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath> // For sine wave animation

// Vertex Shader Source
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position; // vertex position
void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color
}
)";

// Particle system structures
struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;
};

std::vector<Particle> particles;

GLuint CompileShader(const char* shaderSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader); // Clean up shaders after linking
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void GenerateParticles() {
    for (int i = 0; i < 100; ++i) {
        Particle p;
        p.position = glm::vec3(0.0f, 0.0f, 0.0f); // start at center
        p.velocity = glm::vec3(((rand() % 100) / 50.0f) - 1.0f, // random velocity
            ((rand() % 100) / 50.0f) - 1.0f,
            0.0f);
        p.lifetime = 5.0f; // 5 seconds lifespan
        particles.push_back(p);
    }
}

void UpdateParticles(float deltaTime) {
    for (auto& p : particles) {
        p.position += p.velocity * deltaTime;
        p.lifetime -= deltaTime;
    }

    // Remove expired particles
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.lifetime <= 0.0f; }), particles.end());
}

void Graphics::Render() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Use our shader program
    GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(shaderProgram);

    // Define a simple rectangle (two triangles to form a square)
    float vertices[] = {
        // positions        
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    // Create and bind the VAO and VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Animate the object by modifying its Y position
    static float offset = 0.0f;
    float timeValue = glfwGetTime();
    offset = 0.5f * sin(timeValue); // Smooth animation

    // Update the vertex positions with the offset for vertical movement
    float animatedVertices[] = {
        // positions        
        -0.5f, -0.5f + offset, 0.0f,
         0.5f, -0.5f + offset, 0.0f,
         0.5f,  0.5f + offset, 0.0f,
        -0.5f,  0.5f + offset, 0.0f
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(animatedVertices), animatedVertices); // Update VBO data

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Draw the rectangle

    glBindVertexArray(0);

    // Update particles and render them
    static float lastTime = glfwGetTime();
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    UpdateParticles(deltaTime);

    // Render each particle
    for (const auto& p : particles) {
        float particleVertices[] = {
            p.position.x - 0.05f, p.position.y - 0.05f, 0.0f,
            p.position.x + 0.05f, p.position.y - 0.05f, 0.0f,
            p.position.x + 0.05f, p.position.y + 0.05f, 0.0f,
            p.position.x - 0.05f, p.position.y + 0.05f, 0.0f
        };

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particleVertices), particleVertices); // Update VBO data
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Draw the particle
    }

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Graphics::Initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Dynamic Wallpaper Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed!" << std::endl;
        return;
    }

    // Set up OpenGL state
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Generate initial particles
    GenerateParticles();
}
