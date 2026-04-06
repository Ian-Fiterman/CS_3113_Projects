#ifndef DEATHSCENE_H
#define DEATHSCENE_H

#include "Constants.h"
#include "Scene.h"

class DeathScene : public Scene {
private:
    Sound mGameOver;

public:
    DeathScene();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // DEATHSCENE_H
