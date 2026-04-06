#ifndef FOLLOWER_H
#define FOLLOWER_H

#include "Enemy.h"

class Follower : public Enemy {
public:
    Follower(Vector2 position, Vector2 scale, const char* textureFilepath);
    Follower(Vector2 position, Vector2 scale, const char* textureFilepath,
             TextureType textureType, Vector2 spriteSheetDimensions,
             std::map<AnimationState, std::vector<int>> animationAtlas);

    void setFollowRange(float range) { mFollowRange = range; }

    void setLooseRange(float range) { mLooseRange = range; }

private:
    float mFollowRange {250.0f};
    float mLooseRange {350.0f};

    void AIActivate(float deltaTime, Map* map) override;
};

#endif // FOLLOWER_H