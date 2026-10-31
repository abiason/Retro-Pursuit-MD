#include <assert.h>
#include <stdio.h>
#include "core/game.h"
#include "test_support.h"
#include "systems/collision.h"

static const InputState idle = {0, 0, 0, 0}, action = {0, 1, 0, 0}, right = {1, 0, 0, 0};
static void overlap_enemy(Game *game)
{
    game->enemy.body.x = game->player.x;
    game->enemy.body.y = game->player.y;
    game->enemy.body.vx = game->enemy.body.vy = 0;
}
static void capture(uint16_t hz)
{
    Game game;
    uint32_t timer, points;
    int32_t x, enemy_x;
    unsigned i;
    test_start_game(&game, hz);
    assert(game.session.lives == 3 && game.session.round == 1 && !game.session.score);
    game.session.timer.ticks_remaining = 1200;
    overlap_enemy(&game);
    game_update(&game, idle);
    assert(game.session.state == GAME_STATE_CAPTURING);
    assert(game.session.score == 1600);
    points = game.session.score;
    timer = game.session.timer.ticks_remaining;
    x = game.player.x; enemy_x = game.enemy.body.x;
    for (i = 0; i < hz; ++i) {
        game_update(&game, action); /* Holding A cannot skip capture or repeat an award. */
        session_capture(&game.session, &game.rules);
        assert(game.session.score == points);
        assert(game.session.timer.ticks_remaining == timer);
        assert(game.player.x == x && game.enemy.body.x == enemy_x);
    }
    assert(game.session.state == GAME_STATE_LEVEL_COMPLETE);
    game_update(&game, right);
    assert(game.player.x == x);
    game_update(&game, action);
    assert(game.session.state == GAME_STATE_PLAYING && game.session.round == 2);
    assert(game.session.lives == 3 && game.session.score == points);
    assert(game.session.timer.ticks_remaining == 80 * 120);
    assert(game.player.x == (int32_t)level_test.spawn_x * POSITION_ONE);
    game_update(&game, action);
    assert(game.player.grounded); /* Confirmation must not leak into an automatic jump. */
}
static void timeout_and_restart(uint16_t hz)
{
    Game game;
    GameRules rules = game_default_rules;
    unsigned life, i;
    rules.time_limit = 1;
    test_start_game_with_rules(&game, hz, &rules);
    game.session.score = 4321;
    for (life = 3; life; --life) {
        game.enemy.active = 0;
        for (i = 0; i < hz; ++i) game_update(&game, action);
        assert(game.session.lives == life - 1);
        assert(game.session.state == (life > 1 ? GAME_STATE_ATTEMPT_FAILED : GAME_STATE_GAME_OVER));
        assert(!game.session.timer.ticks_remaining && game.session.score == 4321);
        for (i = 0; i < hz; ++i) game_update(&game, action);
        assert(game.session.lives == life - 1); /* No repeated life loss or held-button retry. */
        game_update(&game, idle);
        game_update(&game, action);
        assert(game.session.state == GAME_STATE_PLAYING);
        assert(game.session.timer.ticks_remaining == 120);
        if (life > 1) assert(game.session.lives == life - 1 && game.session.score == 4321);
    }
    assert(game.session.lives == 3 && game.session.score == 0 && game.session.round == 1);
}
static void deadlines_and_contact(void)
{
    Game game;
    Player other;
    GameRules invalid = {0, 0, 0, {0, 0, 0}, 0};
    test_start_game(&game, 60);
    other = game.player;
    other.x += PLAYER_WIDTH * POSITION_ONE;
    assert(!collision_bodies_overlap(&game.player, &other));
    --other.x;
    assert(collision_bodies_overlap(&game.player, &other));
    game.session.timer.ticks_remaining = 1;
    overlap_enemy(&game);
    game_update(&game, idle);
    assert(game.session.state == GAME_STATE_ATTEMPT_FAILED && !game.session.score);
    assert(game.session.lives == 2); /* Deadline has priority over simultaneous overlap. */
    test_start_game(&game, 60);
    game.enemy.active = 0;
    game_update(&game, idle);
    assert(game.session.state == GAME_STATE_PLAYING);
    game.session.state = GAME_STATE_LEVEL_COMPLETE;
    game.session.round = 65535;
    game_update(&game, action);
    assert(game.session.round == 2 && game.stage_index == 1);
    test_start_game_with_rules(&game, 99, &invalid);
    assert(game.refresh_rate == 60 && game.session.lives == 1);
    assert(game.rules.capture_duration == 1 && game.session.timer.ticks_remaining == 120);
}
static Game simulate(uint16_t hz)
{
    Game game;
    unsigned i;
    test_start_game(&game, hz);
    for (i = 0; i < hz * 20U; ++i) game_update(&game, idle);
    return game;
}
int main(void)
{
    Game pal, ntsc;
    capture(50); capture(60);
    timeout_and_restart(50); timeout_and_restart(60);
    deadlines_and_contact();
    pal = simulate(50); ntsc = simulate(60);
    assert(pal.session.state == ntsc.session.state && pal.session.lives == ntsc.session.lives);
    assert(pal.session.score == ntsc.session.score);
    assert(pal.session.timer.ticks_remaining == ntsc.session.timer.ticks_remaining);
    puts("PASS: capture once, frozen result, next round, timeout/lives, game over/restart, deadline, PAL/NTSC");
    return 0;
}
