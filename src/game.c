// game.c - Presentation layer. See game.h.
#include "game.h"
#include "assets.h"

#include "raygui.h"   // declarations only; implementation lives in libs.c
#include "reasings.h" // static inline easing functions

#include <math.h>
#include <stddef.h>

//----------------------------------------------------------------------------------
// Palette: one place for colors, so the technical-artist agent can restyle the game
//----------------------------------------------------------------------------------
#define COL_BG                                                                                                                             \
    (Color) { 18, 16, 28, 255 }
#define COL_GRID                                                                                                                           \
    (Color) { 32, 29, 48, 255 }
#define COL_PLAYER                                                                                                                         \
    (Color) { 120, 220, 255, 255 }
#define COL_ENEMY                                                                                                                          \
    (Color) { 255, 90, 110, 255 }
#define COL_ORB                                                                                                                            \
    (Color) { 255, 210, 90, 255 }
#define COL_TEXT                                                                                                                           \
    (Color) { 235, 230, 250, 255 }
#define COL_DIM                                                                                                                            \
    (Color) { 140, 130, 170, 255 }

//----------------------------------------------------------------------------------
// Particles
//----------------------------------------------------------------------------------
static void EmitBurst(Game *g, Vector2 at, Color color, int count, float speed)
{
    for (int i = 0, made = 0; (i < MAX_PARTICLES) && (made < count); i++)
    {
        Particle *p = &g->particles[i];
        if (p->life > 0.0f) continue;
        float a = (float)GetRandomValue(0, 359) * DEG2RAD;
        float s = speed * (0.4f + 0.6f * (float)GetRandomValue(0, 100) / 100.0f);
        p->pos = at;
        p->vel = (Vector2){cosf(a) * s, sinf(a) * s};
        p->maxLife = p->life = 0.25f + 0.35f * (float)GetRandomValue(0, 100) / 100.0f;
        p->color = color;
        made++;
    }
}

static void UpdateParticles(Game *g, float dt)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        Particle *p = &g->particles[i];
        if (p->life <= 0.0f) continue;
        p->life -= dt;
        p->pos.x += p->vel.x * dt;
        p->pos.y += p->vel.y * dt;
        p->vel.x *= 0.92f;
        p->vel.y *= 0.92f;
    }
}

//----------------------------------------------------------------------------------
// Scene control
//----------------------------------------------------------------------------------
static void ChangeScene(Game *g, Scene next)
{
    g->scene = next;
    g->sceneTime = 0.0f;
    if (next == SCENE_PLAY)
    {
        SimInit(&g->sim, g->seed, g->tune);
        g->seed = SimRandU32(&g->seed); // next run gets a new seed
        g->paused = false;
    }
}

void GameInit(Game *g, uint32_t seed)
{
    *g = (Game){0};
    g->seed = (seed != 0) ? seed : 12345u;
    g->tune = SimDefaultTuning();
    g->target = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);
    SetTextureFilter(g->target.texture, TEXTURE_FILTER_POINT);

    AssetsInit("data.rres", "assets");
    g->playerTex = AssetsLoadTexture("sprites/player.png");

#if defined(GAME_DEBUG)
    g->showDebug = false; // F1 toggles
#endif
    SimInit(&g->sim, g->seed, g->tune); // title screen shows a live background sim
    ChangeScene(g, SCENE_TITLE);
}

void GameShutdown(Game *g)
{
    UnloadTexture(g->playerTex);
    UnloadRenderTexture(g->target);
    AssetsShutdown();
}

