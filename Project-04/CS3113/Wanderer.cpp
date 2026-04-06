#include "Wanderer.h"

Wanderer::Wanderer(Vector2 position, Vector2 scale,
                   const char* textureFilepath) :
    Enemy(position, scale, textureFilepath) { }

Wanderer::Wanderer(Vector2 position, Vector2 scale, const char* textureFilepath,
                   TextureType textureType, Vector2 spriteSheetDimensions,
                   std::map<AnimationState, std::vector<int>> animationAtlas) :
    Enemy(position, scale, textureFilepath, textureType, spriteSheetDimensions,
          animationAtlas) {
    this->setSpeed(100.0f);
    this->setFrameRate(4.0f);
    this->setFlipDirection(RIGHT);
    this->setAIState(AI_WALK);
    this->setColliderDimensions(this->getScale() - Vector2 {0, 34.0f});
    this->setColliderOffset({0.0f, 17.0f});
}

void Wanderer::AIActivate(float deltaTime, Map* map) {
    AIWander(map);
}