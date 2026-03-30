/**
 * Author: Ian Fiterman
 * Assignment: Project 3: Lunar Lander
 * Date due: 03/14/2024
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/Constants.h"
#include "CS3113/Lander.h"
#include "CS3113/Spaceship.h"

// Global Variables
struct GameState {
    Lander* lander;
    Spaceship* spaceship;
    Map* map;
    Entity* flagPole;
    Entity* shipFlag;
    Entity* explosion;
    Music thrusterSound;
    Sound explosionSound;

    float thrusterVolume = 0.0f;
    bool gameWon = false;
    bool gameLost = false;
    Vector2 flagPolePosition = {0, 0};

    bool paused = true;
    bool started = false;
};

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f, gTimeAccumulator = 0.0f;

GameState gState;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// New Functions
void resetGame();
void randomizeFlagPole();
void updateThrusterAudio(float deltaTime);
void renderText();
void renderHUD();
void checkLanded();
void checkCrashed();
void checkOutOfBounds();
void randomizeSpaceship();

int main(void) {
    initialise();

    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");
    InitAudioDevice();

    // Load audio
    gState.thrusterSound = LoadMusicStream("assets/thrust_sound.mp3");
    gState.thrusterSound.looping = true;
    PlayMusicStream(gState.thrusterSound);
    PauseMusicStream(gState.thrusterSound);
    gState.explosionSound = LoadSound("assets/explosion.mp3");

    // Initialize map
    gState.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, (unsigned int*)LEVEL_DATA,
                         "assets/tileset.png", TILE_DIMENSION, TILESHEET_ROWS,
                         TILESHEET_COLUMNS, ORIGIN);

    gState.lander =
        new Lander(LANDER_INIT_POS, LANDER_SCALE, "assets/atlas.png", ATLAS,
                   LANDER_ATLAS_DIM, LANDER_ATLAS);
    gState.lander->setColliderDimensions(LANDER_COLLIDER_DIMENSIONS);
    gState.lander->setColliderOffset(LANDER_COLLIDER_OFFSET);

    // Initialize spaceship
    gState.spaceship =
        new Spaceship(ORIGIN, SPACESHIP_SCALE, "assets/spaceship.png");
    gState.spaceship->setHalfRange(SPACESHIP_HALF_RANGE);
    gState.spaceship->setBaseSpeed(SPACESHIP_BASE_SPEED);

    // Initialize explosion vfx entity (hidden until crash)
    gState.explosion = new Entity(ORIGIN, {TILE_DIMENSION, TILE_DIMENSION},
                                  "assets/explosion.png");

    // Intitialize flagpole and ship flag with animation atlas
    gState.flagPole = new Entity(
        {gState.flagPolePosition}, {TILE_DIMENSION, TILE_DIMENSION},
        "assets/nyu_flagpole.png", ATLAS, FLAG_ATLAS_DIM, FLAG_ATLAS);
    gState.shipFlag =
        new Entity({gState.spaceship->getPosition().x + SHIP_FLAG_X_OFFSET,
                    gState.spaceship->getPosition().y - TILE_DIMENSION},
                   {TILE_DIMENSION, TILE_DIMENSION}, "assets/nyu_flagpole.png",
                   ATLAS, FLAG_ATLAS_DIM, FLAG_ATLAS);
    gState.shipFlag->setDirection(DOWN); // Start with enemy flag sprite

    resetGame();                         // Set initial game state

    SetTargetFPS(FPS);
}

void processInput() {
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    if (IsKeyPressed(KEY_P)) { // Pause/unpause game
        if (!gState.gameWon && !gState.gameLost) {
            gState.paused = !gState.paused;
            if (!gState.started)
                gState.started = true; // Mark game as started on first unpause
            if (!gState.paused)
                gPreviousTicks = (float)GetTime(); // Reset timer on unpause
        }
    }

    if (IsKeyPressed(KEY_R)) resetGame(); // Reset game state

    if (gState.paused) return;

    // Lander controls
    if (IsKeyDown(KEY_A)) gState.lander->rotateLeft();
    if (IsKeyDown(KEY_D)) gState.lander->rotateRight();
    if (IsKeyDown(KEY_W)) gState.lander->thrust();
}

void update() {
    if (gState.paused) return;
    // Delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP) {
        gTimeAccumulator = deltaTime;
        return;
    }

    UpdateMusicStream(gState.thrusterSound); // Update thruster sound stream

    // Physics loop with fixed timestep
    while (deltaTime >= FIXED_TIMESTEP) {
        gState.lander->update(FIXED_TIMESTEP, gState.spaceship, 1, gState.map);
        gState.spaceship->update(FIXED_TIMESTEP, nullptr, 0, nullptr);
        gState.flagPole->animate(FIXED_TIMESTEP);

        // Ship flag follows spaceship if not yet collected
        if (gState.shipFlag->isActive()) {
            gState.shipFlag->setPosition(
                {gState.spaceship->getPosition().x + SHIP_FLAG_X_OFFSET,
                 gState.spaceship->getPosition().y - TILE_DIMENSION});
        }

        if (gState.lander->isCollidingWith(gState.shipFlag))
            gState.shipFlag->deactivate();

        if (gState.lander->isCollidingWith(gState.flagPole)
            && !gState.shipFlag->isActive())
            gState.flagPole->setDirection(UP);

        updateThrusterAudio(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    // Check win/lose conditions after physics updates
    checkLanded();
    checkCrashed();
    checkOutOfBounds();

    gState.lander->resetControls();
    gTimeAccumulator = deltaTime;
}

void render() {
    BeginDrawing();
    ClearBackground(BG_COLOUR);

    // Render entities
    gState.map->render();
    gState.flagPole->render();
    gState.shipFlag->render();
    gState.lander->render();
    gState.spaceship->render();
    gState.explosion->render();

    // Render text
    renderHUD();
    renderText();

    EndDrawing();
}

void shutdown() {
    delete gState.lander;
    delete gState.spaceship;
    delete gState.map;
    delete gState.flagPole;
    delete gState.shipFlag;
    delete gState.explosion;

    UnloadMusicStream(gState.thrusterSound);
    UnloadSound(gState.explosionSound);
    CloseAudioDevice();

    CloseWindow();
}

/**
 * @brief Updates thruster audio to smoothly fade in/out
 * @param deltaTime
 */