void GameFixedUpdate(Game *g, InputState in)
{
    g->sceneTime += FIXED_DT;
    g->shake *= 0.85f;
    if (g->scorePop > 0.0f) g->scorePop -= FIXED_DT * 3.0f;
    UpdateParticles(g, FIXED_DT);

    switch (g->scene)
    {
        case SCENE_TITLE:
        {
            if (in.confirm && (g->sceneTime > 0.3f)) ChangeScene(g, SCENE_PLAY);
            if (in.pause && (g->sceneTime > 0.3f)) g->quit = true;
        }
        break;

        case SCENE_PLAY:
        {
            if (in.pause) g->paused = !g->paused;
            if (g->paused) break;

            g->sim.tune = g->tune; // live tuning from the debug panel
            SimStep(&g->sim, in, FIXED_DT);

            if (g->sim.events & SIM_EVENT_ORB)
            {
                g->scorePop = 1.0f;
                EmitBurst(g, g->sim.player.pos, COL_ORB, 14, 140.0f);
            }
            if (g->sim.events & SIM_EVENT_HIT)
            {
                g->shake = 8.0f;
                EmitBurst(g, g->sim.player.pos, COL_PLAYER, 40, 220.0f);
                if (g->sim.score > g->best) g->best = g->sim.score;
                ChangeScene(g, SCENE_GAMEOVER); // the dead sim stays around for drawing
            }
        }
        break;

        case SCENE_GAMEOVER:
        {
            if (in.confirm && (g->sceneTime > 0.6f)) ChangeScene(g, SCENE_PLAY);
        }
        break;
    }
}

//----------------------------------------------------------------------------------
// Drawing
//----------------------------------------------------------------------------------
static void DrawWorld(const Game *g)
{
    for (int x = 0; x < VIRTUAL_W; x += 32)
        DrawLine(x, 0, x, VIRTUAL_H, COL_GRID);
    for (int y = 0; y < VIRTUAL_H; y += 32)
        DrawLine(0, y, VIRTUAL_W, y, COL_GRID);

    const Sim *s = &g->sim;
    float pulse = 1.0f + 0.15f * sinf((float)GetTime() * 6.0f);
    for (int i = 0; i < SIM_MAX_ORBS; i++)
    {
        if (!s->orbs[i].active) continue;
        DrawCircleV(s->orbs[i].pos, s->orbs[i].radius * pulse, COL_ORB);
    }
    for (int i = 0; i < SIM_MAX_ENEMIES; i++)
    {
        if (!s->enemies[i].active) continue;
        // Arrow-head pointing along velocity. Built from DrawTriangle (counter-clockwise order)
        // because DrawPoly does not render in the raylib 6.2 headless software renderer.
        Vector2 c = s->enemies[i].pos;
        float r = s->enemies[i].radius + 3.0f;
        float a = atan2f(s->enemies[i].vel.y, s->enemies[i].vel.x);
        Vector2 tip = {c.x + cosf(a) * r * 1.3f, c.y + sinf(a) * r * 1.3f};
        Vector2 b1 = {c.x + cosf(a - 2.4f) * r, c.y + sinf(a - 2.4f) * r};
        Vector2 b2 = {c.x + cosf(a + 2.4f) * r, c.y + sinf(a + 2.4f) * r};
        DrawTriangle(tip, b1, b2, COL_ENEMY);
    }

    if (g->scene != SCENE_GAMEOVER)
    {
        float size = s->player.radius * 2.2f;
        Rectangle src = {0, 0, (float)g->playerTex.width, (float)g->playerTex.height};
        Rectangle dst = {s->player.pos.x, s->player.pos.y, size, size};
        DrawTexturePro(g->playerTex, src, dst, (Vector2){size / 2, size / 2}, 0.0f, WHITE);
        DrawCircleLinesV(s->player.pos, s->player.radius + 2.0f, COL_PLAYER);
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        const Particle *p = &g->particles[i];
        if (p->life <= 0.0f) continue;
        DrawCircleV(p->pos, 2.0f * (p->life / p->maxLife) + 0.5f, Fade(p->color, p->life / p->maxLife));
    }
}

static void DrawCenteredText(const char *text, int y, int size, Color color)
{
    DrawText(text, (VIRTUAL_W - MeasureText(text, size)) / 2, y, size, color);
}

static void DrawHud(const Game *g)
{
    float t = (g->scorePop > 0.0f) ? g->scorePop : 0.0f;
    int size = 20 + (int)EaseBackOut(t, 0.0f, 10.0f, 1.0f);
    DrawText(TextFormat("%d", g->sim.score), 12, 8, size, COL_TEXT);
    if (g->paused) DrawCenteredText("PAUSED", VIRTUAL_H / 2 - 10, 20, COL_TEXT);
}

