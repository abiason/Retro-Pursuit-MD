#include <assert.h>
#include <stdio.h>
#include "core/game.h"
#include "test_support.h"
#include "systems/animation.h"
enum { PLAYER_LEFT = 16, PLAYER_RIGHT = 288 };

static const WorldRect flat_solids[] = {
    {8, 0, 8, 200}, {304, 0, 8, 200}, {16, 192, 288, 8}
};
static const LevelConfig flat_level = {flat_solids, 3, 0, 0, 152, 168};
static void init_flat(Game *game, uint16_t hz)
{
    test_start_game(game, hz);
    game->level = &flat_level;
    game->enemy.active = 0;
    game->obstacles.count = 0;
    player_init(&game->player, game->level);
}

static Game simulate(uint16_t hz, unsigned frames, InputState input)
{
    Game game;
    unsigned i;
    init_flat(&game, hz);
    for (i = 0; i < frames; ++i) game_update(&game, input);
    return game;
}
static void check_player(uint16_t hz)
{
    Game game;
    InputState idle = {0, 0, 0, 0}, right = {1, 0, 0, 0}, left = {-1, 0, 0, 0}, jump = {0, 1, 0, 0};
    unsigned i;
    int32_t previous_y, minimum_y;
    init_flat(&game, hz);
    game_update(&game, right);
    assert(game.player.vx > 0 && game.player.vx < 256);
    for (i = 0; i < 10; ++i) game_update(&game, right);
    assert(game.player.vx == 256);
    for (i = 0; i < 10; ++i) game_update(&game, idle);
    assert(game.player.vx == 0 && game.player.frame == PLAYER_FRAME_IDLE);
    for (i = 0; i < 200; ++i) game_update(&game, right);
    assert(player_pixel_x(&game.player) == PLAYER_RIGHT);
    for (i = 0; i < 200; ++i) game_update(&game, left);
    assert(player_pixel_x(&game.player) == PLAYER_LEFT && game.player.facing == -1);
    init_flat(&game, hz);
    previous_y = minimum_y = game.player.y;
    game_update(&game, jump);
    assert(game.player.y < previous_y && !game.player.grounded);
    assert(game.player.frame == PLAYER_FRAME_JUMP);
    for (i = 0; i < hz * 2; ++i) {
        game_update(&game, jump);
        if (game.player.y < minimum_y) minimum_y = game.player.y;
        assert(game.player.frame < PLAYER_FRAME_COUNT);
    }
    assert(minimum_y < previous_y - 50 * POSITION_ONE);
    assert(minimum_y > 80 * POSITION_ONE);
    /* Holding A through landing must not trigger another jump. */
    assert(game.player.grounded && game.player.y == previous_y && game.player.vy == 0);
    game_update(&game, idle);
    game_update(&game, jump);
    assert(!game.player.grounded);
    game_update(&game, idle);
    previous_y = game.player.vy;
    game_update(&game, jump);
    assert(game.player.vy > previous_y); /* No mid-air jump reset. */
}
int main(void)
{
    InputState right = {1, 0, 0, 0}, jump = {1, 1, 0, 0};
    Game pal = simulate(50, 50, right), ntsc = simulate(60, 60, right);
    assert(pal.player.x == ntsc.player.x && pal.player.vx == ntsc.player.vx);
    pal = simulate(50, 25, jump); ntsc = simulate(60, 30, jump);
    assert(pal.player.x == ntsc.player.x && pal.player.y == ntsc.player.y);
    assert(pal.player.vy == ntsc.player.vy && pal.player.frame == ntsc.player.frame);
    check_player(50); check_player(60);
    puts("PASS: acceleration, braking, bounds, jump, landing, input edges, animation, PAL/NTSC parity");
    return 0;
}
