#include "Follower.h"

Follower::Follower(Vector2 position, Vector2 scale,
                   const char* textureFilepath) :
    Enemy(position, scale, textureFilepath) { }

Follower::Follower(Vector2 position, Vector2 scale, const char* textureFilepath,
                   TextureType textureType, Vector2 spriteSheetDimensions,
                   std::map<AnimationState, std::vector<int>> animationAtlas) :
    Enemy(position, scale, textureFilepath, textureType, spriteSheetDimensions,
          animationAtlas) {
    this->setSpeed(67.0f); // LOL
    this->setFrameRate(4.0f);
    this->setFlipDirection(RIGHT);
    this->setColliderDimensions(this->getScale() - Vector2 {0, 34.0f});
    this->setColliderOffset({0.0f, 17.0f});
}

void Follower::AIActivate(float deltaTime, Map* map) {
    if (mTarget == nullptr) return;

    float distance = Vector2Distance(mPosition, mTarget->getPosition());

    switch (mAIState) {
    case AI_IDLE :
        if (distance < mFollowRange) mAIState = AI_FOLLOW;
        break;

    case AI_FOLLOW :
        AIFollow(map);
        if (distance > mLooseRange) mAIState = AI_IDLE;
        break;

    default :
        break;
    }
}