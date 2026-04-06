#ifndef PLAYER_H
#define PLAYER_H

#include "Enemy.h"
#include "Entity.h"

class Player : public Entity {
public:
    static constexpr float INVINCIBILITY_DURATION = 1.0f;

    Player(Vector2 position, Vector2 scale, const char* textureFilepath);
    Player(Vector2 position, Vector2 scale, const char* textureFilepath,
           TextureType textureType, Vector2 spriteSheetDimensions,
           std::map<AnimationState, std::vector<int>> animationAtlas);

    void update(float deltaTime, std::vector<Enemy*>& enemies, Map* map);

    void jump() {
        if (isCollidingBottom()) mIsJumping = true;
    }

    void setJumpingPower(float power) { mJumpingPower = power; }

    float getJumpingPower() const { return mJumpingPower; }

    bool isInvincible() const { return mInvincibilityTimer > 0.0f; }

    bool wasHit() const { return mWasHit; }

    bool isJumping() const { return mIsJumping; }

    void clearHit() { mWasHit = false; }

    void hit() {
        mWasHit = true;
        mInvincibilityTimer = INVINCIBILITY_DURATION;
        mTint = RED;
    }

private:
    bool mIsJumping {false};
    float mJumpingPower {7.0f};
    float mInvincibilityTimer {0.0f};
    bool mWasHit {false};

    void checkEnemyCollisions(std::vector<Enemy*>& enemies);
};

#endif // PLAYER_H