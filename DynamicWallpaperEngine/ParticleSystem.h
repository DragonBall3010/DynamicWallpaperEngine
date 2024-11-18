#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Define a Particle structure
struct Particle {
    float x, y;  // Position
    float velocityX, velocityY;  // Velocity
    float lifetime;  // Lifetime
    float size;  // Size of the particle
    float r, g, b, a;  // Color (RGBA)
};

class ParticleSystem {
public:
    ParticleSystem(int maxParticles);
    ~ParticleSystem();

    void Update(float deltaTime);
    void Render();
    void AddParticle(float x, float y);

private:
    std::vector<Particle> particles;
    int maxParticles;

    GLuint particleVAO, particleVBO;
    GLuint particleShader;

    const char* particleVertexShaderSource;
    const char* particleFragmentShaderSource;

    GLuint CompileShader(const char* shaderSource, GLenum shaderType);
    GLuint CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
    void InitShaders();
};

#endif
