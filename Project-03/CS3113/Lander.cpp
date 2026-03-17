#include "Lander.h"

void Lander::update(float deltaTime, Entity* collidableEntities,
                    int collisionCheckCount, Map* map) {
    // Handle rotation input
    if (mRotateLeft) mAngle -= ROTATION_SPEED * deltaTime;
    if (mRotateRight) mAngle += ROTATION_SPEED * deltaTime;
    mAngle = Clamp(mAngle, -90.0f, 90.0f);
    // Only apply thrust if the lander is currently thrusting and has fuel
    if (mIsThrusting && mFuel > 0.0f) {
        float rad = (mAngle - 90.0f) * DEG2RAD;
        mAcceleration.x += cosf(rad) * THRUST_POWER;
        mAcceleration.y += sinf(rad) * THRUST_POWER;
        mFuel = fmaxf(0.0f, mFuel - FUEL_BURN_RATE * deltaTime);
    }
    Vector2 impactVelocity = mVelocity;
    // Integrate acceleration and velocity, and move the lander
    Entity::update(deltaTime, collidableEntities, collisionCheckCount, map);
    // Reset to just gravity each frame
    mAcceleration.x = 0.0f;
    mAcceleration.y = GRAVITY;
    // Check collision
    checkCollisions(map);
    checkCollisions(collidableEntities, collisionCheckCount);

    if (isColliding() && !mHasLanded && !mHasCrashed) {
        float landingAngle = fabs(mAngle);
        if (fabs(impactVelocity.x) <= SAFE_LANDING_SPEED
            && fabs(impactVelocity.y) <= SAFE_LANDING_SPEED
            && landingAngle <= SAFE_LANDING_ANGLE) {
            mHasLanded = true;
        } else {
            mHasCrashed = true;
        }
        mVelocity = impactVelocity;
    }

    // Update animation based on whether the lander is thrusting or not
    mDirection = mIsThrusting ? UP : DOWN;
    mAnimationIndices = mAnimationAtlas.at(mDirection);
}

void Lander::resetControls() {
    mIsThrusting = false;
    mRotateLeft = false;
    mRotateRight = false;
}