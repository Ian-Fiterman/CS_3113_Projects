#include "Spaceship.h"

/*
This math took me forever to figure out but tl;dr it moves the spaceship along a
sinusoidal path with constant speed along the curve. It works by computing the
slope of the sine wave at the current position. Then it uses that slope to apply
an arc-length correction to ensure uniform speed. It advances the spaceship
along the curve based on the base speed, direction, and delta time. If the ship
reaches the end of its range, it reverses direction. Finally, it converts the
mDistance value to the actual x and y coordinates of the spaceship.
*/
void Spaceship::update(float deltaTime, Entity* collidableEntities,
                       int collisionCheckCount, Map* map) {
    // Compute slope
    const float cosTerm = cosf(k * mDistance);
    const float slope = mAmplitude * k * cosTerm;
    // Arc-length correction
    const float pathScale = 1.0f / sqrtf(1.0f + slope * slope);
    // Advance along curve
    mDistance += mDirection * mBaseSpeed * pathScale * deltaTime;
    // Clamp and reverse
    if (mDistance > mHalfRange) {
        mDistance = mHalfRange;
        mDirection = -1.0f;
    } else if (mDistance < -mHalfRange) {
        mDistance = -mHalfRange;
        mDirection = 1.0f;
    }
    // Update position
    updatePositionFromDistance();
}

/**
 @brief Updates the spaceship's position based on its current distance along the
 path
*/
void Spaceship::updatePositionFromDistance() {
    const float sinTerm = sinf(k * mDistance);
    mPosition.x = mCenterX + mDistance;
    mPosition.y = mCenterY + mAmplitude * sinTerm;
}