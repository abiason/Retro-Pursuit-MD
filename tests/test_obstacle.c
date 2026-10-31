#include <assert.h>
#include <stdio.h>
#include "core/game.h"
#include "test_support.h"

static const InputState idle = {0, 0, 0, 0}, action = {0, 1, 0, 0}, right = {1, 0, 0, 0};
static const ObstacleConfig cone = {OBSTACLE_STATIC, 112, 184, 16, 8, 0, 0, 0};

static void pool_and_motion(void)
{
    ObstaclePool pool;
    ObstacleConfig configs[MAX_OBSTACLES + 4];
    ObstacleConfig moving = {OBSTACLE_HORIZONTAL, 100, 100, 16, 8, 100, 102, 192};
    unsigned i;
    for (i = 0; i < MAX_OBSTACLES + 4; ++i) configs[i] = cone;
    assert(obstacles_init(&pool, configs, MAX_OBSTACLES + 4) == MAX_OBSTACLES);
    assert(obstacles_init(&pool, 0, 8) == 0);
    for (i = 0; i < MAX_OBSTACLES; ++i) assert(!pool.objects[i].active);
    configs[0] = cone; configs[0].width = 0;
    configs[1] = moving; configs[1].maximum = 99;
    configs[2] = cone; configs[2].x = 319;
    configs[3] = moving; configs[3].speed = 257;
    assert(obstacles_init(&pool, configs, 4) == 0);
    assert(obstacles_init(&pool, &moving, 1) == 1);
    obstacles_step(&pool); obstacles_step(&pool); obstacles_step(&pool);
    assert(pool.objects[0].x == 102 * POSITION_ONE && pool.objects[0].velocity == -192);
    for (i = 0; i < 1000; ++i) {
        obstacles_step(&pool);
        assert(pool.objects[0].x >= 100 * POSITION_ONE && pool.objects[0].x <= 102 * POSITION_ONE);
    }
    moving.kind = OBSTACLE_VERTICAL; moving.width = 8;
    moving.y = moving.minimum = 80; moving.maximum = 88;
    obstacles_init(&pool, &moving, 1);
    for (i = 0; i < 1000; ++i) {
        obstacles_step(&pool);
        assert(pool.objects[0].x == 100 * POSITION_ONE);
        assert(pool.objects[0].y >= 80 * POSITION_ONE && pool.objects[0].y <= 88 * POSITION_ONE);
    }
    moving.maximum = moving.minimum;
    obstacles_init(&pool, &moving, 1); obstacles_step(&pool);
    assert(pool.objects[0].y == 80 * POSITION_ONE);
}
static void contacts(void)
{
    ObstaclePool pool;
    Player player;
    ObstacleConfig thin = cone;
    const ObstacleConfig cart = {OBSTACLE_HORIZONTAL, 96, 184, 16, 8, 96, 100, 128};
    player_init(&player, &level_test);
    obstacles_init(&pool, &cone, 1);
    player.x = 96 * POSITION_ONE; player.y = 168 * POSITION_ONE;
    assert(!obstacles_hit(&pool, &player, player.x, player.y));
    player.x = 104 * POSITION_ONE;
    assert(obstacles_hit(&pool, &player, player.x, player.y));
    player.y = 152 * POSITION_ONE;
    assert(!obstacles_hit(&pool, &player, player.x, player.y));
    /* Crossing a thin hitbox with separated start/end rectangles still hits. */
    thin.width = 1;
    thin.x = 104; /* Centered inside the 16px visual, including a half-pixel inset. */
    obstacles_init(&pool, &thin, 1);
    player.x = 114 * POSITION_ONE; player.y = 168 * POSITION_ONE;
    assert(obstacles_hit(&pool, &player, 95 * POSITION_ONE, player.y));
    /* Exact corner grazing has no interval of overlap. */
    obstacles_init(&pool, &cone, 1);
    player.x = 97 * POSITION_ONE; player.y = 159 * POSITION_ONE;
    assert(!obstacles_hit(&pool, &player, 95 * POSITION_ONE, 161 * POSITION_ONE));
    obstacles_init(&pool, &cart, 1);
    player.x = 112 * POSITION_ONE; player.y = 168 * POSITION_ONE;
    obstacles_step(&pool);
    assert(obstacles_hit(&pool, &player, player.x, player.y));
    pool.objects[0].active = 0;
    assert(!obstacles_hit(&pool, &player, player.x, player.y));
}
static void shield_boundary(void)
{
    GameSession session;
    GameRules rules = game_default_rules;
    unsigned i;
    rules.invulnerability_duration = 5;
    session_init(&session, &rules);
    assert(session_damage(&session, &rules) && session.lives == 2);
    for (i = 0; i < 4; ++i) {
        session_tick(&session);
        assert(!session_damage(&session, &rules));
    }
    session_tick(&session);
    assert(!session.invulnerability_ticks);
    assert(session_damage(&session, &rules) && session.lives == 1);
    for (i = 0; i < 5; ++i) session_tick(&session);
    assert(session_damage(&session, &rules) && !session.lives);
    assert(session.state == GAME_STATE_GAME_OVER);
    assert(!session_damage(&session, &rules));
}
static void place_on_cone(Game *game)
{
    game->player.x = 112 * POSITION_ONE; game->player.y = 72 * POSITION_ONE;
    game->player.vx = game->player.vy = 0;
    game->player.grounded = 1;
}
static void game_damage(void)
{
    Game game;
    uint32_t timer;
    int32_t obstacle_x;
    unsigned i;
    test_start_game(&game, 60);
    game.enemy.active = 0;
    game.session.score = 99;
    game.session.timer.ticks_remaining = 1200;
    game.previous_jump = 1;
    place_on_cone(&game);
    game_update(&game, action);
    assert(game.session.state == GAME_STATE_PLAYING && game.session.lives == 2);
    assert(game.session.score == 99 && game.session.round == 1);
    assert(game.session.timer.ticks_remaining == 1198);
    assert(game.session.invulnerability_ticks == 239);
    assert(game.player.x == (int32_t)level_test.spawn_x * POSITION_ONE && game.player.grounded);
    game_update(&game, action);
    assert(game.player.grounded); /* Held A cannot auto-jump on respawn. */
    place_on_cone(&game);
    game_update(&game, idle);
    assert(game.session.lives == 2); /* Repeated contact is ignored under the shield. */
    test_start_game(&game, 60);
    place_on_cone(&game);
    game.enemy.body = game.player;
    game_update(&game, idle);
    assert(game.session.lives == 2 && game.session.score == 0);
    assert(game.session.state == GAME_STATE_PLAYING); /* Damage before capture. */
    place_on_cone(&game);
    game.enemy.body = game.player;
    game_update(&game, idle);
    assert(game.session.state == GAME_STATE_CAPTURING); /* Protected capture is valid. */
    obstacle_x = game.obstacles.objects[1].x;
    for (i = 0; i < 20; ++i) game_update(&game, idle);
    assert(game.obstacles.objects[1].x == obstacle_x);
    test_start_game(&game, 60);
    place_on_cone(&game); game.enemy.active = 0;
    game.session.timer.ticks_remaining = 1;
    game_update(&game, idle);
    assert(game.session.state == GAME_STATE_ATTEMPT_FAILED && game.session.lives == 2);
    assert(!game.session.invulnerability_ticks); /* Timeout before damage. */
    test_start_game(&game, 60);
    place_on_cone(&game); game.enemy.active = 0; game.session.lives = 1;
    game_update(&game, idle);
    assert(game.session.state == GAME_STATE_GAME_OVER && !game.session.lives);
    timer = game.session.timer.ticks_remaining;
    obstacle_x = game.obstacles.objects[1].x;
    for (i = 0; i < 60; ++i) game_update(&game, idle);
    assert(game.session.timer.ticks_remaining == timer && game.obstacles.objects[1].x == obstacle_x);
    game_update(&game, action);
    assert(game.session.state == GAME_STATE_PLAYING && game.session.lives == 3);
    assert(!game.session.invulnerability_ticks);
    assert(game.obstacles.objects[1].x == 168 * POSITION_ONE);
}
static void jump_clearance(void)
{
    Game game;
    const InputState jump_right = {1, 1, 0, 0};
    unsigned i;
    test_start_game(&game, 60);
    game.enemy.active = 0;
    obstacles_init(&game.obstacles, obstacles_test_config, 1);
    game.player.x = 88 * POSITION_ONE;
    game.player.y = 72 * POSITION_ONE;
    for (i = 0; i < 7; ++i) game_update(&game, right);
    game_update(&game, jump_right);
    for (i = 0; i < 40; ++i) game_update(&game, right);
    assert(game.player.x >= 128 * POSITION_ONE && game.session.lives == 3);
    test_start_game(&game, 60);
    game.enemy.active = 0;
    obstacles_init(&game.obstacles, &obstacles_test_config[1], 1);
    game.obstacles.objects[0].x = 180 * POSITION_ONE;
    game.obstacles.objects[0].velocity = -128;
    game.player.x = 160 * POSITION_ONE; game.player.y = 72 * POSITION_ONE;
    game_update(&game, jump_right);
    for (i = 0; i < 20; ++i) game_update(&game, right);
    assert(game.player.x >= 184 * POSITION_ONE && game.session.lives == 3);
    test_start_game(&game, 60);
    game.enemy.active = 0;
    obstacles_init(&game.obstacles, &obstacles_test_config[2], 1);
    game.obstacles.objects[0].y = 88 * POSITION_ONE;
    game.obstacles.objects[0].velocity = -32;
    game.player.x = 247 * POSITION_ONE; game.player.y = 72 * POSITION_ONE;
    game.player.vx = POSITION_ONE;
    game_update(&game, jump_right);
    for (i = 0; i < 18; ++i) game_update(&game, right);
    assert(game.player.x >= 272 * POSITION_ONE && game.session.lives == 3);
}
static Game simulate(uint16_t hz)
{
    Game game;
    unsigned i;
    test_start_game(&game, hz);
    game.enemy.active = 0;
    place_on_cone(&game);
    for (i = 0; i < hz * 20U; ++i) game_update(&game, idle);
    return game;
}
int main(void)
{
    Game pal, ntsc;
    unsigned i;
    pool_and_motion(); contacts(); shield_boundary(); game_damage(); jump_clearance();
    pal = simulate(50); ntsc = simulate(60);
    assert(pal.session.lives == 2 && ntsc.session.lives == 2);
    assert(pal.session.invulnerability_ticks == ntsc.session.invulnerability_ticks);
    assert(pal.session.timer.ticks_remaining == ntsc.session.timer.ticks_remaining);
    for (i = 0; i < pal.obstacles.count; ++i) {
        assert(pal.obstacles.objects[i].x == ntsc.obstacles.objects[i].x);
        assert(pal.obstacles.objects[i].y == ntsc.obstacles.objects[i].y);
    }
    puts("PASS: pool/limits, motion, swept hits, jump clearance, damage/shield, priorities, restart, PAL/NTSC");
    return 0;
}
