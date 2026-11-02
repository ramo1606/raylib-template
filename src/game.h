// game.h - Presentation layer: scenes, rendering, juice, audio, debug UI.
// Owns a Sim (pure logic) and turns it into pixels and sound.
#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "sim.h"

#define GAME_TITLE   "Orbit Dodge"
#define GAME_VERSION "0.1.0"
#define VIRTUAL_W    640
#define VIRTUAL_H    360
#define FIXED_DT     (1.0f / 60.0f)

typedef enum Scene
{
    SCENE_TITLE = 0,
    SCENE_PLAY,
    SCENE_GAMEOVER
} Scene;

typedef struct Particle
{
    Vector2 pos, vel;
    float life, maxLife;
    Color color;
} Particle;
#define MAX_PARTICLES 256

typedef struct Game
{
    Scene scene;
    float sceneTime; // seconds since entering current scene
    Sim sim;
    Tuning tune; // live tuning (edited by debug panel)
    uint32_t seed;
    bool paused;
    bool showDebug;
    bool quit;              // set by the title screen, read by main loop
    RenderTexture2D target; // fixed virtual resolution, scaled to window
    Texture2D playerTex;    // loaded through assets.c (rres or loose file or placeholder)
    float shake;            // screen shake amount (decays)
    float scorePop;         // 0..1 timer for score "pop" easing
    int best;
    Particle particles[MAX_PARTICLES];
} Game;

void GameInit(Game *g, uint32_t seed);
void GameShutdown(Game *g);
void GameFixedUpdate(Game *g, InputState in);        // called at FIXED_DT
void GameDraw(Game *g);                              // renders into g->target, then to the window
bool GameSaveFrame(const Game *g, const char *path); // exports the virtual framebuffer as PNG

InputState GamePollInput(void);         // keyboard + gamepad
InputState GameBotInput(const Game *g); // simple autoplay bot (smoke tests, attract mode)

#endif // GAME_H
