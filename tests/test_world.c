#include <assert.h>
#include <stdio.h>
#include "core/game.h"
#include "test_support.h"
#include "systems/collision.h"

static void tick(Player *p, int8_t horizontal, int8_t vertical, uint16_t jump)
{
    player_update(p, &level_test, horizontal, vertical, jump);
    if (p->ladder_index < 0) assert(!collision_player_overlaps(p, &level_test));
}
static void ladder_round_trip(uint16_t index)
{
    const Ladder *ladder = &level_test.ladders[index];
    Player p;
    unsigned i;
    player_init(&p, &level_test);
    p.x = (int32_t)ladder->x * POSITION_ONE;
    p.y = (ladder->bottom - PLAYER_HEIGHT) * POSITION_ONE;
    tick(&p, 0, -1, 0);
    assert(p.ladder_index == (int16_t)index);
    {
        const int32_t paused_y = p.y;
        tick(&p, 0, 0, 1); /* A cannot jump through the floor from a ladder. */
        assert(p.y == paused_y && p.ladder_index >= 0);
    }
    for (i = 0; i < 100; ++i) tick(&p, 0, -1, 0);
    assert(p.y == (ladder->top - PLAYER_HEIGHT) * POSITION_ONE && p.grounded);
    assert(p.ladder_index < 0);
    for (i = 0; i < 100; ++i) tick(&p, 0, 1, 0);
    assert(p.y == (ladder->bottom - PLAYER_HEIGHT) * POSITION_ONE && p.grounded);
}
static void route(void)
{
    Player p;
    unsigned i, limit = 0;
    player_init(&p, &level_test);
    while (player_pixel_x(&p) < 64 && ++limit < 100) tick(&p, 1, 0, 0);
    assert(limit < 100);
    for (i = 0; i < 12; ++i) tick(&p, 0, 0, 0);
    for (i = 0; i < 100; ++i) tick(&p, 0, -1, 0);
    assert(player_pixel_y(&p) == 120);
    limit = 0;
    while (player_pixel_x(&p) < 104 && ++limit < 100) tick(&p, 1, 0, 0);
    assert(limit < 100);
    tick(&p, 1, 0, 1);
    limit = 0;
    while (player_pixel_x(&p) < 240 && ++limit < 200) tick(&p, 1, 0, 0);
    assert(limit < 200);
    for (i = 0; i < 12; ++i) tick(&p, 0, 0, 0);
    for (i = 0; i < 100; ++i) tick(&p, 0, -1, 0);
    assert(player_pixel_y(&p) == 72 && p.grounded);
}
static void platform_and_ladder_exit(void)
{
    const WorldRect platform = {16, 144, 32, 8};
    const LevelConfig level = {&platform, 1, 0, 0, 24, 120};
    Player p;
    unsigned i;
    player_init(&p, &level);
    for (i = 0; i < 40; ++i) player_update(&p, &level, 1, 0, 0);
    assert(!p.grounded && p.y > 120 * POSITION_ONE);
    player_init(&p, &level_test);
    p.x = 64 * POSITION_ONE;
    for (i = 0; i < 8; ++i) tick(&p, 0, -1, 0);
    tick(&p, 1, 0, 0);
    assert(p.ladder_index < 0); /* Sideways exit clear of a floor. */
    player_init(&p, &level_test);
    p.x = 64 * POSITION_ONE;
    for (i = 0; i < 40; ++i) tick(&p, 0, -1, 0);
    assert(collision_player_overlaps(&p, &level_test));
    tick(&p, 1, 0, 0);
    assert(p.ladder_index == 0); /* Cannot leave sideways inside a solid floor. */
    for (i = 0; i < 50; ++i) tick(&p, 0, -1, 0);
    assert(p.grounded && p.ladder_index < 0);
    player_init(&p, &level_test);
    for (i = 0; i < 10; ++i) tick(&p, 0, -1, 0);
    assert(p.ladder_index < 0 && p.grounded); /* Up away from a ladder is inert. */
}
static Game climb_game(uint16_t hz)
{
    Game game;
    InputState up = {0, 0, -1, 0};
    unsigned i;
    test_start_game(&game, hz);
    game.player.x = 64 * POSITION_ONE;
    for (i = 0; i < hz; ++i) game_update(&game, up);
    return game;
}
int main(void)
{
    Game pal, ntsc;
    ladder_round_trip(0);
    ladder_round_trip(1);
    platform_and_ladder_exit();
    route();
    pal = climb_game(50); ntsc = climb_game(60);
    assert(pal.player.x == ntsc.player.x && pal.player.y == ntsc.player.y);
    assert(pal.player.grounded && ntsc.player.grounded);
    puts("PASS: both ladders, pause/exit, platform edge, three-floor route, PAL/NTSC");
    return 0;
}
