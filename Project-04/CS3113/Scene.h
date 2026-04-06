#ifndef SCENE_H
#define SCENE_H

#include "Flagpole.h"
#include "Follower.h"
#include "Jumper.h"
#include "Player.h"
#include "Wanderer.h"

constexpr int LEVEL_WIDTH = 24, LEVEL_HEIGHT = 16;

constexpr float TILE_DIMENSION = 64.0f, ACCELERATION_OF_GRAVITY = 981.0f;

struct GameState {
    Player* lilGuy = nullptr;
    Map* map = nullptr;
    std::vector<Enemy*> enemies = {};
    Flagpole* flagpole = nullptr;
    float winTimer = -1.0f;
    bool initialised = false;

    Sound jumpSound;
    Sound hitSound;
    Sound winSound;

    int nextSceneID = -1;
};

class Scene {
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char* mBGColourHexCode = "#000000";
    float mEndgameThreshold = 0.0f;

public:
    Scene();
    Scene(Vector2 origin, const char* bgHexCode);

    virtual ~Scene() { shutdown(); }

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    virtual void shutdown();

    GameState& getState() { return mGameState; }

    Vector2 getOrigin() const { return mOrigin; }

    const char* getBGColourHexCode() const { return mBGColourHexCode; }

    void setEndgameThreshold() {
        mEndgameThreshold = mGameState.map->getBottomBoundary();
    }

    float getEndgameThreshold() const { return mEndgameThreshold; }
};

#endif // SCENE_H