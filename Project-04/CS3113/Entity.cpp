#include "Entity.h"

Entity::Entity() :
    mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, mVelocity {0.0f, 0.0f},
    mAcceleration {0.0f, 0.0f}, mScale {DEFAULT_SIZE, DEFAULT_SIZE},
    mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, mTexture {},
    mTextureType {SINGLE}, mAngle {0.0f}, mSpriteSheetDimensions {},
    mDirection {DOWN}, mAnimationAtlas {{}}, mAnimationIndices {},
    mFrameRate {0.0f} { }

Entity::Entity(Vector2 position, Vector2 scale, const char* textureFilepath) :
    mPosition {position}, mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
    mScale {scale}, mMovement {0.0f, 0.0f}, mColliderDimensions {scale},
    mTexture {LoadTexture(textureFilepath)}, mTextureType {SINGLE},
    mDirection {DOWN}, mAnimationAtlas {{}}, mAnimationIndices {},
    mFrameRate {0.0f}, mSpeed {DEFAULT_SPEED}, mAngle {0.0f} { }

Entity::Entity(Vector2 position, Vector2 scale, const char* textureFilepath,
               TextureType textureType, Vector2 spriteSheetDimensions,
               std::map<AnimationState, std::vector<int>> animationAtlas) :
    mPosition {position}, mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
    mMovement {0.0f, 0.0f}, mScale {scale}, mColliderDimensions {scale},
    mTexture {LoadTexture(textureFilepath)}, mTextureType {ATLAS},
    mSpriteSheetDimensions {spriteSheetDimensions},
    mAnimationAtlas {animationAtlas}, mDirection {DOWN},
    mAnimationIndices {animationAtlas.at(IDLE)},
    mFrameRate {DEFAULT_FRAME_SPEED}, mAngle {0.0f}, mSpeed {DEFAULT_SPEED} { }

Entity::~Entity() {
    UnloadTexture(mTexture);
};

void Entity::integrate(float deltaTime) {
    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;
    mPosition.x += mVelocity.x * deltaTime;
}

void Entity::animate(float deltaTime) {
    mAnimationIndices = mAnimationAtlas.at(mAnimationState);

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameRate;

    if (mAnimationTime >= framesPerSecond) {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::resetColliderFlags() {
    mCollision.reset();
}

/**
 * @brief Computes the overlap between this entity and another entity
 * @param other The other entity to check for overlap with
 * @return A Vector2 containing the x and y overlap value
 */
Vector2 Entity::getOverlap(Entity* other) const {
    if (!other->isActive() || other == this) return {0.0f, 0.0f};

    float xDistance = fabs((mPosition.x + mColliderOffset.x)
                           - (other->mPosition.x + other->mColliderOffset.x));
    float yDistance = fabs((mPosition.y + mColliderOffset.y)
                           - (other->mPosition.y + other->mColliderOffset.y));

    float xOverlap = (mColliderDimensions.x / 2.0f)
                   + (other->mColliderDimensions.x / 2.0f) - xDistance;
    float yOverlap = (mColliderDimensions.y / 2.0f)
                   + (other->mColliderDimensions.y / 2.0f) - yDistance;

    if (xOverlap <= 0.0f || yOverlap <= 0.0f) return {0.0f, 0.0f};

    return {xOverlap, yOverlap};
}

/**
 * @brief Checks if this entity is colliding with another entity, caches the
 * overlap for use in resolveCollisionX and resolveCollisionY
 * @param other The other entity to check for collision with
 * @return True if the entities are colliding, false otherwise
 */
bool Entity::isCollidingWith(Entity* other) {
    mCollision.overlap = getOverlap(other);

    if (mCollision.overlap.x <= 0.0f || mCollision.overlap.y <= 0.0f)
        return false;

    if (mCollision.overlap.y < mCollision.overlap.x) {
        if (mVelocity.y > 0)
            mCollision.bottom = true;
        else if (mVelocity.y < 0)
            mCollision.top = true;
    } else {
        if (mVelocity.x > 0)
            mCollision.right = true;
        else if (mVelocity.x < 0)
            mCollision.left = true;
    }

    return true;
}

/**
 * @brief Checks for and resolves collisions with map tiles along the Y axis
 * @param map
 */
void Entity::checkCollisionY(Map* map) {
    if (map == nullptr) return;

    float centerX = mPosition.x + mColliderOffset.x;
    float centerY = mPosition.y + mColliderOffset.y;
    float halfWidth = mColliderDimensions.x / 2.0f;
    float halfHeight = mColliderDimensions.y / 2.0f;

    Vector2 topLeftProbe = {centerX - halfWidth, centerY - halfHeight};
    Vector2 topCentreProbe = {centerX, centerY - halfHeight};
    Vector2 topRightProbe = {centerX + halfWidth, centerY - halfHeight};

    Vector2 bottomLeftProbe = {centerX - halfWidth, centerY + halfHeight};
    Vector2 bottomCentreProbe = {centerX, centerY + halfHeight};
    Vector2 bottomRightProbe = {centerX + halfWidth, centerY + halfHeight};

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ABOVE (jumping upward)
    if ((map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap)
         || map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap)
         || map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap))
        && mVelocity.y < 0.0f) {
        mPosition.y += yOverlap; // push down
        mVelocity.y = 0.0f;
        mCollision.top = true;
    }

    // COLLISION BELOW (falling downward)
    if ((map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap)
         || map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap)
         || map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap))
        && mVelocity.y > 0.0f) {
        mPosition.y -= yOverlap; // push up
        mVelocity.y = 0.0f;
        mCollision.bottom = true;
    }
}

