// test_sim.c - Unit tests for the pure simulation. Builds and runs without raylib.
#include "test.h"
#include "../src/sim.h"

#include <string.h>

static const InputState NO_INPUT = {0};

static void test_init_places_player_center(void)
{
    Sim s;
    SimInit(&s, 1, SimDefaultTuning());
    CHECK_NEAR(s.player.pos.x, SIM_WORLD_W / 2, 0.01f);
    CHECK_NEAR(s.player.pos.y, SIM_WORLD_H / 2, 0.01f);
    CHECK(s.score == 0);
    CHECK(!s.gameOver);
}

static void test_determinism_same_seed_same_state(void)
{
    Sim a, b;
    SimInit(&a, 42, SimDefaultTuning());
    SimInit(&b, 42, SimDefaultTuning());
    InputState in = {.move = {1.0f, 0.5f}};
    for (int i = 0; i < 600; i++)
    {
        SimStep(&a, in, 1.0f / 60.0f);
        SimStep(&b, in, 1.0f / 60.0f);
    }
    CHECK(memcmp(&a, &b, sizeof(Sim)) == 0);
}

static void test_player_stays_in_bounds(void)
{
    Sim s;
    SimInit(&s, 7, SimDefaultTuning());
    s.tune.spawnInterval = s.tune.spawnIntervalMin = 9999.0f; // no enemies
    s.spawnTimer = 9999.0f;
    InputState in = {.move = {-1.0f, -1.0f}};
    for (int i = 0; i < 600; i++)
        SimStep(&s, in, 1.0f / 60.0f);
    CHECK(s.player.pos.x >= s.player.radius - 0.01f);
    CHECK(s.player.pos.y >= s.player.radius - 0.01f);
}

static void test_max_speed_is_respected(void)
{
    Sim s;
    SimInit(&s, 7, SimDefaultTuning());
    InputState in = {.move = {1.0f, 1.0f}};
    for (int i = 0; i < 10; i++)
        SimStep(&s, in, 1.0f / 60.0f);
    float v2 = s.player.vel.x * s.player.vel.x + s.player.vel.y * s.player.vel.y;
    CHECK(v2 <= s.tune.playerMaxSpeed * s.tune.playerMaxSpeed + 0.5f);
}

static void test_orb_pickup_scores(void)
{
    Sim s;
    SimInit(&s, 3, SimDefaultTuning());
    s.orbs[0].pos = s.player.pos; // force overlap
    SimStep(&s, NO_INPUT, 1.0f / 60.0f);
    CHECK(s.score == 1);
    CHECK(s.events & SIM_EVENT_ORB);
}

static void test_enemy_contact_ends_game(void)
{
    Sim s;
    SimInit(&s, 3, SimDefaultTuning());
    s.enemies[0] = (Entity){.pos = s.player.pos, .radius = 8.0f, .active = true};
    SimStep(&s, NO_INPUT, 1.0f / 60.0f);
    CHECK(s.gameOver);
    CHECK(s.events & SIM_EVENT_HIT);
    uint32_t tick = s.tick;
    SimStep(&s, NO_INPUT, 1.0f / 60.0f);
    CHECK(s.tick == tick); // frozen after game over
}

static void test_enemies_spawn_over_time(void)
{
    Sim s;
    SimInit(&s, 9, SimDefaultTuning());
    int spawns = 0;
    for (int i = 0; i < 60 * 5; i++)
    {
        SimStep(&s, NO_INPUT, 1.0f / 60.0f);
        if (s.events & SIM_EVENT_SPAWN) spawns++;
        if (s.gameOver) break;
    }
    CHECK(spawns >= 3);
}

int main(void)
{
    RUN(test_init_places_player_center);
    RUN(test_determinism_same_seed_same_state);
    RUN(test_player_stays_in_bounds);
    RUN(test_max_speed_is_respected);
    RUN(test_orb_pickup_scores);
    RUN(test_enemy_contact_ends_game);
    RUN(test_enemies_spawn_over_time);
    return TEST_SUMMARY();
}
