#include "Player.h"

Player::Player(Vector2 position, Vector2 scale, const char* textureFilepath) :
    Entity(position, scale, textureFilepath) { }

Player::Player(Vector2 position, Vector2 scale, const char* textureFilepath,
               TextureType textureType, Vector2 spriteSheetDimensions,
               std::map<AnimationState, std::vector<int>> animationAtlas) :
    Entity(position, scale, textureFilepath, textureType, spriteSheetDimensions,
           animationAtlas) { }

void Player::checkEnemyCollisions(std::vector<Enemy*>& enemies) {
    bool wasOnGround = isCollidingBottom();

    for (Entity* enemy : enemies) {
        resetColliderFlags();

        if (isCollidingWith(enemy)) {

            if (mInvincibilityTimer <= 0.0f) { hit(); }
        }
    }

    mCollision.bottom = isCollidingBottom() || wasOnGround;
}

void Player::update(float deltaTime, std::vector<Enemy*>& enemies, Map* map) {
    if (!isActive()) return;

    resetColliderFlags();

    if (mInvincibilityTimer > 0.0f) {
        mInvincibilityTimer -= deltaTime;

        if (mInvincibilityTimer <= 0.0f) {
            mInvincibilityTimer = 0.0f;
            mTint = WHITE;
        }
    }

    mVelocity.x = mMovement.x * mSpeed;

    // ––––– JUMPING ––––– //
    if (mIsJumping) {
        // STEP 1: Immediately return the flag to its original false state
        mIsJumping = false;

        // STEP 2: The player now acquires an upward velocity
        mVelocity.y -= mJumpingPower;
    }

    integrate(deltaTime);

    checkCollisions(map);
    checkEnemyCollisions(enemies);

    if (mTextureType == ATLAS) {
        if (!isCollidingBottom())
            mAnimationState = JUMP;
        else if (mMovement.x != 0.0f)
            mAnimationState = WALK;
        else
            mAnimationState = IDLE;

        animate(deltaTime);
    }
    resetMovement();
}