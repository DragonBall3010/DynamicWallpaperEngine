#include "WallpaperEngine.h"
#include "Graphics.h"

WallpaperEngine::WallpaperEngine() {
    graphics = new Graphics();
    graphics->Initialize();
}

void WallpaperEngine::Run() {
    while (!glfwWindowShouldClose(graphics->window)) {
        graphics->Render();
    }
}

WallpaperEngine::~WallpaperEngine() {
    delete graphics;
}
