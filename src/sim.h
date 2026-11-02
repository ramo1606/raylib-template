// sim.h - Pure gameplay simulation (no rendering, no audio, no OS calls).
//
// RULE: nothing in sim.c may call a raylib *function*. Only raylib *types*
// (Vector2, Rectangle, Color) are allowed. This keeps the simulation:
//   - deterministic (same seed + same inputs = same result)
//   - unit-testable without a window (see tests/)
//   - replayable (record InputState per tick, play it back)
#ifndef SIM_H
#define SIM_H

#include "raylib.h" // types only
#include <stdbool.h>
#include <stdint.h>

#define SIM_WORLD_W     640.0f
#define SIM_WORLD_H     360.0f
#define SIM_MAX_ENEMIES 64
#define SIM_MAX_ORBS    4

// One tick of player intent. The platform layer (keyboard, gamepad, bot,
// replay file) produces this; the sim only ever sees this struct.
typedef struct InputState
{
    Vector2 move; // normalized-ish, each axis in [-1, 1]
    bool confirm;
    bool pause;
} InputState;

// Every number a designer might want to tweak lives here. The debug panel
// (raygui) edits a live copy; game-designer agent proposes values in docs.
typedef struct Tuning
{
    float playerAccel;      // px/s^2
    float playerMaxSpeed;   // px/s
    float playerFriction;   // 0..1 velocity kept per second (lower = snappier)
    float enemyBaseSpeed;   // px/s at t=0
    float enemySpeedRamp;   // extra px/s per second survived
    float spawnInterval;    // seconds between enemy spawns at t=0
    float spawnIntervalMin; // floor for spawn interval
} Tuning;

typedef struct Entity
{
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool active;
} Entity;

typedef enum SimEvent
{
    SIM_EVENT_NONE = 0,
    SIM_EVENT_ORB = 1 << 0,   // player collected an orb
    SIM_EVENT_HIT = 1 << 1,   // player died
    SIM_EVENT_SPAWN = 1 << 2, // enemy spawned
} SimEvent;

typedef struct Sim
{
    Tuning tune;
    Entity player;
    Entity enemies[SIM_MAX_ENEMIES];
    Entity orbs[SIM_MAX_ORBS];
    uint32_t rng;
    uint32_t tick;
    float time;
    float spawnTimer;
    int score;
    bool gameOver;
    unsigned int events; // SimEvent bitmask raised during the last SimStep
} Sim;

Tuning SimDefaultTuning(void);
void SimInit(Sim *sim, uint32_t seed, Tuning tune);
void SimStep(Sim *sim, InputState in, float dt);
int SimActiveEnemies(const Sim *sim);

// Deterministic RNG helpers (xorshift32), exposed for tests and tools.
uint32_t SimRandU32(uint32_t *state);
float SimRandRange(uint32_t *state, float min, float max);

#endif // SIM_H
