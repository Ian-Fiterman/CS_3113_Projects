#ifndef LANDER_H
#define LANDER_H

#include "Entity.h"

class Lander : public Entity {
private:
    static constexpr float MAX_FUEL = 100.0f;
    static constexpr float ROTATION_SPEED = 60.0f;     // deg/sec
    static constexpr float GRAVITY = 10.0f;            // pixels/sec^2
    static constexpr float THRUST_POWER = GRAVITY * 3; // pixels/sec^2
    static constexpr float FUEL_BURN_RATE = 5.0f;     // fuel/sec
    static constexpr float SAFE_LANDING_SPEED = 10.0f; // pixels/sec
    static constexpr float SAFE_LANDING_ANGLE = 10.0f; // degrees

    float mFuel = MAX_FUEL;
    bool mRotateLeft = false;
    bool mRotateRight = false;
    bool mIsThrusting = false;
    bool mHasLanded = false;
    bool mHasCrashed = false;

public:
    using Entity::Entity;

    void update(float deltaTime, Entity* collidableEntities,
                int collisionCheckCount, Map* map) override;

    void rotateLeft() { mRotateLeft = true; }

    bool isRotatingLeft() const { return mRotateLeft; }

    void rotateRight() { mRotateRight = true; }

    bool isRotatingRight() const { return mRotateRight; }

    void thrust() { mIsThrusting = true; }

    bool isThrusting() const { return mIsThrusting; }

    float getFuel() const { return mFuel; }

    bool hasLanded() const { return mHasLanded; }

    bool hasCrashed() const { return mHasCrashed; }

    void resetControls();

    void resetStates() {
        mHasLanded = false;
        mHasCrashed = false;
    }

    void refuel() { mFuel = MAX_FUEL; }
};
#endif // LANDER_H