static void DrawScene(const Game *g)
{
    switch (g->scene)
    {
        case SCENE_TITLE:
        {
            DrawWorld(g);
            DrawRectangle(0, 0, VIRTUAL_W, VIRTUAL_H, Fade(COL_BG, 0.6f));
            float t = (g->sceneTime < 1.0f) ? g->sceneTime : 1.0f;
            int y = (int)EaseElasticOut(t, -60.0f, 160.0f, 1.0f);
            DrawCenteredText(GAME_TITLE, y, 40, COL_TEXT);
            if (fmodf(g->sceneTime, 1.0f) < 0.7f) DrawCenteredText("PRESS ENTER", 220, 10, COL_DIM);
        }
        break;

        case SCENE_PLAY:
            DrawWorld(g);
            DrawHud(g);
            break;

        case SCENE_GAMEOVER:
        {
            DrawWorld(g);
            float t = (g->sceneTime < 0.5f) ? g->sceneTime / 0.5f : 1.0f;
            DrawRectangle(0, 0, VIRTUAL_W, VIRTUAL_H, Fade(COL_BG, EaseQuadOut(t, 0.0f, 0.7f, 1.0f)));
            DrawCenteredText("GAME OVER", 120, 30, COL_ENEMY);
            DrawCenteredText(TextFormat("SCORE %d   BEST %d", g->sim.score, g->best), 170, 10, COL_TEXT);
            DrawCenteredText("ENTER TO RETRY", 220, 10, COL_DIM);
        }
        break;
    }
}

static void DrawDebugPanel(Game *g)
{
#if defined(GAME_DEBUG)
    if (IsKeyPressed(KEY_F1)) g->showDebug = !g->showDebug;
    if (!g->showDebug) return;

    Rectangle r = {10, 10, 280, 250};
    GuiPanel(r, "DEBUG (F1)");
    float x = r.x + 110, y = r.y + 32;
    GuiSlider((Rectangle){x, y, 120, 16}, "accel", TextFormat("%.0f", g->tune.playerAccel), &g->tune.playerAccel, 200, 6000);
    y += 22;
    GuiSlider((Rectangle){x, y, 120, 16}, "max speed", TextFormat("%.0f", g->tune.playerMaxSpeed), &g->tune.playerMaxSpeed, 50, 600);
    y += 22;
    GuiSlider((Rectangle){x, y, 120, 16}, "friction", TextFormat("%.4f", g->tune.playerFriction), &g->tune.playerFriction, 0.00001f, 0.5f);
    y += 22;
    GuiSlider((Rectangle){x, y, 120, 16}, "enemy speed", TextFormat("%.0f", g->tune.enemyBaseSpeed), &g->tune.enemyBaseSpeed, 20, 400);
    y += 22;
    GuiSlider((Rectangle){x, y, 120, 16}, "spawn every", TextFormat("%.2f", g->tune.spawnInterval), &g->tune.spawnInterval, 0.1f, 3.0f);
    y += 30;
    GuiLabel((Rectangle){r.x + 10, y, 260, 16},
             TextFormat("FPS %d  tick %u  enemies %d", GetFPS(), g->sim.tick, SimActiveEnemies(&g->sim)));
    y += 20;
    GuiLabel((Rectangle){r.x + 10, y, 260, 16}, TextFormat("seed %u  t=%.1fs", g->seed, g->sim.time));
    y += 24;
    if (GuiButton((Rectangle){r.x + 10, y, 120, 20}, "Reset tuning")) g->tune = SimDefaultTuning();
#else
    (void)g;
#endif
}

