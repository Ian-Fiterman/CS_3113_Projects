#ifndef FLAGPOLE_H
#define FLAGPOLE_H

#include "Entity.h"

enum FlagpoleState { FLAG_DOWN, FLAG_UP };

class Flagpole : public Entity {
public:
    Flagpole(Vector2 position, Vector2 scale, const char* texturePath,
             Vector2 sheetDimensions,
             std::map<AnimationState, std::vector<int>> animationAtlas);

    void activate() { mState = FLAG_UP; mAnimationState = WALK; }
    bool isUp()     const { return mState == FLAG_UP; }

    void update(float deltaTime) { if (mState == FLAG_UP) animate(deltaTime); }

private:
    FlagpoleState mState { FLAG_DOWN };
};

#endif // FLAGPOLE_H