#ifndef WINSCENE_H
#define WINSCENE_H

#include "Constants.h"
#include "Scene.h"

class WinScene : public Scene {
private:
    Sound mGameWin;

public:
    WinScene();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // WINSCENE_H