void updateThrusterAudio(float deltaTime) {
    // Determine target volume based on thrusting state and smoothly interpolate
    // towards it
    float targetVolume = gState.lander->isThrusting() ? MAX_VOLUME : 0.0f;
    gState.thrusterVolume +=
        (targetVolume - gState.thrusterVolume) * FADE_SPEED * deltaTime;
    SetMusicVolume(gState.thrusterSound, gState.thrusterVolume);

    // Pause or resume thruster sound based on thrusting state and volume
    if (gState.lander->isThrusting()) {
        if (!IsMusicStreamPlaying(gState.thrusterSound))
            ResumeMusicStream(gState.thrusterSound);
    } else {
        if (gState.thrusterVolume <= 0.0f
            && IsMusicStreamPlaying(gState.thrusterSound))
            PauseMusicStream(gState.thrusterSound);
    }
}

/**
 * @brief Resets the game state to initial conditions for a new game
 */
void resetGame() {
    // Reset game state
    gState.gameWon = false;
    gState.gameLost = false;
    gState.paused = true;
    gState.started = false;
    gState.thrusterVolume = 0.0f;
    // Reset lander position, velocity, angle, and state
    gState.lander->setPosition(LANDER_INIT_POS);
    gState.lander->setVelocity(LANDER_INIT_VEL);
    gState.lander->setAngle(LANDER_INIT_ANGLE);
    gState.lander->refuel();
    gState.lander->setDirection(DOWN);
    gState.lander->activate();
    // Reset spaceship position and state
    randomizeSpaceship();
    // Reset flagpole and ship flag
    randomizeFlagPole();
    gState.shipFlag->activate();
    gState.shipFlag->setPosition(
        {gState.spaceship->getPosition().x + SHIP_FLAG_X_OFFSET,
         gState.spaceship->getPosition().y - TILE_DIMENSION});
    // Reset thruster sound and explosion sound
    SeekMusicStream(gState.thrusterSound, 0);
    PauseMusicStream(gState.thrusterSound);
    StopSound(gState.explosionSound);
    // Reset explosion state
    gState.explosion->deactivate();
    // Reset timing
    gPreviousTicks = (float)GetTime();
}

/**
 * @brief Randomizes the spawn of the flagpole on the surface tiles
 */
void randomizeFlagPole() {
    // Get all surface tiles and randomly select one for the flagpole position
    std::vector<Vector2> surface = gState.map->getTerrainSurface();
    int index = GetRandomValue(0, surface.size() - 1);
    gState.flagPolePosition = surface[index];
    gState.flagPole->setPosition(gState.flagPolePosition);
    gState.flagPole->setDirection(DOWN);
}

/**
 * @brief Renders the HUD with altitude, speed, fuel, and angle information
 */
void renderHUD() {
    DrawText(TextFormat("Altitude:  %.0f",
                        gState.map->getAltitude(gState.lander->getPosition())),
             100, 20, HUD_FONT, WHITE);
    DrawText(TextFormat("H-Speed:   %.1f", gState.lander->getVelocity().x), 100,
             45, HUD_FONT, WHITE);
    DrawText(TextFormat("V-Speed:   %.1f", gState.lander->getVelocity().y), 100,
             70, HUD_FONT, WHITE);
    DrawText(TextFormat("Fuel:      %.0f", gState.lander->getFuel()), 100, 95,
             HUD_FONT, WHITE);
    DrawText(TextFormat("Angle:     %.0f", gState.lander->getAngle()), 100, 120,
             HUD_FONT, WHITE);
}

