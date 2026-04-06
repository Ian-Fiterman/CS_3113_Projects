#include "Flagpole.h"

Flagpole::Flagpole(Vector2 position, Vector2 scale, const char* texturePath,
                   Vector2 sheetDimensions,
                   std::map<AnimationState, std::vector<int>> animationAtlas) :
    Entity(position, scale, texturePath, ATLAS, sheetDimensions, animationAtlas)
{
    mAnimationState = IDLE;
}