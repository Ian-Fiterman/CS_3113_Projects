#include "Entity.h"

Entity::Entity() :
    mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, mVelocity {0.0f, 0.0f},
    mAcceleration {0.0f, 0.0f}, mScale {DEFAULT_SIZE, DEFAULT_SIZE},
    mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, mTexture {},
    mTextureType {SINGLE}, mAngle {0.0f}, mSpriteSheetDimensions {},
    mDirection {DOWN}, mAnimationAtlas {{}}, mAnimationIndices {},
    mFrameSpeed {0} { }

Entity::Entity(Vector2 position, Vector2 scale, const char* textureFilepath) :
    mPosition {position}, mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
    mScale {scale}, mMovement {0.0f, 0.0f}, mColliderDimensions {scale},
    mTexture {LoadTexture(textureFilepath)}, mTextureType {SINGLE},
    mDirection {DOWN}, mAnimationAtlas {{}}, mAnimationIndices {},
    mFrameSpeed {0}, mSpeed {DEFAULT_SPEED}, mAngle {0.0f} { }

Entity::Entity(Vector2 position, Vector2 scale, const char* textureFilepath,
               TextureType textureType, Vector2 spriteSheetDimensions,
               std::map<Direction, std::vector<int>> animationAtlas) :
    mPosition {position}, mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
    mMovement {0.0f, 0.0f}, mScale {scale}, mColliderDimensions {scale},
    mTexture {LoadTexture(textureFilepath)}, mTextureType {ATLAS},
    mSpriteSheetDimensions {spriteSheetDimensions},
    mAnimationAtlas {animationAtlas}, mDirection {DOWN},
    mAnimationIndices {animationAtlas.at(DOWN)},
    mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle {0.0f}, mSpeed {DEFAULT_SPEED} { }

Entity::~Entity() {
    UnloadTexture(mTexture);
};

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
 * @brief Helper function to get the minimum overlap between this entity and map
 * tiles
 * @param map The map to check for collisions with
 * @param probes The points to probe for collisions
 * @param minimizeX Whether to minimize the x or y overlap value when resolving
 * collisions
 * @return The minimum overlap value in the specified direction, or {0, 0} if no
 * collision
 */
Vector2 Entity::getMinOverlap(Map* map, std::vector<Vector2> probes,
                              bool minimizeX) const {
    float xOverlap = 0.0f, yOverlap = 0.0f;
    float bestX = INFINITY, bestY = INFINITY;
    bool hit = false;

    for (const Vector2& probe : probes) {
        if (map->isSolidTileAt(probe, &xOverlap, &yOverlap)) {
            if (minimizeX ? xOverlap < bestX : yOverlap < bestY) {
                bestX = xOverlap;
                bestY = yOverlap;
            }
            hit = true;
        }
    }
    return hit ? Vector2 {bestX, bestY} : Vector2 {0.0f, 0.0f};
}

/**
 * @brief Checks if this entity is colliding with another entity
 * @param other The other entity to check for collision with
 * @return True if the entities are colliding, false otherwise
 */
bool Entity::isCollidingWith(Entity* other) const {
    Vector2 overlap = getOverlap(other);
    return overlap.x > 0.0f && overlap.y > 0.0f;
}

/**
 * @brief Checks for and resolves collisions with another entity along the Y
 * axis
 * @param other
 */
void Entity::checkCollisionY(Entity* other) {
    Vector2 overlap = getOverlap(other);
    if (overlap.y == 0.0f) return;

    if (mVelocity.y > 0 && overlap.y < overlap.x) {
        mPosition.y -= overlap.y;
        mVelocity.y = 0;
        mIsCollidingBottom = true;
    } else if (mVelocity.y < 0 && overlap.y < overlap.x) {
        mPosition.y += overlap.y;
        mVelocity.y = 0;
        mIsCollidingTop = true;
    }
}

/**
 * @brief Checks for and resolves collisions with another entity along the X
 * axis
 * @param other
 */
