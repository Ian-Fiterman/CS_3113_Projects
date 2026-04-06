#include "WinScene.h"

WinScene::WinScene() : Scene {} { }

void WinScene::initialise() {
    mGameState.nextSceneID = -1;
    mGameWin = LoadSound("assets/game/gameWin.mp3");
    mGameState.initialised = true;
    PlaySound(mGameWin);
}

void WinScene::update(float deltaTime) {
    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = 1;
}

void WinScene::render() {
    ClearBackground(ColorFromHex("#000000"));

    const char* text = "LilGuy is now BigGuy!!!";
    const char* prompt = "Press Enter to Play Again";

    int textSize = 60;
    int promptSize = 30;

    int textWidth = MeasureText(text, textSize);
    int promptWidth = MeasureText(prompt, promptSize);
    DrawText(text, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT / 2 - 60,
             textSize, GREEN);
    DrawText(prompt, (SCREEN_WIDTH - promptWidth) / 2, SCREEN_HEIGHT / 2 + 20,
             promptSize, GRAY);
}

void WinScene::shutdown() {
    if (mGameState.initialised) {
        UnloadSound(mGameWin);
        mGameState.initialised = false;
    }
}
