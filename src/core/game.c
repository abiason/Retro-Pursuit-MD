#include "game.h"
#include "systems/collision.h"
#include "systems/animation.h"

static void reset_actors(Game *game)
{
    player_init(&game->player, game->level);
    game->wall_contact = 0;
    enemy_init(&game->enemy, game->level, game->stage->navigation, game->stage->enemy);
    obstacles_init(&game->obstacles, game->stage->obstacles, game->stage->obstacle_count);
}
uint16_t game_begin_stage(Game *game, uint16_t index)
{
    const StageConfig *stage = stage_get(index);
    if (!stage) return 0;
    game->stage = stage;
    game->stage_index = index;
    game->level = stage->level;
    if (!game->custom_rules) game->rules.time_limit = stage->time_limit;
    game->events |= GAME_EVENT_STAGE_START;
    game->session.round = stage->id;
    session_start_attempt(&game->session, &game->rules);
    reset_actors(game);
    game->tick_remainder = 0;
    return 1;
}
void game_init_with_rules(Game *game, uint16_t refresh_rate, const GameRules *rules)
{
    game->events = 0;
    game->custom_rules = rules != 0;
    game->rules = rules ? *rules : game_default_rules;
    if (!game->rules.time_limit) game->rules.time_limit = 1;
    if (!game->rules.initial_lives) game->rules.initial_lives = 1;
    if (!game->rules.capture_duration) game->rules.capture_duration = 1;
    if (!game->rules.invulnerability_duration) game->rules.invulnerability_duration = 1;
    session_init(&game->session, &game->rules);
    game_begin_stage(game, 0);
    game->previous_jump = 0;
    game->previous_start = 0;
    game->previous_vertical = 0;
    scene_init(&game->scene);
    game->session.state = GAME_STATE_TITLE;
    game->events = 0;
    game->refresh_rate = refresh_rate == 50 ? 50 : 60;
}
void game_init(Game *game, uint16_t refresh_rate)
{
    game_init_with_rules(game, refresh_rate, 0);
}
void game_start_session(Game *game)
{
    session_init(&game->session, &game->rules);
    game_begin_stage(game, 0);
    game->scene.paused_state = GAME_STATE_PLAYING;
    game->scene.pause_selection = PAUSE_CONTINUE;
}
void game_show_title(Game *game)
{
    game->session.state = GAME_STATE_TITLE;
    game->tick_remainder = 0;
    scene_init(&game->scene);
}
static void continue_session(Game *game)
{
    if (game->session.state == GAME_STATE_GAME_OVER || game->session.state == GAME_STATE_CAMPAIGN_COMPLETE)
        game_start_session(game);
    else if (game->session.state == GAME_STATE_LEVEL_COMPLETE) {
        if (!game_begin_stage(game, game->stage_index + 1))
            game->session.state = GAME_STATE_CAMPAIGN_COMPLETE;
    } else game_begin_stage(game, game->stage_index);
}
static void game_tick(Game *game, InputState input, uint16_t jump)
{
    int32_t previous_x, previous_y;
    uint16_t grounded, climbing, contact;
    const GameState previous_state = game->session.state;
    /* Deadline wins a simultaneous capture: no capture is valid once time reaches zero. */
    session_tick(&game->session);
    if (game->session.state == GAME_STATE_LEVEL_COMPLETE && game->stage_index + 1 >= stage_count())
        game->session.state = GAME_STATE_CAMPAIGN_COMPLETE;
    if (previous_state == GAME_STATE_CAPTURING && game->session.state == GAME_STATE_LEVEL_COMPLETE)
        game->events |= GAME_EVENT_STAGE_CLEAR;
    if (previous_state == GAME_STATE_CAPTURING && game->session.state == GAME_STATE_CAMPAIGN_COMPLETE)
        game->events |= GAME_EVENT_CAMPAIGN_CLEAR;
    if (previous_state == GAME_STATE_PLAYING && game->session.state == GAME_STATE_ATTEMPT_FAILED)
        game->events |= GAME_EVENT_TIMEOUT;
    if (previous_state == GAME_STATE_PLAYING && game->session.state == GAME_STATE_GAME_OVER)
        game->events |= GAME_EVENT_GAME_OVER;
    if (game->session.state == GAME_STATE_CAPTURING) {
        game->player.frame = PLAYER_FRAME_CAPTURE;
        game->enemy.body.frame = PLAYER_FRAME_CAPTURE;
    }
    if (game->session.state != GAME_STATE_PLAYING) return;
    previous_x = game->player.x; previous_y = game->player.y;
    grounded = game->player.grounded; climbing = game->player.ladder_index >= 0;
    obstacles_step(&game->obstacles);
    player_update(&game->player, game->level, input.horizontal, input.vertical, jump);
    if (jump && grounded && !climbing && !game->player.grounded && game->player.vy < 0)
        game->events |= GAME_EVENT_JUMP;
    contact = input.horizontal != 0 && game->player.x == previous_x && !game->player.vx &&
              game->player.ladder_index < 0;
    if ((contact && !game->wall_contact) || (!grounded && !climbing && game->player.grounded))
        game->events |= GAME_EVENT_COLLISION;
    game->wall_contact = contact;
    enemy_update(&game->enemy, game->level, &game->player);
    if (obstacles_hit(&game->obstacles, &game->player, previous_x, previous_y) &&
        session_damage(&game->session, &game->rules)) {
        game->events |= GAME_EVENT_DAMAGE;
        if (game->session.state == GAME_STATE_PLAYING) {
            player_init(&game->player, game->level);
            game->wall_contact = 0;
        }
        else { game->player.vx = game->player.vy = 0; game->events |= GAME_EVENT_GAME_OVER; }
        return; /* Unprotected damage wins a simultaneous capture. */
    }
    if (game->enemy.active && collision_bodies_overlap(&game->player, &game->enemy.body)) {
        session_capture(&game->session, &game->rules);
        game->events |= GAME_EVENT_CAPTURE;
        game->player.frame = game->enemy.body.frame = PLAYER_FRAME_CAPTURE;
        game->player.vx = game->player.vy = 0;
        game->enemy.body.vx = game->enemy.body.vy = 0;
    }
}
void game_update(Game *game, InputState input)
{
    uint16_t action = input.jump_held && !game->previous_jump;
    const uint16_t start = input.start_held && !game->previous_start;
    const int8_t vertical = (input.vertical > 0) - (input.vertical < 0);
    const int8_t navigate = vertical != game->previous_vertical ? vertical : 0;
    game->events = 0;
    game->previous_jump = input.jump_held != 0;
    game->previous_start = input.start_held != 0;
    game->previous_vertical = vertical;
    if (game->session.state == GAME_STATE_TITLE) {
        if (navigate) game->events |= GAME_EVENT_MENU_MOVE;
        game->scene.title_selection = scene_selection_step(game->scene.title_selection, navigate, TITLE_OPTION_COUNT);
        if (action || start) {
            game->events |= GAME_EVENT_MENU_CONFIRM;
            if (game->scene.title_selection == TITLE_START) game_start_session(game);
            else game->session.state = game->scene.title_selection == TITLE_INSTRUCTIONS ?
                GAME_STATE_INSTRUCTIONS : GAME_STATE_CREDITS;
        }
        return;
    }
    if (game->session.state == GAME_STATE_INSTRUCTIONS || game->session.state == GAME_STATE_CREDITS) {
        if (action || start) { game->session.state = GAME_STATE_TITLE; game->events |= GAME_EVENT_MENU_CONFIRM; }
        return;
    }
    if (game->session.state == GAME_STATE_PAUSED) {
        if (start) { game->session.state = game->scene.paused_state; return; }
        game->scene.pause_selection = scene_selection_step(game->scene.pause_selection, navigate, PAUSE_OPTION_COUNT);
        if (action) {
            if (game->scene.pause_selection == PAUSE_CONTINUE) game->session.state = game->scene.paused_state;
            else game_show_title(game);
        }
        return;
    }
    if (start && scene_can_pause(game->session.state)) {
        game->scene.paused_state = game->session.state;
        game->scene.pause_selection = PAUSE_CONTINUE;
        game->session.state = GAME_STATE_PAUSED;
        return;
    }
    if (game->session.state == GAME_STATE_LEVEL_COMPLETE ||
        game->session.state == GAME_STATE_ATTEMPT_FAILED || game->session.state == GAME_STATE_GAME_OVER ||
        game->session.state == GAME_STATE_CAMPAIGN_COMPLETE) {
        if (start) { game_show_title(game); game->events |= GAME_EVENT_MENU_CONFIRM; }
        else if (action) continue_session(game);
        return;
    }
    game->tick_remainder += PLAYER_TICK_HZ;
    while (game->tick_remainder >= game->refresh_rate) {
        game->tick_remainder -= game->refresh_rate;
        game_tick(game, input, action);
        action = 0;
    }
}