void Entity::checkCollisionX(Entity* other) {
    Vector2 overlap = getOverlap(other);
    if (overlap.x == 0.0f) return;
    if (overlap.y < Y_COLLISION_THRESHOLD) return;

    if (mVelocity.x > 0 && overlap.x < overlap.y) {
        mPosition.x -= overlap.x;
        mVelocity.x = 0;

        // Collision!
        mIsCollidingRight = true;
    } else if (mVelocity.x < 0 && overlap.x < overlap.y) {
        mPosition.x += overlap.x;
        mVelocity.x = 0;

        // Collision!
        mIsCollidingLeft = true;
    }
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

    Vector2 overlap;

    // COLLISION ABOVE (jumping upward)
    overlap = getMinOverlap(map,
                            {{centerX - halfWidth, centerY - halfHeight},
                             {centerX, centerY - halfHeight},
                             {centerX + halfWidth, centerY - halfHeight}},
                            false);
    if (overlap.y > 0.0f && mVelocity.y < 0.0f && overlap.y < overlap.x) {
        mPosition.y += overlap.y; // push down
        mVelocity.y = 0.0f;
        mIsCollidingTop = true;
    }

    // COLLISION BELOW (falling downward)
    overlap = getMinOverlap(map,
                            {{centerX - halfWidth, centerY + halfHeight},
                             {centerX, centerY + halfHeight},
                             {centerX + halfWidth, centerY + halfHeight}},
                            false);
    if (overlap.y > 0.0f && mVelocity.y > 0.0f && overlap.y < overlap.x) {
        mPosition.y -= overlap.y; // push up
        mVelocity.y = 0.0f;
        mIsCollidingBottom = true;
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

    Vector2 overlap;

    // COLLISION ON RIGHT (moving right)
    overlap = getMinOverlap(map,
                            {{centerX + halfWidth, centerY - halfHeight},
                             {centerX + halfWidth, centerY},
                             {centerX + halfWidth, centerY + halfHeight}},
                            true);
    if (overlap.x > 0.0f && mVelocity.x > 0.0f && overlap.x < overlap.y) {
        mPosition.x -= overlap.x * 1.01f; // push left
        mVelocity.x = 0.0f;
        mIsCollidingRight = true;
    }

    // COLLISION ON LEFT (moving left)
    overlap = getMinOverlap(map,
                            {{centerX - halfWidth, centerY - halfHeight},
                             {centerX - halfWidth, centerY},
                             {centerX - halfWidth, centerY + halfHeight}},
                            true);
    if (overlap.x > 0.0f && mVelocity.x < 0.0f && overlap.x < overlap.y) {
        mPosition.x += overlap.x * 1.01f; // push right
        mVelocity.x = 0.0f;
        mIsCollidingLeft = true;
    }
}

/**
 * @brief Checks for and resolves collisions with other entities along both axes
 * @param collidableEntities
 * @param collisionCheckCount 
 */
void Entity::checkCollisions(Entity* collidableEntities,
                             int collisionCheckCount) {
    for (int i = 0; i < collisionCheckCount; i++) {
        checkCollisionY(&collidableEntities[i]);
    }
    for (int i = 0; i < collisionCheckCount; i++) {
        checkCollisionX(&collidableEntities[i]);
    }
}

/**
 * @brief Checks for and resolves collisions with map tiles along both axes
 * @param map
 */
void Entity::checkCollisions(Map* map) {
    checkCollisionY(map);
    checkCollisionX(map);
}

void Entity::animate(float deltaTime) {
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond) {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::update(float deltaTime, Entity* collidableEntities,
                    int collisionCheckCount, Map* map) {
    if (mEntityStatus == INACTIVE) return;

    resetColliderFlags();

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;
    mPosition.x += mVelocity.x * deltaTime;
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

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {mPosition.x, mPosition.y,
                                 static_cast<float>(mScale.x),
                                 static_cast<float>(mScale.y)};

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {static_cast<float>(mScale.x) / 2.0f,
                            static_cast<float>(mScale.y) / 2.0f};

    // Render the texture on screen
    DrawTexturePro(mTexture, textureArea, destinationArea, originOffset, mAngle,
                   WHITE);

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