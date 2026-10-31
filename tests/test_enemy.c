#include <assert.h>
#include <stdio.h>
#include "core/game.h"
#include "test_support.h"
#include "systems/collision.h"

static void check_graph(void)
{
    const NavigationNode nodes[] = {{32, 192}, {64, 192}, {240, 144}};
    const NavigationEdge edge = {0, 1, -1};
    const NavigationGraph disconnected = {nodes, 3, &edge, 1};
    const NavigationGraph too_large = {nodes, NAVIGATION_MAX_NODES + 1, 0, 0};
    assert(navigation_next_hop(&navigation_test, 9, 0) == 8);
    assert(navigation_next_hop(&navigation_test, 1, 8) == 4);
    assert(navigation_next_hop(&navigation_test, 5, 5) == 5);
    assert(navigation_next_hop(&disconnected, 0, 2) == -1);
    assert(navigation_next_hop(&disconnected, 0, 99) == -1);
    assert(navigation_nearest(&too_large, 0, 0) == -1);
    assert(navigation_nearest(0, 0, 0) == -1);
    assert(navigation_escape_goal(&navigation_test, 32, 192) == 9);
}
static void check_patrol(void)
{
    Enemy enemy;
    Player observer;
    unsigned tick, floors = 0, states = 0, endpoints = 0;
    enemy_init(&enemy, &level_test, &navigation_test, &enemy_test_config);
    player_init(&observer, &level_test);
    observer.x = -1000 * POSITION_ONE; /* Isolate patrol from perception. */
    for (tick = 0; tick < 120 * 40; ++tick) {
        enemy_update(&enemy, &level_test, &observer);
        states |= 1U << enemy.state;
        if (enemy.body.ladder_index < 0)
            assert(!collision_player_overlaps(&enemy.body, &level_test));
        if (enemy.body.grounded) {
            int16_t feet = player_pixel_y(&enemy.body) + PLAYER_HEIGHT;
            if (feet == 192) floors |= 1;
            if (feet == 144) floors |= 2;
            if (feet == 96) floors |= 4;
        }
        if (enemy.current_node == 0) endpoints |= 1;
        if (enemy.current_node == 9 && tick > 1000) endpoints |= 2;
        assert(enemy.body.x >= 16 * POSITION_ONE && enemy.body.x <= 288 * POSITION_ONE);
        assert(enemy.body.y >= 48 * POSITION_ONE && enemy.body.y <= 168 * POSITION_ONE);
    }
    assert(floors == 7 && endpoints == 3);
    assert(states & (1U << ENEMY_PATROL));
    assert(states & (1U << ENEMY_CLIMB));
    assert(states & (1U << ENEMY_DESCEND));
    assert(states & (1U << ENEMY_IDLE));
}
static void check_escape_and_invalid(void)
{
    Enemy enemy;
    Player observer;
    EnemyConfig config = enemy_test_config;
    unsigned tick, saw_escape = 0;
    config.spawn_node = 1;
    enemy_init(&enemy, &level_test, &navigation_test, &config);
    player_init(&observer, &level_test);
    observer.x = 32 * POSITION_ONE;
    for (tick = 0; tick < 600; ++tick) {
        enemy_update(&enemy, &level_test, &observer);
        if (enemy.state == ENEMY_ESCAPE) saw_escape = 1;
        if (enemy.body.ladder_index < 0)
            assert(!collision_player_overlaps(&enemy.body, &level_test));
    }
    assert(saw_escape);
    config.spawn_node = 5;
    enemy_init(&enemy, &level_test, &navigation_test, &config);
    observer.x = 224 * POSITION_ONE;
    observer.y = 120 * POSITION_ONE;
    enemy_update(&enemy, &level_test, &observer);
    assert(enemy.state == ENEMY_CLIMB && enemy.body.ladder_index == 1);
    for (tick = 0; tick < 100; ++tick) enemy_update(&enemy, &level_test, &observer);
    assert(player_pixel_y(&enemy.body) == 72); /* Reached the upper floor to escape. */
    config.spawn_node = 100;
    enemy_init(&enemy, &level_test, &navigation_test, &config);
    assert(!enemy.active && enemy.state == ENEMY_IDLE);
    enemy_update(&enemy, &level_test, &observer);
}
static Game simulate(uint16_t hz)
{
    Game game;
    InputState idle = {0, 0, 0, 0};
    unsigned i;
    test_start_game(&game, hz);
    for (i = 0; i < hz * 30U; ++i) game_update(&game, idle);
    return game;
}
int main(void)
{
    Game pal, ntsc;
    check_graph();
    check_patrol();
    check_escape_and_invalid();
    pal = simulate(50); ntsc = simulate(60);
    assert(pal.enemy.body.x == ntsc.enemy.body.x && pal.enemy.body.y == ntsc.enemy.body.y);
    assert(pal.enemy.state == ntsc.enemy.state && pal.enemy.target_node == ntsc.enemy.target_node);
    assert(pal.enemy.flee_ticks == ntsc.enemy.flee_ticks);
    puts("PASS: graph bounds, routes, 40s patrol, three floors, escape, PAL/NTSC parity");
    return 0;
}
