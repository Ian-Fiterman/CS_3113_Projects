#include "DeathScene.h"

DeathScene::DeathScene() : Scene {} { }

void DeathScene::initialise() {
    mGameState.nextSceneID = -1;
    mGameOver = LoadSound("assets/game/gameOver.mp3");
    mGameState.initialised = true;
    PlaySound(mGameOver);
}

void DeathScene::update(float deltaTime) {
    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = 1;
}

void DeathScene::render() {
    ClearBackground(ColorFromHex("#000000"));

    const char* text = "LilGuy is now DeadGuy...";
    const char* prompt = "Press Enter to Play Again";

    int textSize = 60;
    int promptSize = 30;

    int textWidth = MeasureText(text, textSize);
    int promptWidth = MeasureText(prompt, promptSize);
    DrawText(text, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT / 2 - 60,
             textSize, RED);
    DrawText(prompt, (SCREEN_WIDTH - promptWidth) / 2, SCREEN_HEIGHT / 2 + 20,
             promptSize, GRAY);
}

void DeathScene::shutdown() {
    if (mGameState.initialised) {
        UnloadSound(mGameOver);
        mGameState.initialised = false;
    }
}
