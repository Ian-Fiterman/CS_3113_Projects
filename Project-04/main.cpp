#include "CS3113/Constants.h"
#include "CS3113/DeathScene.h"
#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/Life.h"
#include "CS3113/TitleScene.h"
#include "CS3113/WinScene.h"

constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f, gTimeAccumulator = 0.0f;
Music gBGM;

std::vector<Life*> gLives = {};
int gLivesCount = STARTING_LIVES;

Camera2D gCamera = {0};
Scene* gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

// Function Declarations
void switchToScene(Scene* scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene* scene) {
    bool inMenu =
        gCurrentScene == nullptr || gCurrentScene->getState().lilGuy == nullptr;

    if (gCurrentScene != nullptr) gCurrentScene->shutdown();

    gCurrentScene = scene;
    gCurrentScene->initialise();

    if (inMenu && gCurrentScene->getState().lilGuy != nullptr)
        gLivesCount = STARTING_LIVES;

    if (gCurrentScene->getState().lilGuy != nullptr)
        gCamera.target = gCurrentScene->getState().lilGuy->getPosition();
}

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LilGuy's Lil Venture");
    InitAudioDevice();

    gBGM = LoadMusicStream("assets/game/bgm.mp3");
    SetMusicVolume(gBGM, 0.5f);

    gLevels.push_back(new TitleScene());              // 0
    gLevels.push_back(new LevelA(ORIGIN, "#d7ffa3")); // 1
    gLevels.push_back(new LevelB(ORIGIN, "#c7fdff")); // 2
    gLevels.push_back(new LevelC(ORIGIN, "#f8c7ff")); // 3
    gLevels.push_back(new WinScene());                // 4
    gLevels.push_back(new DeathScene());              // 5

    switchToScene(gLevels[SCENE_TITLE]);

    gCamera.offset = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom = 1.0f;

    for (int i = 0; i < STARTING_LIVES; i++) {
        gLives.push_back(new Life({80.0f + i * 40.0f, 30.0f}, {64.0f, 64.0f},
                                  "assets/game/spritesheet-tiles-default.png",
                                  {18, 18}, i, gLivesCount));
    }

    SetTargetFPS(FPS);
}

void processInput() {
    Player* lilGuy = gCurrentScene->getState().lilGuy;

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    if (lilGuy == nullptr) { return; }

    if (IsKeyDown(KEY_A))
        lilGuy->moveLeft();
    else if (IsKeyDown(KEY_D))
        lilGuy->moveRight();

    if (IsKeyPressed(KEY_W)) {
        lilGuy->jump();
        if (lilGuy->isJumping()) PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (IsKeyPressed(KEY_ONE)) {
        switchToScene(gLevels[SCENE_A]);
        gLivesCount = STARTING_LIVES;
    }
    if (IsKeyPressed(KEY_TWO)) {
        switchToScene(gLevels[SCENE_B]);
        gLivesCount = STARTING_LIVES;
    }
    if (IsKeyPressed(KEY_THREE)) {
        switchToScene(gLevels[SCENE_C]);
        gLivesCount = STARTING_LIVES;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() {
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP) {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        Player* lilGuy = gCurrentScene->getState().lilGuy;
        if (lilGuy == nullptr) continue;

        if (lilGuy->wasHit()) {
            PlaySound(gCurrentScene->getState().hitSound);
            gLivesCount--;
            lilGuy->clearHit();

            if (gLivesCount <= 0) {
                gLivesCount = STARTING_LIVES;
                gCurrentScene->getState().nextSceneID = SCENE_DEATH;
            }
        }

        Vector2 playerPos = lilGuy->getPosition();
        gCurrentScene->getState().map->panCamera(&gCamera, &playerPos,
                                                 SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    if (gCurrentScene->getState().lilGuy != nullptr) {
        if (!IsMusicStreamPlaying(gBGM)) PlayMusicStream(gBGM);
        UpdateMusicStream(gBGM);
    } else {
        if (IsMusicStreamPlaying(gBGM)) StopMusicStream(gBGM);
    }
}

void render() {
    BeginDrawing();

    Player* lilGuy = gCurrentScene->getState().lilGuy;

    if (lilGuy != nullptr) {
        BeginMode2D(gCamera);
        gCurrentScene->render();
        EndMode2D();

        for (Life* life : gLives)
            life->render();
    } else {
        gCurrentScene->render();
    }

    EndDrawing();
}

void shutdown() {
    for (Scene* scene : gLevels)
        delete scene;
    gLevels.clear();

    for (Life* life : gLives)
        delete life;
    gLives.clear();

    UnloadMusicStream(gBGM);

    CloseAudioDevice();
    CloseWindow();
}

int main(void) {
    initialise();

    while (gAppStatus == RUNNING) {
        processInput();
        update();

        int id = gCurrentScene->getState().nextSceneID;
        if (id >= 0) switchToScene(gLevels[id]);

        render();
    }

    shutdown();
    return 0;
}