/**
 * @brief Checks for and resolves collisions with map tiles along the X axis
 * @param map
 */
void Entity::checkCollisionX(Map* map) {
    if (map == nullptr) return;

    float centerX = mPosition.x + mColliderOffset.x;
    float centerY = mPosition.y + mColliderOffset.y;
    float halfWidth = mColliderDimensions.x / 2.0f;
    float halfHeight = mColliderDimensions.y / 2.0f;

    Vector2 rightTopProbe = {centerX + halfWidth, centerY - halfHeight};
    Vector2 rightCentreProbe = {centerX + halfWidth, centerY};
    Vector2 rightBottomProbe = {centerX + halfWidth, centerY + halfHeight};

    Vector2 leftTopProbe = {centerX - halfWidth, centerY - halfHeight};
    Vector2 leftCentreProbe = {centerX - halfWidth, centerY};
    Vector2 leftBottomProbe = {centerX - halfWidth, centerY + halfHeight};

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ON RIGHT (moving right)
    if ((map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap)
         || map->isSolidTileAt(rightTopProbe, &xOverlap, &yOverlap)
         || map->isSolidTileAt(rightBottomProbe, &xOverlap, &yOverlap))
        && mVelocity.x > 0.0f && yOverlap >= Y_COLLISION_THRESHOLD) {
        mPosition.x -= xOverlap * 1.01f; // push left
        mVelocity.x = 0.0f;
        mCollision.right = true;
    }

    // COLLISION ON LEFT (moving left)
    if ((map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap)
         || map->isSolidTileAt(leftTopProbe, &xOverlap, &yOverlap)
         || map->isSolidTileAt(leftBottomProbe, &xOverlap, &yOverlap))
        && mVelocity.x < 0.0f && yOverlap >= Y_COLLISION_THRESHOLD) {
        mPosition.x += xOverlap * 1.01f; // push right
        mVelocity.x = 0.0f;
        mCollision.left = true;
    }
}

/**
 * @brief Checks for and resolves collisions with map tiles along both axes
 * @param map
 */
void Entity::checkCollisions(Map* map) {
    checkCollisionX(map);
    checkCollisionY(map);
}

bool Entity::isOffEdge(Map* map, Direction* direction) const {
    if (map == nullptr) return false;

    float centerX = mPosition.x + mColliderOffset.x;
    float centerY = mPosition.y + mColliderOffset.y;
    float halfWidth = mColliderDimensions.x / 2.0f;
    float halfHeight = mColliderDimensions.y / 2.0f;

    Vector2 bottomLeftProbe = {centerX - halfWidth, centerY + halfHeight};
    Vector2 bottomRightProbe = {centerX + halfWidth, centerY + halfHeight};

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    bool leftGrounded =
        map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap);
    bool rightGrounded =
        map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap);

    if (mCollision.bottom) {
        if (!leftGrounded && rightGrounded) {
            if (direction) *direction = LEFT;
            return true;
        } else if (leftGrounded && !rightGrounded) {
            if (direction) *direction = RIGHT;
            return true;
        }
    }
    return false;
}

void Entity::render() {
    if (mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType) {
    case SINGLE :
        // Whole texture (UV coordinates)
        textureArea = {// top-left corner
                       0.0f, 0.0f,

                       // bottom-right corner (of texture)
                       static_cast<float>(mTexture.width),
                       static_cast<float>(mTexture.height)};
        break;
    case ATLAS :
        textureArea =
            getUVRectangle(&mTexture, mAnimationIndices[mCurrentFrameIndex],
                           mSpriteSheetDimensions.x, mSpriteSheetDimensions.y);

    default :
        break;
    }

    if (mDirection == mFlipDirection) textureArea.width *= -1.0f;

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {mPosition.x, mPosition.y,
                                 static_cast<float>(mScale.x),
                                 static_cast<float>(mScale.y)};

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {static_cast<float>(mScale.x) / 2.0f,
                            static_cast<float>(mScale.y) / 2.0f};

    // Render the texture on screen
    DrawTexturePro(mTexture, textureArea, destinationArea, originOffset, mAngle,
                   mTint);

    // displayCollider();
}

void Entity::displayCollider() {
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x + mColliderOffset.x - mColliderDimensions.x / 2.0f,
        mPosition.y + mColliderOffset.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x, mColliderDimensions.y};

    DrawRectangleLines(colliderBox.x,      // Top-left X
                       colliderBox.y,      // Top-left Y
                       colliderBox.width,  // Width
                       colliderBox.height, // Height
                       GREEN               // Color
    );
}