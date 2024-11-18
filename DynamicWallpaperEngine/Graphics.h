#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Graphics {
public:
    GLFWwindow* window;

    void Initialize();   // Initialize GLFW and OpenGL
    void Render();       // Render objects and animation
};

#endif // GRAPHICS_H
