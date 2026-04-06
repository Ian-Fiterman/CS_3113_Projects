#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"

enum Direction { LEFT, RIGHT, UP, DOWN }; // For walking

enum EntityStatus { ACTIVE, INACTIVE };

enum AnimationState { WALK, IDLE, JUMP };

struct CollisionState {
    bool top {false};
    bool bottom {false};
    bool left {false};
    bool right {false};
    Vector2 overlap {0.0f, 0.0f};

    void reset() {
        top = false;
        bottom = false;
        left = false;
        right = false;
        overlap = {0.0f, 0.0f};
    }

    bool any() const { return top || bottom || left || right; }
};

class Entity {
public:
    static constexpr float DEFAULT_SIZE = 250.0f;
    static constexpr float DEFAULT_SPEED = 200.0f;
    static constexpr float DEFAULT_FRAME_SPEED = 15.0f;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(Vector2 position, Vector2 scale, const char* textureFilepath);
    Entity(Vector2 position, Vector2 scale, const char* textureFilepath,
           TextureType textureType, Vector2 spriteSheetDimensions,
           std::map<AnimationState, std::vector<int>> animationAtlas);
    virtual ~Entity();

    virtual void render();
    void displayCollider();

    bool isCollidingWith(Entity* other);
    Vector2 getOverlap(Entity* other) const;

    void activate() { mEntityStatus = ACTIVE; }

    void deactivate() { mEntityStatus = INACTIVE; }

    bool isActive() const { return mEntityStatus == ACTIVE; }

    void moveLeft() {
        mMovement.x = -1.0f;
        mDirection = LEFT;
    }

    void moveRight() {
        mMovement.x = 1.0f;
        mDirection = RIGHT;
    }

    void resetMovement() { mMovement = {0.0f, 0.0f}; }

    void normaliseMovement() { Normalise(&mMovement); }

    Vector2 getPosition() const { return mPosition; }

    Vector2 getVelocity() const { return mVelocity; }

    Vector2 getMovement() const { return mMovement; }

    Vector2 getScale() const { return mScale; }

    Vector2 getColliderDimensions() const { return mColliderDimensions; }

    Vector2 getColliderOffset() const { return mColliderOffset; }

    Direction getDirection() const { return mDirection; }

    Color getTint() const { return mTint; }

    bool isCollidingTop() const { return mCollision.top; }

    bool isCollidingBottom() const { return mCollision.bottom; }

    bool isCollidingLeft() const { return mCollision.left; }

    bool isCollidingRight() const { return mCollision.right; }

    bool isColliding() const { return mCollision.any(); }

    bool isOffEdge(Map* map, Direction* direction = nullptr) const;

    void setPosition(Vector2 position) { mPosition = position; }

    void setVelocity(Vector2 velocity) { mVelocity = velocity; }

    void setAcceleration(Vector2 acceleration) { mAcceleration = acceleration; }

    void setMovement(Vector2 movement) { mMovement = movement; }

    void setSpeed(float speed) { mSpeed = speed; }

    void setAngle(float angle) { mAngle = angle; }

    void setScale(Vector2 scale) { mScale = scale; }

    void setColliderDimensions(Vector2 dims) { mColliderDimensions = dims; }

    void setColliderOffset(Vector2 offset) { mColliderOffset = offset; }

    void setFlipDirection(Direction direction) { mFlipDirection = direction; }

    void setFrameRate(float rate) { mFrameRate = rate; }

    void setDirection(Direction direction) { mDirection = direction; }

    void setTint(Color tint) { mTint = tint; }

protected:
    Vector2 mPosition {};
    Vector2 mVelocity {};
    Vector2 mAcceleration {};
    Vector2 mMovement {};
    float mSpeed {DEFAULT_SPEED};
    float mAngle {0.0f};

    Texture2D mTexture {};
    TextureType mTextureType {SINGLE};
    Vector2 mScale {DEFAULT_SIZE, DEFAULT_SIZE};
    Vector2 mSpriteSheetDimensions {};

    std::map<AnimationState, std::vector<int>> mAnimationAtlas {};
    std::vector<int> mAnimationIndices {};
    int mCurrentFrameIndex {0};
    float mAnimationTime {0.0f};
    float mFrameRate {0.0f};
    Direction mDirection {DOWN};
    Direction mFlipDirection {LEFT};
    AnimationState mAnimationState {IDLE};

    Color mTint {WHITE};

    CollisionState mCollision {};

    Vector2 mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE};
    Vector2 mColliderOffset {0.0f, 0.0f};

    EntityStatus mEntityStatus {ACTIVE};

    void checkCollisionY(Map* map);
    void checkCollisionX(Map* map);
    void checkCollisions(Map* map);

    void integrate(float deltaTime);
    void animate(float deltaTime);
    void resetColliderFlags();
};

#endif // ENTITY_H