/**
 * @brief Renders centered text on the screen based on game state
 * (win/lose/pause)
 */
void renderText() {
    const char* text = nullptr;
    Color color = WHITE;
    int fontSize;

    if (gState.gameWon) { // Game won
        text = "YOU WIN!";
        color = GREEN;
        fontSize = LARGE_FONT;
    } else if (gState.gameLost) { // Game lost
        text = "GAME OVER";
        color = RED;
        fontSize = LARGE_FONT;
    } else if (gState.paused) { // Game paused
        text = gState.started ? "PAUSED" : "Press P to Play";
        color = WHITE;
        fontSize = MEDIUM_FONT;
    }
    // Draw text centered on screen
    if (text != nullptr) {
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, SCREEN_WIDTH / 2 - textWidth / 2,
                 SCREEN_HEIGHT / 2 - fontSize / 2, fontSize, color);
    }
}

/**
 * @brief Checks if the Lander has landed and determines win/loss based on
 * landing conditions
 */
void checkLanded() {
    if (gState.lander->hasLanded()) {
        auto landerTile = // I bet this word terrifies Prof. Romero Cruz
            gState.map->getTileIndices(gState.lander->getPosition());
        auto winTile = gState.map->getTileIndices(gState.flagPolePosition);

        bool flagCollected = !gState.shipFlag->isActive();
        bool flagPlanted = gState.flagPole->getDirection() == UP;
        bool correctTile = landerTile == winTile;

        if (flagCollected && flagPlanted && correctTile) {
            gState.gameWon = true; // Landed on flagpole with flag raised = win
        } else {
            gState.gameLost =
                true;         // Landed elsewhere or flag not raised = loss
        }
        gState.paused = true; // Pause game on landing
        gState.lander->setDirection(
            DOWN);            // Set lander animation to idle on landing
        gState.lander->animate(
            gState.lander->getFrameSpeed()); // Update lander animation frame
        gState.lander
            ->resetStates(); // Reset landed/crashed states for next game
    }
}

/**
 * @brief Checks if the Lander has crashed and triggers explosion if so
 */
void checkCrashed() {
    if (gState.lander->hasCrashed()) {
        PlaySound(gState.explosionSound);
        gState.gameLost = true;      // Set game lost on crash
        gState.paused = true;        // Pause game on crash
        gState.lander->deactivate(); // Hide lander on crash
        gState.explosion->setPosition(gState.lander->getPosition());
        gState.explosion->activate();
        gState.lander
            ->resetStates(); // Reset landed/crashed states for next game
    }
}

/**
 @brief Checks if the Lander has gone out of bounds
 */
void checkOutOfBounds() {
    // Get lander collider bounds
    Vector2 landerPos = gState.lander->getPosition();
    Vector2 colliderOffset = gState.lander->getColliderOffset();
    Vector2 colliderDims = gState.lander->getColliderDimensions();
    // Check if lander collider is outside screen bounds
    float left = landerPos.x + colliderOffset.x - colliderDims.x / 2.0f;
    float right = landerPos.x + colliderOffset.x + colliderDims.x / 2.0f;
    float top = landerPos.y + colliderOffset.y - colliderDims.y / 2.0f;
    float bottom = landerPos.y + colliderOffset.y + colliderDims.y / 2.0f;
    // If any part of the collider is out of bounds, trigger crash but no vfx
    if (right < 0 || left > SCREEN_WIDTH || bottom < 0 || top > SCREEN_HEIGHT) {
        PlaySound(gState.explosionSound);
        gState.gameLost = true;
        gState.paused = true;
        gState.lander->deactivate();
        gState.lander->resetStates();
    }
}

/**
 @brief Randomizes the spaceship's properties for a new game
 */
void randomizeSpaceship() {
    float amplitude = GetRandomValue(MIN_AMPLITUDE, MAX_AMPLITUDE);
    float wavelength =
        amplitude * WAVELENGTH_MULTIPLIER; // proportional to amplitude

    gState.spaceship->setAmplitude(amplitude);
    gState.spaceship->setWavelength(wavelength);

    float thirdRange = (SPACESHIP_HALF_RANGE * 2.0f) / 3.0f;
    float minDist = SPACESHIP_HALF_RANGE - thirdRange; // start of outer third
    float maxDist = SPACESHIP_HALF_RANGE;              // end of range

    gState.spaceship->setDistance(
        GetRandomValue(static_cast<int>(minDist), static_cast<int>(maxDist)));
    gState.spaceship->setDirection(-1.0f); // always moving toward lander
    gState.spaceship->updatePositionFromDistance();
}