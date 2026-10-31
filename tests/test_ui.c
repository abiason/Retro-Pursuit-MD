#include <assert.h>
#include <stdio.h>
#include "core/game.h"

static const InputState idle = {0, 0, 0, 0}, action = {0, 1, 0, 0};
static const InputState start = {0, 0, 0, 1}, down = {0, 0, 1, 0};

static void body_equal(const Player *a, const Player *b)
{
    assert(a->x == b->x && a->y == b->y && a->vx == b->vx && a->vy == b->vy);
    assert(a->grounded == b->grounded && a->ladder_index == b->ladder_index);
    assert(a->frame == b->frame && a->animation_ticks == b->animation_ticks && a->facing == b->facing);
}
static void simulation_equal(const Game *a, const Game *b)
{
    unsigned i;
    assert(a->session.timer.ticks_remaining == b->session.timer.ticks_remaining);
    assert(a->session.score == b->session.score && a->session.lives == b->session.lives);
    assert(a->session.round == b->session.round && a->session.capture_ticks == b->session.capture_ticks);
    assert(a->session.invulnerability_ticks == b->session.invulnerability_ticks);
    assert(a->tick_remainder == b->tick_remainder);
    body_equal(&a->player, &b->player); body_equal(&a->enemy.body, &b->enemy.body);
    assert(a->enemy.state == b->enemy.state && a->enemy.current_node == b->enemy.current_node);
    assert(a->enemy.target_node == b->enemy.target_node && a->enemy.patrol_goal == b->enemy.patrol_goal);
    assert(a->enemy.flee_ticks == b->enemy.flee_ticks && a->enemy.wait_ticks == b->enemy.wait_ticks);
    assert(a->obstacles.count == b->obstacles.count);
    for (i = 0; i < a->obstacles.count; ++i) {
        const Obstacle *x = &a->obstacles.objects[i], *y = &b->obstacles.objects[i];
        assert(x->x == y->x && x->y == y->y && x->velocity == y->velocity);
        assert(x->previous_x == y->previous_x && x->previous_y == y->previous_y && x->active == y->active);
    }
}
static void menus(void)
{
    Game game, frozen;
    unsigned i;
    game_init(&game, 60);
    assert(game.session.state == GAME_STATE_TITLE && game.scene.title_selection == TITLE_START);
    frozen = game;
    for (i = 0; i < 300; ++i) game_update(&game, idle);
    simulation_equal(&game, &frozen);
    game_update(&game, down);
    assert(game.scene.title_selection == TITLE_INSTRUCTIONS);
    for (i = 0; i < 60; ++i) game_update(&game, down);
    assert(game.scene.title_selection == TITLE_INSTRUCTIONS); /* One move per press. */
    game_update(&game, action);
    assert(game.session.state == GAME_STATE_INSTRUCTIONS);
    for (i = 0; i < 60; ++i) game_update(&game, action);
    assert(game.session.state == GAME_STATE_INSTRUCTIONS);
    simulation_equal(&game, &frozen);
    game_update(&game, idle); game_update(&game, start);
    assert(game.session.state == GAME_STATE_TITLE);
    game_update(&game, start);
    assert(game.session.state == GAME_STATE_TITLE); /* Held START cannot confirm twice. */
    game_update(&game, idle); game_update(&game, down);
    assert(game.scene.title_selection == TITLE_CREDITS);
    game_update(&game, action);
    assert(game.session.state == GAME_STATE_CREDITS);
    game_update(&game, action);
    assert(game.session.state == GAME_STATE_CREDITS);
    game_update(&game, idle); game_update(&game, action);
    assert(game.session.state == GAME_STATE_TITLE);
    game_update(&game, down);
    assert(game.scene.title_selection == TITLE_START);
    game_update(&game, start);
    assert(game.session.state == GAME_STATE_PLAYING);
    assert(game.session.timer.ticks_remaining == 90 * 120);
    for (i = 0; i < 100; ++i) game_update(&game, start);
    assert(game.session.state == GAME_STATE_PLAYING && game.player.grounded);
    assert(game.session.timer.ticks_remaining == 90 * 120 - 200);
    game_init(&game, 60); game_update(&game, action); game_update(&game, action);
    assert(game.session.state == GAME_STATE_PLAYING && game.player.grounded);
    assert(scene_selection_step(0, -1, 3) == 2);
    assert(scene_selection_step(2, 1, 3) == 0);
    assert(scene_selection_step(99, 0, 3) == 0 && scene_selection_step(99, 1, 0) == 0);
    assert(scene_is_frontend(GAME_STATE_CREDITS) && !scene_is_frontend(GAME_STATE_GAME_OVER));
    assert(scene_can_pause(GAME_STATE_CAPTURING) && !scene_can_pause(GAME_STATE_TITLE));
}
static void pause_freezes(uint16_t hz)
{
    Game game, frozen, reference;
    const InputState jump_right = {1, 1, 0, 0}, pause_with_a = {1, 1, 0, 1};
    unsigned i;
    game_init(&game, hz); game_start_session(&game);
    game.session.invulnerability_ticks = 123;
    game_update(&game, jump_right);
    frozen = game;
    game_update(&game, pause_with_a);
    assert(game.session.state == GAME_STATE_PAUSED && game.scene.paused_state == GAME_STATE_PLAYING);
    simulation_equal(&game, &frozen);
    for (i = 0; i < hz * 10U; ++i) game_update(&game, start);
    assert(game.session.state == GAME_STATE_PAUSED);
    simulation_equal(&game, &frozen);
    game_update(&game, down);
    assert(game.scene.pause_selection == PAUSE_MAIN_MENU);
    simulation_equal(&game, &frozen);
    game_update(&game, start); /* START resumes even with MAIN MENU highlighted. */
    assert(game.session.state == GAME_STATE_PLAYING);
    simulation_equal(&game, &frozen);
    reference = frozen;
    game_update(&game, idle); game_update(&reference, idle);
    simulation_equal(&game, &reference);
}
static void pause_choices_and_results(void)
{
    Game game;
    unsigned i;
    const GameState results[] = {GAME_STATE_LEVEL_COMPLETE, GAME_STATE_ATTEMPT_FAILED, GAME_STATE_GAME_OVER, GAME_STATE_CAMPAIGN_COMPLETE};
    game_init(&game, 60); game_start_session(&game);
    game_update(&game, start); game_update(&game, idle); game_update(&game, action);
    assert(game.session.state == GAME_STATE_PLAYING);
    game_update(&game, action);
    assert(game.player.grounded); /* A to resume must not become a jump. */
    game_update(&game, idle);
    game.session.score = 1234; game.session.lives = 2; game.session.round = 7;
    game_update(&game, start); game_update(&game, down); game_update(&game, action);
    assert(game.session.state == GAME_STATE_TITLE);
    game_update(&game, action);
    assert(game.session.state == GAME_STATE_TITLE);
    game_update(&game, idle); game_update(&game, start);
    assert(game.session.state == GAME_STATE_PLAYING && game.session.score == 0);
    assert(game.session.lives == 3 && game.session.round == 1);
    for (i = 0; i < sizeof(results) / sizeof(results[0]); ++i) {
        game_init(&game, 60); game_start_session(&game);
        game.session.state = results[i];
        game_update(&game, start);
        assert(game.session.state == GAME_STATE_TITLE);
    }
}
static void capture_pause(void)
{
    Game game, frozen;
    unsigned i;
    game_init(&game, 60); game_start_session(&game);
    game.enemy.body.x = game.player.x; game.enemy.body.y = game.player.y;
    game_update(&game, idle);
    assert(game.session.state == GAME_STATE_CAPTURING);
    frozen = game;
    game_update(&game, start);
    assert(game.session.state == GAME_STATE_PAUSED && game.scene.paused_state == GAME_STATE_CAPTURING);
    for (i = 0; i < 300; ++i) game_update(&game, idle);
    simulation_equal(&game, &frozen);
    game_update(&game, start);
    assert(game.session.state == GAME_STATE_CAPTURING);
    assert(game.session.capture_ticks == frozen.session.capture_ticks);
    for (i = 0; i < 60; ++i) game_update(&game, idle);
    assert(game.session.state == GAME_STATE_LEVEL_COMPLETE);
    assert(game.session.score == frozen.session.score);
}
static Game simulate(uint16_t hz, uint16_t paused)
{
    Game game;
    unsigned i;
    game_init(&game, hz); game_start_session(&game);
    for (i = 0; i < hz * 3U; ++i) game_update(&game, idle);
    if (paused) {
        game_update(&game, start);
        for (i = 0; i < hz * 5U; ++i) game_update(&game, idle);
        game_update(&game, start);
    }
    for (i = 0; i < hz * 3U; ++i) game_update(&game, idle);
    return game;
}
int main(void)
{
    Game paused, continuous, pal, ntsc;
    menus(); pause_freezes(50); pause_freezes(60);
    pause_choices_and_results(); capture_pause();
    paused = simulate(50, 1); continuous = simulate(50, 0);
    simulation_equal(&paused, &continuous);
    pal = simulate(50, 1); ntsc = simulate(60, 1);
    simulation_equal(&pal, &ntsc);
    puts("PASS: menu/pages, held buttons, exact pause/resume, capture pause, exits, fresh session, PAL/NTSC");
    return 0;
}
