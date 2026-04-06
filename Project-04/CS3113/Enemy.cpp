#include "Enemy.h"

Enemy::Enemy(Vector2 position, Vector2 scale, const char* textureFilepath) :
    Entity(position, scale, textureFilepath) { }

Enemy::Enemy(Vector2 position, Vector2 scale, const char* textureFilepath,
             TextureType textureType, Vector2 spriteSheetDimensions,
             std::map<AnimationState, std::vector<int>> animationAtlas) :
    Entity(position, scale, textureFilepath, textureType, spriteSheetDimensions,
           animationAtlas) { }

void Enemy::AIWander(Map* map) {
    if (isOffEdge(map) || isCollidingLeft() || isCollidingRight())
        mDirection = (mDirection == LEFT) ? RIGHT : LEFT;
    if (mDirection == LEFT)
        moveLeft();
    else if (mDirection == RIGHT)
        moveRight();
}

void Enemy::AIJump(Map* map) {
    if (isCollidingBottom()) mIsJumping = true;

    if (mIsJumping) {
        mIsJumping = false;
        mVelocity.y -= mJumpingPower;
    }
}

void Enemy::AIFollow(Map* map) {
    if (mTarget == nullptr) return;

    Direction edgeDir = UP;
    bool offEdge = isOffEdge(map, &edgeDir);

    bool targetIsLeft = mPosition.x > mTarget->getPosition().x;

    if (targetIsLeft) {
        if (!(offEdge && edgeDir == LEFT)) moveLeft();
    } else {
        if (!(offEdge && edgeDir == RIGHT)) moveRight();
    }
}

void Enemy::update(float deltaTime, Map* map) {
    if (!isActive()) return;

    AIActivate(deltaTime, map);
    resetColliderFlags();
    mVelocity.x = mMovement.x * mSpeed;
    integrate(deltaTime);
    checkCollisions(map);

    if (mTextureType == ATLAS) {
        if (mAIState == AI_FOLLOW || mMovement.x != 0.0f)
            mAnimationState = WALK;
        else if (mAIState == AI_JUMP || !isCollidingBottom())
            mAnimationState = JUMP;
        else
            mAnimationState = IDLE;

        animate(deltaTime);
    }

    resetMovement();
}