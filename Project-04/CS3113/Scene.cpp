#include "Scene.h"

Scene::Scene() : mOrigin {{}} { }

Scene::Scene(Vector2 origin, const char* bgHexCode) :
    mOrigin {origin}, mBGColourHexCode {bgHexCode} {
    ClearBackground(ColorFromHex(bgHexCode));
}

void Scene::shutdown() {
    delete mGameState.lilGuy;
    mGameState.lilGuy = nullptr;

    delete mGameState.map;
    mGameState.map = nullptr;

    delete mGameState.flagpole;
    mGameState.flagpole = nullptr;

    for (Enemy* enemy : mGameState.enemies)
        delete enemy;
    mGameState.enemies.clear();

    if (mGameState.initialised) {
        UnloadSound(mGameState.jumpSound);
        UnloadSound(mGameState.hitSound);
        UnloadSound(mGameState.winSound);
        mGameState.initialised = false;
    }
}