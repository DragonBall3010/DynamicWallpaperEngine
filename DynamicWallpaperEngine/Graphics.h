#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>

class Graphics {
public:
    void Initialize();
    void Render();


    GLFWwindow* window;
};

#endif // GRAPHICS_H
