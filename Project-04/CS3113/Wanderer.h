#ifndef WANDERER_H
#define WANDERER_H

#include "Enemy.h"

class Wanderer : public Enemy {
public:
    Wanderer(Vector2 position, Vector2 scale, const char* textureFilepath);
    Wanderer(Vector2 position, Vector2 scale, const char* textureFilepath,
             TextureType textureType, Vector2 spriteSheetDimensions,
             std::map<AnimationState, std::vector<int>> animationAtlas);

private:
    void AIActivate(float deltaTime, Map* map) override;
};

#endif // WANDERER_H