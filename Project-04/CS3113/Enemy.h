#ifndef NPC_H
#define NPC_H

#include "Entity.h"

enum AIState { AI_IDLE, AI_WALK, AI_FOLLOW, AI_JUMP };

class Enemy : public Entity {
public:
    Enemy(Vector2 position, Vector2 scale, const char* textureFilepath);
    Enemy(Vector2 position, Vector2 scale, const char* textureFilepath,
          TextureType textureType, Vector2 spriteSheetDimensions,
          std::map<AnimationState, std::vector<int>> animationAtlas);

    virtual void update(float deltaTime, Map* map);

    void setTarget(Entity* target) { mTarget = target; }

    AIState getAIState() const { return mAIState; }

    void setAIState(AIState state) { mAIState = state; }

    void setJumpingPower(float power) { mJumpingPower = power; }

    float getJumpingPower() const { return mJumpingPower; }

    bool isJumping() const { return mIsJumping; }

protected:
    AIState mAIState {AI_IDLE};
    Entity* mTarget {nullptr};
    bool mIsJumping {false};
    float mJumpingPower {7.0f};

    void AIJump(Map* map);
    void AIWander(Map* map);
    void AIFollow(Map* map);

private:
    virtual void AIActivate(float deltaTime, Map* map) = 0;
};

#endif // NPC_H