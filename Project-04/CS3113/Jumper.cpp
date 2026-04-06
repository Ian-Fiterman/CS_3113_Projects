#include "Jumper.h"

Jumper::Jumper(Vector2 position, Vector2 scale, const char* textureFilepath) :
    Enemy(position, scale, textureFilepath) { }

Jumper::Jumper(Vector2 position, Vector2 scale, const char* textureFilepath,
               TextureType textureType, Vector2 spriteSheetDimensions,
               std::map<AnimationState, std::vector<int>> animationAtlas) :
    Enemy(position, scale, textureFilepath, textureType, spriteSheetDimensions,
          animationAtlas) { }

void Jumper::AIActivate(float deltaTime, Map* map) {
    if (isCollidingBottom()) mJumpTimer -= deltaTime;

    if (mJumpTimer <= 0.0f) {
        mJumpTimer = mJumpInterval;
        AIJump(map);
        mAIState = AI_JUMP;
    }

    if (isCollidingBottom()) mAIState = AI_IDLE;
}