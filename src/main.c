// main.c - Platform entry: window, fixed-timestep loop, command-line modes.
//
// Command line (all optional):
//   --seed N        deterministic run seed
//   --autoplay      the bot plays instead of the keyboard (attract mode, soak tests)
//   --frames N      run exactly N frames with a fixed dt, then exit (smoke test)
//   --shot PATH     save the virtual framebuffer as PNG when the run ends
//   --shot-every N  also save PATH_<frame>.png every N frames (visual review for agents)
//
// Example used by agents and CI:
//   ./build/headless-debug/game --autoplay --frames 900 --shot build/shots/smoke.png --shot-every 300
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Options
{
    uint32_t seed;
    bool autoplay;
    int frames; // 0 = run until the window closes
    const char *shot;
    int shotEvery;
} Options;

static Options ParseOptions(int argc, char **argv)
{
    Options o = {.seed = 12345u};
    for (int i = 1; i < argc; i++)
    {
        bool hasNext = (i + 1 < argc);
        if (!strcmp(argv[i], "--seed") && hasNext)
            o.seed = (uint32_t)strtoul(argv[++i], NULL, 10);
        else if (!strcmp(argv[i], "--autoplay"))
            o.autoplay = true;
        else if (!strcmp(argv[i], "--frames") && hasNext)
            o.frames = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--shot") && hasNext)
            o.shot = argv[++i];
        else if (!strcmp(argv[i], "--shot-every") && hasNext)
            o.shotEvery = atoi(argv[++i]);
        else
            fprintf(stderr, "unknown argument: %s\n", argv[i]);
    }
    return o;
}

static const char *ShotPathForFrame(const char *base, int frame)
{
    // "dir/smoke.png" -> "dir/smoke_000300.png"
    static char buf[512];
    const char *dot = strrchr(base, '.');
    int stem = (dot != NULL) ? (int)(dot - base) : (int)strlen(base);
    snprintf(buf, sizeof(buf), "%.*s_%06d.png", stem, base, frame);
    return buf;
}

int main(int argc, char **argv)
{
    Options opt = ParseOptions(argc, argv);
    bool smoke = (opt.frames > 0);

#if !defined(GAME_DEBUG)
    SetTraceLogLevel(LOG_WARNING);
#endif
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(VIRTUAL_W * 2, VIRTUAL_H * 2, GAME_TITLE " " GAME_VERSION);
    SetExitKey(KEY_NULL); // ESC is pause, not quit
    if (!smoke) SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()) > 0 ? GetMonitorRefreshRate(GetCurrentMonitor()) : 60);

    static Game game; // big struct: keep it off the stack
    GameInit(&game, opt.seed);

    float accumulator = 0.0f;
    int frame = 0;

    while (!WindowShouldClose() && !game.quit)
    {
        // Smoke runs use a perfectly fixed dt so they are deterministic
        float dt = smoke ? FIXED_DT : GetFrameTime();
        if (dt > 0.25f) dt = 0.25f; // avoid spiral of death after a stall
        accumulator += dt;

        while (accumulator >= FIXED_DT)
        {
            InputState in = opt.autoplay ? GameBotInput(&game) : GamePollInput();
            GameFixedUpdate(&game, in);
            accumulator -= FIXED_DT;
        }

        GameDraw(&game);
        frame++;

        if (opt.shot && (opt.shotEvery > 0) && (frame % opt.shotEvery == 0)) GameSaveFrame(&game, ShotPathForFrame(opt.shot, frame));
        if (smoke && (frame >= opt.frames)) break;
    }

    int exitCode = 0;
    if (opt.shot && !GameSaveFrame(&game, opt.shot)) exitCode = 2;

    // Machine-readable summary line: agents and CI grep for "SMOKE"
    if (smoke)
    {
        printf("SMOKE frames=%d tick=%u score=%d best=%d scene=%d enemies=%d\n", frame, game.sim.tick, game.sim.score, game.best,
               (int)game.scene, SimActiveEnemies(&game.sim));
    }

    GameShutdown(&game);
    CloseWindow();
    return exitCode;
}
