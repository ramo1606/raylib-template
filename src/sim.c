// sim.c - see sim.h for the rules of this module.
#include "sim.h"

#include <math.h>
#include <string.h>

//----------------------------------------------------------------------------------
// Small math helpers (kept local so sim.c never links against raylib/raymath)
//----------------------------------------------------------------------------------
static float Len2(Vector2 v) { return v.x * v.x + v.y * v.y; }

static Vector2 Sub(Vector2 a, Vector2 b) { return (Vector2){a.x - b.x, a.y - b.y}; }

static float Clampf(float v, float lo, float hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }

static bool Overlap(const Entity *a, const Entity *b)
{
    float r = a->radius + b->radius;
    return Len2(Sub(a->pos, b->pos)) < r * r;
}

//----------------------------------------------------------------------------------
// RNG
//----------------------------------------------------------------------------------
uint32_t SimRandU32(uint32_t *state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

float SimRandRange(uint32_t *state, float min, float max)
{
    float t = (float)(SimRandU32(state) & 0xFFFFFF) / (float)0xFFFFFF;
    return min + (max - min) * t;
}

//----------------------------------------------------------------------------------
// Spawning
//----------------------------------------------------------------------------------
static void PlaceOrb(Sim *sim, Entity *orb)
{
    // Keep orbs away from the walls and not on top of the player
    for (int tries = 0; tries < 16; tries++)
    {
        orb->pos.x = SimRandRange(&sim->rng, 24.0f, SIM_WORLD_W - 24.0f);
        orb->pos.y = SimRandRange(&sim->rng, 24.0f, SIM_WORLD_H - 24.0f);
        if (Len2(Sub(orb->pos, sim->player.pos)) > 80.0f * 80.0f) break;
    }
    orb->vel = (Vector2){0};
    orb->radius = 6.0f;
    orb->active = true;
}

static void SpawnEnemy(Sim *sim)
{
    for (int i = 0; i < SIM_MAX_ENEMIES; i++)
    {
        Entity *e = &sim->enemies[i];
        if (e->active) continue;

        // Spawn on a random edge, aimed at where the player is now
        int edge = (int)(SimRandU32(&sim->rng) % 4);
        float u = SimRandRange(&sim->rng, 0.0f, 1.0f);
        switch (edge)
        {
            case 0: e->pos = (Vector2){u * SIM_WORLD_W, -10.0f}; break;
            case 1: e->pos = (Vector2){u * SIM_WORLD_W, SIM_WORLD_H + 10.0f}; break;
            case 2: e->pos = (Vector2){-10.0f, u * SIM_WORLD_H}; break;
            default: e->pos = (Vector2){SIM_WORLD_W + 10.0f, u * SIM_WORLD_H}; break;
        }

        Vector2 d = Sub(sim->player.pos, e->pos);
        float len = sqrtf(Len2(d));
        if (len < 0.001f) len = 1.0f;
        float speed = sim->tune.enemyBaseSpeed + sim->tune.enemySpeedRamp * sim->time;
        e->vel = (Vector2){d.x / len * speed, d.y / len * speed};
        e->radius = 8.0f;
        e->active = true;
        sim->events |= SIM_EVENT_SPAWN;
        return;
    }
}

//----------------------------------------------------------------------------------
// Public API
//----------------------------------------------------------------------------------
Tuning SimDefaultTuning(void)
{
    return (Tuning){
        .playerAccel = 2200.0f,
        .playerMaxSpeed = 220.0f,
        .playerFriction = 0.0005f,
        .enemyBaseSpeed = 90.0f,
        .enemySpeedRamp = 1.5f,
        .spawnInterval = 1.2f,
        .spawnIntervalMin = 0.35f,
    };
}

void SimInit(Sim *sim, uint32_t seed, Tuning tune)
{
    memset(sim, 0, sizeof(*sim));
    sim->tune = tune;
    sim->rng = (seed != 0) ? seed : 0x9E3779B9u; // xorshift must not start at 0
    sim->player = (Entity){.pos = {SIM_WORLD_W / 2, SIM_WORLD_H / 2}, .radius = 7.0f, .active = true};
    sim->spawnTimer = tune.spawnInterval;
    for (int i = 0; i < SIM_MAX_ORBS; i++)
        PlaceOrb(sim, &sim->orbs[i]);
}

void SimStep(Sim *sim, InputState in, float dt)
{
    sim->events = SIM_EVENT_NONE;
    if (sim->gameOver) return;

    sim->tick++;
    sim->time += dt;

    // Player: acceleration + exponential friction, clamped to max speed
    Entity *p = &sim->player;
    p->vel.x += in.move.x * sim->tune.playerAccel * dt;
    p->vel.y += in.move.y * sim->tune.playerAccel * dt;
    float keep = powf(sim->tune.playerFriction, dt);
    if (in.move.x == 0.0f) p->vel.x *= keep;
    if (in.move.y == 0.0f) p->vel.y *= keep;

    float speed2 = Len2(p->vel);
    float maxSpeed = sim->tune.playerMaxSpeed;
    if (speed2 > maxSpeed * maxSpeed)
    {
        float s = maxSpeed / sqrtf(speed2);
        p->vel.x *= s;
        p->vel.y *= s;
    }

    p->pos.x = Clampf(p->pos.x + p->vel.x * dt, p->radius, SIM_WORLD_W - p->radius);
    p->pos.y = Clampf(p->pos.y + p->vel.y * dt, p->radius, SIM_WORLD_H - p->radius);

    // Enemy spawning, interval shrinks over time
    sim->spawnTimer -= dt;
    if (sim->spawnTimer <= 0.0f)
    {
        SpawnEnemy(sim);
        float interval = sim->tune.spawnInterval - sim->time * 0.01f;
        sim->spawnTimer = (interval < sim->tune.spawnIntervalMin) ? sim->tune.spawnIntervalMin : interval;
    }

    // Enemies: straight lines, despawn when far outside the world
    for (int i = 0; i < SIM_MAX_ENEMIES; i++)
    {
        Entity *e = &sim->enemies[i];
        if (!e->active) continue;
        e->pos.x += e->vel.x * dt;
        e->pos.y += e->vel.y * dt;
        if ((e->pos.x < -40) || (e->pos.x > SIM_WORLD_W + 40) || (e->pos.y < -40) || (e->pos.y > SIM_WORLD_H + 40))
            e->active = false;
        else if (Overlap(e, p))
        {
            sim->gameOver = true;
            sim->events |= SIM_EVENT_HIT;
        }
    }

    // Orbs
    for (int i = 0; i < SIM_MAX_ORBS; i++)
    {
        Entity *o = &sim->orbs[i];
        if (o->active && Overlap(o, p))
        {
            sim->score++;
            sim->events |= SIM_EVENT_ORB;
            PlaceOrb(sim, o);
        }
    }
}

int SimActiveEnemies(const Sim *sim)
{
    int count = 0;
    for (int i = 0; i < SIM_MAX_ENEMIES; i++)
        count += sim->enemies[i].active ? 1 : 0;
    return count;
}
