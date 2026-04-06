#include "LevelB.h"

LevelB::LevelB() : Scene {{0.0f}, nullptr} { }

LevelB::LevelB(Vector2 origin, const char* bgHexCode) :
    Scene {origin, bgHexCode} { }

void LevelB::initialise() {
    mGameState.nextSceneID = -1;

    mGameState.jumpSound = LoadSound("assets/game/sfx_jump.ogg");
    mGameState.hitSound = LoadSound("assets/game/oof.mp3");
    mGameState.winSound = LoadSound("assets/game/win.mp3");

    /*
       ----------- MAP -----------
    */
    mGameState.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, (int*)mLevelData,
                             "assets/game/spritesheet-tiles-default.png",
                             TILE_DIMENSION, 18, 18, 1, mOrigin);

    mGameState.map->addNonSolidTile(123);
    mGameState.map->addNonSolidTile(124);
    setEndgameThreshold();

    /*
       ----------- PROTAGONIST -----------
    */
    std::map<AnimationState, std::vector<int>> charAtlas = {
        {IDLE, {12}},
        {WALK, {16, 17}},
        {JUMP, {15}},
    };

    float sizeRatio = 48.0f / 64.0f;

    mSpawnPosition = mGameState.map->getTileCenter(10, 1);
    mGameState.lilGuy =
        new Player(mSpawnPosition, {64, 64},
                   "assets/game/spritesheet-characters-default.png", ATLAS,
                   {7, 7}, charAtlas);

    mGameState.lilGuy->setJumpingPower(
        sqrtf(2.0f * ACCELERATION_OF_GRAVITY * TILE_DIMENSION * 3.0f) + 20.0f);
    mGameState.lilGuy->setColliderDimensions(mGameState.lilGuy->getScale()
                                             - Vector2 {14.0f, 14.0f});
    mGameState.lilGuy->setColliderOffset({0.0f, 7.0f});
    mGameState.lilGuy->setFrameRate(4.0f);
    mGameState.lilGuy->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    /*
       ----------- ENEMIES -----------
    */
    std::map<AnimationState, std::vector<int>> wandererAtlas = {
        {IDLE, {56}}, {WALK, {54, 55}}};

    std::map<AnimationState, std::vector<int>> followerAtlas = {
        {IDLE, {59}}, {WALK, {57, 58}}};

    Wanderer* wanderer1 =
        new Wanderer(mGameState.map->getTileCenter(12, 8), {64, 64},
                     "assets/game/spritesheet-enemies-default.png", ATLAS,
                     {8, 8}, wandererAtlas);
    wanderer1->setDirection(LEFT);
    wanderer1->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(wanderer1);

    Follower* follower =
        new Follower(mGameState.map->getTileCenter(5, 12), {64, 64},
                     "assets/game/spritesheet-enemies-default.png", ATLAS,
                     {8, 8}, followerAtlas);
    follower->setTarget(mGameState.lilGuy);
    follower->setDirection(LEFT);
    follower->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(follower);

    /*
       ----------- FLAGPOLE -----------
    */
    std::map<AnimationState, std::vector<int>> flagpoleAtlas = {
        {IDLE, {52}}, {WALK, {48, 49}}};

    mGameState.flagpole = new Flagpole(
        mGameState.map->getTileCenter(2, 1), {64, 64},
        "assets/game/spritesheet-tiles-default.png", {18, 18}, flagpoleAtlas);
    mGameState.flagpole->setFrameRate(4.0f);

    mGameState.initialised = true;
}

void LevelB::update(float deltaTime) {
    if (mGameState.winTimer > 0.0f) {
        mGameState.winTimer -= deltaTime;
        if (mGameState.winTimer <= 0.0f) {
            mGameState.nextSceneID = SCENE_C;
            return;
        }
    }

    mGameState.lilGuy->update(deltaTime, mGameState.enemies, mGameState.map);

    if (mGameState.lilGuy->getPosition().y > mEndgameThreshold) {
        mGameState.lilGuy->setPosition(mSpawnPosition);
        mGameState.lilGuy->setVelocity({0.0f, 0.0f});
        mGameState.lilGuy->hit();
    }

    for (Enemy* enemy : mGameState.enemies)
        enemy->update(deltaTime, mGameState.map);

    if (!mGameState.flagpole->isUp()
        && mGameState.lilGuy->isCollidingWith(mGameState.flagpole)) {
        mGameState.flagpole->activate();
        PlaySound(mGameState.winSound);
        mGameState.winTimer = 3.0f;
    }

    mGameState.flagpole->update(deltaTime);
}

void LevelB::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.flagpole->render();
    mGameState.lilGuy->render();

    for (Entity* enemy : mGameState.enemies)
        enemy->render();
}

void LevelB::shutdown() {
    Scene::shutdown();
}