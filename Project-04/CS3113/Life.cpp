#include "Life.h"

Life::Life(Vector2 position, Vector2 scale, const char* texturePath,
           Vector2 sheetDimensions, int index, int& lives) :
    texture {LoadTexture(texturePath)}, position {position}, scale {scale},
    sheetDimensions {sheetDimensions}, index {index}, lives {lives} { }

void Life::render() {
    int frame = (index < lives) ? FULL_FRAME : EMPTY_FRAME;

    Rectangle textureArea =
        getUVRectangle(&texture, frame, sheetDimensions.x, sheetDimensions.y);

    Rectangle destinationArea = {position.x, position.y, scale.x, scale.y};

    Vector2 originOffset = {scale.x / 2.0f, scale.y / 2.0f};

    DrawTexturePro(texture, textureArea, destinationArea, originOffset, 0.0f,
                   WHITE);
}