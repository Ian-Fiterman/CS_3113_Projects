#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "Entity.h"

class Spaceship : public Entity {
private:
    float mCenterX = 0.0f;
    float mCenterY = 0.0f;
    float mHalfRange = 0.0f;
    float mBaseSpeed = 0.0f;
    float mDistance = 0.0f;
    float mDirection = 1.0f;
    float mAmplitude = 0.0f;
    float mWavelength = 0.0f;

    float k = 2.0f * PI / mWavelength;

public:
    Spaceship(Vector2 position, Vector2 scale, const char* textureFilepath) :
        Entity(position, scale, textureFilepath), // Call base constructor
        mCenterX {position.x}, mCenterY {position.y} { }

    void update(float deltaTime, Entity* collidableEntities,
                int collisionCheckCount, Map* map) override;

    void updatePositionFromDistance();

    void setHalfRange(float halfRange) { mHalfRange = halfRange; }

    void setBaseSpeed(float baseSpeed) { mBaseSpeed = baseSpeed; }

    void setDirection(float direction) { mDirection = direction; }

    void setDistance(float distance) { mDistance = distance; }

    void setAmplitude(float amplitude) { mAmplitude = amplitude; }

    void setWavelength(float wavelength) {
        mWavelength = wavelength;
        k = 2.0f * PI / mWavelength;
    }

    float getHalfRange() const { return mHalfRange; }

    float getBaseSpeed() const { return mBaseSpeed; }

    float getDirection() const { return mDirection; }

    float getDistance() const { return mDistance; }

    float getAmplitude() const { return mAmplitude; }

    float getWavelength() const { return mWavelength; }
};

#endif // SPACESHIP_H