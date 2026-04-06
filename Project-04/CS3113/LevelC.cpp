#include "LevelC.h"

LevelC::LevelC() : Scene {{0.0f}, nullptr} { }

LevelC::LevelC(Vector2 origin, const char* bgHexCode) :
    Scene {origin, bgHexCode} { }

void LevelC::initialise() {
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

    mSpawnPosition = mGameState.map->getTileCenter(13, 11);
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

    std::map<AnimationState, std::vector<int>> jumperAtlas = {{IDLE, {21}},
                                                              {JUMP, {22}}};

    float jumperJumpingPower =
        sqrtf(2.0f * ACCELERATION_OF_GRAVITY * TILE_DIMENSION * 4.0f);

    Wanderer* wanderer1 =
        new Wanderer(mGameState.map->getTileCenter(5, 4), {64, 64},
                     "assets/game/spritesheet-enemies-default.png", ATLAS,
                     {8, 8}, wandererAtlas);
    wanderer1->setDirection(RIGHT);
    wanderer1->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(wanderer1);

    Wanderer* wanderer2 =
        new Wanderer(mGameState.map->getTileCenter(11, 7), {64, 64},
                     "assets/game/spritesheet-enemies-default.png", ATLAS,
                     {8, 8}, wandererAtlas);
    wanderer2->setDirection(RIGHT);
    wanderer2->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(wanderer2);

    Follower* follower =
        new Follower(mGameState.map->getTileCenter(9, 21), {64, 64},
                     "assets/game/spritesheet-enemies-default.png", ATLAS,
                     {8, 8}, followerAtlas);
    follower->setTarget(mGameState.lilGuy);
    follower->setDirection(LEFT);
    follower->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(follower);

    Jumper* jumper1 = new Jumper(mGameState.map->getTileCenter(11, 4), {64, 64},
                                 "assets/game/spritesheet-enemies-default.png",
                                 ATLAS, {8, 8}, jumperAtlas);
    jumper1->setJumpingPower(jumperJumpingPower);
    jumper1->setDirection(LEFT);
    jumper1->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(jumper1);

    Jumper* jumper2 = new Jumper(mGameState.map->getTileCenter(5, 7), {64, 64},
                                 "assets/game/spritesheet-enemies-default.png",
                                 ATLAS, {8, 8}, jumperAtlas);
    jumper2->setJumpingPower(jumperJumpingPower);
    jumper2->setDirection(LEFT);
    jumper2->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(jumper2);

    Jumper* jumper3 = new Jumper(mGameState.map->getTileCenter(5, 19), {64, 64},
                                 "assets/game/spritesheet-enemies-default.png",
                                 ATLAS, {8, 8}, jumperAtlas);
    jumper3->setJumpingPower(jumperJumpingPower);
    jumper3->setDirection(LEFT);
    jumper3->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(jumper3);

    Jumper* jumper4 = new Jumper(mGameState.map->getTileCenter(3, 16), {64, 64},
                                 "assets/game/spritesheet-enemies-default.png",
                                 ATLAS, {8, 8}, jumperAtlas);
    jumper4->setJumpingPower(jumperJumpingPower);
    jumper4->setDirection(LEFT);
    jumper4->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemies.push_back(jumper4);
    /*
       ----------- FLAGPOLE -----------
    */
    std::map<AnimationState, std::vector<int>> flagpoleAtlas = {
        {IDLE, {52}}, {WALK, {53, 54}}};

    mGameState.flagpole = new Flagpole(
        mGameState.map->getTileCenter(2, 11), {64, 64},
        "assets/game/spritesheet-tiles-default.png", {18, 18}, flagpoleAtlas);
    mGameState.flagpole->setFrameRate(4.0f);

    mGameState.initialised = true;
}

void LevelC::update(float deltaTime) {
    if (mGameState.winTimer > 0.0f) {
        mGameState.winTimer -= deltaTime;
        if (mGameState.winTimer <= 0.0f) {
            mGameState.nextSceneID = SCENE_WIN;
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

void LevelC::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.flagpole->render();
    mGameState.lilGuy->render();

    for (Entity* enemy : mGameState.enemies)
        enemy->render();
}

void LevelC::shutdown() {
    Scene::shutdown();
}