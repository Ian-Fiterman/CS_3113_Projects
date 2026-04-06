#ifndef LIFE_H
#define LIFE_H

#include "cs3113.h"

struct Life {
    Texture2D texture;
    Vector2   position;
    Vector2   scale;
    Vector2   sheetDimensions;
    int       index;
    int&      lives;

    static constexpr int FULL_FRAME  = 80;
    static constexpr int EMPTY_FRAME = 81;

    Life(Vector2 position, Vector2 scale, const char* texturePath,
         Vector2 sheetDimensions, int index, int& lives);

    ~Life() { UnloadTexture(texture); }

    void render();
};

#endif // LIFE_H