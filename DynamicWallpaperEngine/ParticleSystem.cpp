#include "ParticleSystem.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

ParticleSystem::ParticleSystem(int maxParticles)
    : maxParticles(maxParticles) {

    srand(time(0));  // Seed random number generator

    // Initialize the shaders
    InitShaders();

    // Create VAO and VBO for the particles
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * maxParticles, nullptr, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, x));
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, lifetime));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, r));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &particleVAO);
    glDeleteBuffers(1, &particleVBO);
}

void ParticleSystem::InitShaders() {
    // Shader source code
    particleVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 position;
    layout (location = 1) in float lifetime;
    layout (location = 2) in vec4 color;
    
    out vec4 fragColor;
    
    void main()
    {
        gl_Position = vec4(position.x, position.y, 0.0, 1.0);
        fragColor = color;
    })";

    particleFragmentShaderSource = R"(
    #version 330 core
    in vec4 fragColor;
    out vec4 color;
    
    void main()
    {
        color = fragColor;
    })";

    // Compile shaders
    particleShader = CreateShaderProgram(particleVertexShaderSource, particleFragmentShaderSource);
}

GLuint ParticleSystem::CompileShader(const char* shaderSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint ParticleSystem::CreateShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void ParticleSystem::Update(float deltaTime) {
    // Add new particles
    for (int i = 0; i < 5; i++) {
        AddParticle(rand() % 800 - 400, rand() % 600 - 300);
    }

    // Update existing particles
    for (auto& particle : particles) {
        particle.x += particle.velocityX * deltaTime;
        particle.y += particle.velocityY * deltaTime;
        particle.lifetime -= deltaTime;

        if (particle.lifetime <= 0) {
            particle.lifetime = 0;
        }
    }

    // Remove expired particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return p.lifetime <= 0; }),
        particles.end());

    // Update VBO data
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * particles.size(), &particles[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::Render() {
    glUseProgram(particleShader);
    glBindVertexArray(particleVAO);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBindVertexArray(0);
}

void ParticleSystem::AddParticle(float x, float y) {
    if (particles.size() < maxParticles) {
        Particle p;
        p.x = x;
        p.y = y;
        p.velocityX = (rand() % 200 - 100) / 100.0f;
        p.velocityY = (rand() % 200 - 100) / 100.0f;
        p.lifetime = rand() % 5 + 1;
        p.size = rand() % 10 + 5;
        p.r = 1.0f;
        p.g = 1.0f;
        p.b = 1.0f;
        p.a = 1.0f;

        particles.push_back(p);
    }
}
