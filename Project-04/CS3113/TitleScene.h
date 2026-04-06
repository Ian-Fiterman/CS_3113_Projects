#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "Constants.h"
#include "Scene.h"

class TitleScene : public Scene {
public:
    TitleScene();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;

    void shutdown() override { }
};

#endif // TITLESCENE_H
