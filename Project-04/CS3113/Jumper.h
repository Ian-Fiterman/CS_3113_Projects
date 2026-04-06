#ifndef JUMPER_H
#define JUMPER_H

#include "Enemy.h"

class Jumper : public Enemy {
public:
    Jumper(Vector2 position, Vector2 scale, const char* textureFilepath);
    Jumper(Vector2 position, Vector2 scale, const char* textureFilepath,
           TextureType textureType, Vector2 spriteSheetDimensions,
           std::map<AnimationState, std::vector<int>> animationAtlas);

    void setJumpInterval(float interval) { mJumpInterval = interval; }

private:
    float mJumpInterval {2.0f};
    float mJumpTimer {0.0f};

    void AIActivate(float deltaTime, Map* map) override;
};

#endif // JUMPER_H