void GameDraw(Game *g)
{
    // 1) Render the game at a fixed virtual resolution
    BeginTextureMode(g->target);
    ClearBackground(COL_BG);
    DrawScene(g);
    EndTextureMode();

    // 2) Scale to the window with letterboxing (+ screen shake)
    float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
    float scale = fminf(sw / VIRTUAL_W, sh / VIRTUAL_H);
    float ox = (g->shake > 0.1f) ? (float)GetRandomValue(-100, 100) / 100.0f * g->shake : 0.0f;
    float oy = (g->shake > 0.1f) ? (float)GetRandomValue(-100, 100) / 100.0f * g->shake : 0.0f;
    Rectangle src = {0, 0, (float)VIRTUAL_W, -(float)VIRTUAL_H}; // render textures are y-flipped
    Rectangle dst = {(sw - VIRTUAL_W * scale) / 2 + ox, (sh - VIRTUAL_H * scale) / 2 + oy, VIRTUAL_W * scale, VIRTUAL_H * scale};

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(g->target.texture, src, dst, (Vector2){0}, 0.0f, WHITE);
    DrawDebugPanel(g); // drawn at window resolution so text stays crisp
    EndDrawing();
}

bool GameSaveFrame(const Game *g, const char *path)
{
#if defined(PLATFORM_MEMORY)
    // Headless (rlsw software renderer). Verified on raylib 6.2:
    //  - LoadImageFromTexture() on a render texture returns zeros, so read the screen instead
    //  - LoadImageFromScreen() comes back bottom-up and BGR, so normalize it here
    Image img = LoadImageFromScreen();
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ImageFlipVertical(&img);
    unsigned char *px = (unsigned char *)img.data;
    for (int i = 0; i < img.width * img.height; i++)
    {
        unsigned char t = px[i * 4];
        px[i * 4] = px[i * 4 + 2];
        px[i * 4 + 2] = t;
    }
    ImageResizeNN(&img, VIRTUAL_W, VIRTUAL_H);
#else
    Image img = LoadImageFromTexture(g->target.texture);
    ImageFlipVertical(&img);
#endif
    bool ok = ExportImage(img, path);
    UnloadImage(img);
    return ok;
}

//----------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------
InputState GamePollInput(void)
{
    InputState in = {0};
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) in.move.x -= 1.0f;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) in.move.x += 1.0f;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) in.move.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) in.move.y += 1.0f;
    in.confirm = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    in.pause = IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P);

    if (IsGamepadAvailable(0))
    {
        float ax = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float ay = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        if (fabsf(ax) > 0.2f) in.move.x = ax;
        if (fabsf(ay) > 0.2f) in.move.y = ay;
        in.confirm |= IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
        in.pause |= IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT);
    }
    return in;
}

InputState GameBotInput(const Game *g)
{
    InputState in = {0};
    const Sim *s = &g->sim;

    // Title / game over: press confirm once the scene is ready
    if (g->scene != SCENE_PLAY)
    {
        in.confirm = (g->sceneTime > 1.0f);
        return in;
    }

    // Seek nearest orb
    float best = 1e30f;
    Vector2 goal = s->player.pos;
    for (int i = 0; i < SIM_MAX_ORBS; i++)
    {
        float dx = s->orbs[i].pos.x - s->player.pos.x, dy = s->orbs[i].pos.y - s->player.pos.y;
        float d2 = dx * dx + dy * dy;
        if (s->orbs[i].active && (d2 < best))
        {
            best = d2;
            goal = s->orbs[i].pos;
        }
    }
    Vector2 dir = {goal.x - s->player.pos.x, goal.y - s->player.pos.y};

    // Flee enemies that are close
    for (int i = 0; i < SIM_MAX_ENEMIES; i++)
    {
        if (!s->enemies[i].active) continue;
        float dx = s->player.pos.x - s->enemies[i].pos.x, dy = s->player.pos.y - s->enemies[i].pos.y;
        float d2 = dx * dx + dy * dy;
        if (d2 < 90.0f * 90.0f)
        {
            float w = 9000.0f / (d2 + 1.0f);
            dir.x += dx * w;
            dir.y += dy * w;
        }
    }

    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (len > 1.0f) in.move = (Vector2){dir.x / len, dir.y / len};
    return in;
}
