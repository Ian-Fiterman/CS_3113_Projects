#include "TitleScene.h"

TitleScene::TitleScene() : Scene {} { }

void TitleScene::initialise() {
    mGameState.nextSceneID = -1;
}

void TitleScene::update(float deltaTime) {
    if (IsKeyPressed(KEY_ENTER)) mGameState.nextSceneID = SCENE_A;
}

void TitleScene::render() {

    ClearBackground(ColorFromHex("#000000"));

    const char* title = "LilGuy's Lil Venture";
    const char* prompt = "Press Enter to Start";

    int titleSize = 60;
    int promptSize = 30;

    int titleWidth = MeasureText(title, titleSize);
    int promptWidth = MeasureText(prompt, promptSize);

    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, SCREEN_HEIGHT / 2 - 60,
             titleSize, WHITE);
    DrawText(prompt, (SCREEN_WIDTH - promptWidth) / 2, SCREEN_HEIGHT / 2 + 20,
             promptSize, GRAY);
}
