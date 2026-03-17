#include "cs3113.h"
#include "Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH = 1680, SCREEN_HEIGHT = 960, FPS = 120;
constexpr Color BG_COLOUR = BLACK;
constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Text constants
constexpr int HUD_FONT = 20;
constexpr int MEDIUM_FONT = 40;
constexpr int LARGE_FONT = 60;

// Audio Constants
constexpr float FADE_SPEED = 3.0f;
constexpr float MAX_VOLUME = 0.3f;

// Map Constants
constexpr int LEVEL_WIDTH = 28;
constexpr int LEVEL_HEIGHT = 16;
constexpr float TILE_DIMENSION = 60.0f;
constexpr int TILESHEET_ROWS = 1;
constexpr int TILESHEET_COLUMNS = 4;

// Lander Constants
constexpr Vector2 LANDER_INIT_POS = {100.0f, 300.0f};
constexpr Vector2 LANDER_INIT_VEL = {50.0f, 0.0f};
constexpr Vector2 LANDER_SCALE = {50.0f, 50.0f};
constexpr Vector2 LANDER_COLLIDER_DIMENSIONS = {30.0f, 40.0f};
constexpr Vector2 LANDER_COLLIDER_OFFSET = {0.0f, -5.0f};
constexpr float LANDER_INIT_ANGLE = 90.0f;
const std::map<Direction, std::vector<int>> LANDER_ATLAS = {
    {DOWN, {0}}, // idle
    {UP, {1}},   // thrusting
};
constexpr Vector2 LANDER_ATLAS_DIM = {2, 1};

// Spaceship Constants
constexpr float SPACESHIP_HALF_RANGE = (SCREEN_WIDTH - 300.0f) * 0.5f;
constexpr float SPACESHIP_BASE_SPEED = 50.0f;
const Vector2 SPACESHIP_SCALE = {100.0f, 60.0f};
constexpr int MIN_AMPLITUDE = 25;
constexpr int MAX_AMPLITUDE = 75;
const float WAVELENGTH_MULTIPLIER =
    5.0f; // Wavelength is proportional to amplitude

// Flag Constants
constexpr float SHIP_FLAG_X_OFFSET = 13.0f;
const std::map<Direction, std::vector<int>> FLAG_ATLAS = {
    {DOWN, {1}}, // enemy flag
    {UP, {0}},   // ally flag (our flag is "up")
};
constexpr Vector2 FLAG_ATLAS_DIM = {2, 1};

// clang-format off
constexpr unsigned int LEVEL_DATA[] = {
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4,
    4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
};
// clang-format on