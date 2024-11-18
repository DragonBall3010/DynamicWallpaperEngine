#ifndef WALLPAPERENGINE_H
#define WALLPAPERENGINE_H

#include "Graphics.h"

class WallpaperEngine {
public:
    Graphics* graphics;

    WallpaperEngine();
    void Run();    // Run the wallpaper engine
    ~WallpaperEngine();
};

#endif // WALLPAPERENGINE_H
