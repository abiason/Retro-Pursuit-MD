#include <assert.h>
#include <stdio.h>
#include "core/game.h"
#include "systems/collision.h"
#include "systems/physics.h"

static const InputState idle = {0, 0, 0, 0}, action = {0, 1, 0, 0}, start = {0, 0, 0, 1};

static void validate_content(const StageConfig *stage)
{
    const NavigationGraph *graph = stage->navigation;
    ObstaclePool obstacles;
    Player body;
    unsigned i, j;
    assert(stage->id && stage->time_limit && stage->difficulty);
    assert(graph->node_count <= NAVIGATION_MAX_NODES && stage->obstacle_count <= MAX_OBSTACLES);
    assert(stage->enemy->spawn_node < graph->node_count && stage->enemy->speed <= POSITION_ONE);
    assert(obstacles_init(&obstacles, stage->obstacles, stage->obstacle_count) == stage->obstacle_count);
    for (i = 0; i < graph->node_count; ++i) {
        player_init(&body, stage->level);
        body.x = (int32_t)graph->nodes[i].x * POSITION_ONE;
        body.y = (graph->nodes[i].feet_y - PLAYER_HEIGHT) * POSITION_ONE;
        assert(!collision_player_overlaps(&body, stage->level));
        assert(!obstacles_hit(&obstacles, &body, body.x, body.y));
        physics_step(&body, stage->level, 0, 0, 0);
        assert(body.grounded && !collision_player_overlaps(&body, stage->level));
        for (j = 0; j < graph->node_count; ++j)
            assert(navigation_next_hop(graph, (uint16_t)i, (uint16_t)j) >= 0);
    }
    for (i = 0; i < graph->edge_count; ++i) {
        const NavigationEdge *edge = &graph->edges[i];
        const NavigationNode *a, *b;
        assert(edge->from < graph->node_count && edge->to < graph->node_count);
        a = &graph->nodes[edge->from]; b = &graph->nodes[edge->to];
        if (edge->ladder_index >= 0) {
            const Ladder *ladder;
            assert((uint16_t)edge->ladder_index < stage->level->ladder_count);
            ladder = &stage->level->ladders[edge->ladder_index];
            assert(a->x == ladder->x && b->x == ladder->x);
            assert((a->feet_y == ladder->top && b->feet_y == ladder->bottom) ||
                   (b->feet_y == ladder->top && a->feet_y == ladder->bottom));
        } else assert(a->feet_y == b->feet_y);
    }
}
static void route(const StageConfig *stage, uint16_t speed)
{
    Enemy actor;
    EnemyConfig config = *stage->enemy;
    Player observer;
    unsigned i, floors = 0, endpoints = 0;
    config.speed = speed;
    enemy_init(&actor, stage->level, stage->navigation, &config);
    player_init(&observer, stage->level); observer.x = -1000 * POSITION_ONE;
    for (i = 0; i < 120 * 40; ++i) {
        enemy_update(&actor, stage->level, &observer);
        if (actor.body.ladder_index < 0) assert(!collision_player_overlaps(&actor.body, stage->level));
        assert(actor.body.x >= 16 * POSITION_ONE && actor.body.x <= 288 * POSITION_ONE);
        if (actor.body.grounded) {
            int16_t feet = player_pixel_y(&actor.body) + PLAYER_HEIGHT;
            if (feet == 192) floors |= 1;
            if (feet == 144) floors |= 2;
            if (feet == 96) floors |= 4;
        }
        if (actor.current_node == 0) endpoints |= 1;
        if (actor.current_node == (int16_t)stage->navigation->node_count - 1 && i > 1000) endpoints |= 2;
    }
    assert(floors == 7 && endpoints == 3);
}
static void campaign(uint16_t hz)
{
    Game game;
    uint32_t score = 0;
    unsigned index, wait;
    game_init(&game, hz); game_start_session(&game);
    game.session.lives = 2;
    for (index = 0; index < stage_count(); ++index) {
        const StageConfig *stage = stage_get((uint16_t)index);
        assert(game.stage_index == index && game.stage == stage && game.level == stage->level);
        assert(game.session.round == stage->id && game.enemy.graph == stage->navigation);
        assert(game.rules.time_limit == stage->time_limit && game.session.lives == 2);
        assert(game.obstacles.count == stage->obstacle_count);
        game.enemy.body.x = game.player.x; game.enemy.body.y = game.player.y;
        game_update(&game, idle);
        assert(game.session.state == GAME_STATE_CAPTURING);
        score += 1500 + 10U * stage->time_limit;
        assert(game.session.score == score);
        for (wait = 0; wait < hz && game.session.state == GAME_STATE_CAPTURING; ++wait)
            game_update(&game, idle);
        assert(game.session.state == (index + 1 == stage_count() ?
            GAME_STATE_CAMPAIGN_COMPLETE : GAME_STATE_LEVEL_COMPLETE));
        if (index + 1 < stage_count()) game_update(&game, action);
    }
    assert(game.stage_index + 1 == stage_count() && game.session.score == 6900);
    game_update(&game, action);
    assert(game.session.state == GAME_STATE_PLAYING && !game.stage_index);
    assert(game.session.lives == 3 && !game.session.score && game.session.round == 1);
    assert(game.rules.time_limit == 90 && game.session.timer.ticks_remaining == 90 * 120);
}
static void failure_bounds_and_override(void)
{
    Game game;
    const StageConfig *before;
    GameRules rules = game_default_rules;
    uint32_t timer;
    game_init(&game, 60); game_start_session(&game);
    assert(game_begin_stage(&game, 1));
    game.session.score = 777; game.session.lives = 2;
    before = game.stage; timer = game.session.timer.ticks_remaining;
    assert(!game_begin_stage(&game, stage_count()) && !game_begin_stage(&game, 65535));
    assert(game.stage == before && game.session.score == 777 && game.session.timer.ticks_remaining == timer);
    game.enemy.active = 0; game.session.timer.ticks_remaining = 1;
    game_update(&game, idle); game_update(&game, action);
    assert(game.stage_index == 1 && game.session.round == 2 && game.session.lives == 1);
    assert(game.session.score == 777 && game.session.timer.ticks_remaining == 80 * 120);
    assert(game.obstacles.count == before->obstacle_count && game.enemy.graph == before->navigation);
    game.enemy.active = 0; game.session.timer.ticks_remaining = 1;
    game_update(&game, idle);
    assert(game.session.state == GAME_STATE_GAME_OVER);
    game_update(&game, action);
    assert(!game.stage_index && game.session.score == 0 && game.session.lives == 3);
    game.session.state = GAME_STATE_CAMPAIGN_COMPLETE;
    game_update(&game, start);
    assert(game.session.state == GAME_STATE_TITLE);
    rules.time_limit = 2;
    game_init_with_rules(&game, 60, &rules); game_start_session(&game);
    assert(game_begin_stage(&game, 2));
    assert(game.session.timer.ticks_remaining == 240 && game.rules.time_limit == 2);
}
static Game simulate(uint16_t hz, uint16_t index)
{
    Game game;
    unsigned i;
    game_init(&game, hz); game_start_session(&game); game_begin_stage(&game, index);
    for (i = 0; i < hz * 20U; ++i) game_update(&game, idle);
    return game;
}
static void hazard_clearance(uint16_t index)
{
    const StageConfig *stage = stage_get(index);
    const InputState jump_right = {1, 1, 0, 0}, right = {1, 0, 0, 0};
    unsigned i, frame;
    for (i = 0; i < stage->obstacle_count; ++i) {
        Game game;
        const ObstacleConfig *config = &stage->obstacles[i];
        Obstacle *object;
        int16_t width = config->kind == OBSTACLE_VERTICAL ? 8 : 16;
        int32_t hit_left;
        game_init(&game, 60); game_start_session(&game); game_begin_stage(&game, index);
        game.enemy.active = 0;
        obstacles_init(&game.obstacles, config, 1);
        object = &game.obstacles.objects[0];
        if (config->kind == OBSTACLE_HORIZONTAL) {
            object->x = (int32_t)config->maximum * POSITION_ONE;
            object->velocity = -config->speed; /* A reachable approaching phase. */
        } else if (config->kind == OBSTACLE_VERTICAL) {
            object->y = (config->maximum - 4) * POSITION_ONE;
            object->velocity = config->speed; /* Low and descending before its turnaround. */
        }
        hit_left = object->x + (width - config->width) * POSITION_ONE / 2;
        game.player.x = hit_left - (PLAYER_WIDTH + 3) * POSITION_ONE;
        game.player.y = 72 * POSITION_ONE;
        game.player.vx = POSITION_ONE;
        game_update(&game, jump_right);
        for (frame = 0; frame < 15; ++frame) game_update(&game, right);
        assert(game.session.lives == 3 && game.session.state == GAME_STATE_PLAYING);
        assert(game.player.x >= hit_left + config->width * POSITION_ONE);
    }
}
int main(void)
{
    unsigned i, j;
    assert(stage_count() == 3 && stage_count() <= MAX_LEVELS);
    assert(!stage_get(stage_count()) && !stage_get(65535));
    assert(stage_get(0)->level == &level_test && stage_get(0)->navigation == &navigation_test);
    assert(stage_get(0)->enemy == &enemy_test_config && stage_get(0)->obstacle_count == obstacles_test_count);
    for (i = 0; i < stage_count(); ++i) {
        const StageConfig *stage = stage_get((uint16_t)i);
        Game pal, ntsc;
        validate_content(stage);
        route(stage, stage->enemy->speed); route(stage, POSITION_ONE);
        hazard_clearance((uint16_t)i);
        if (i) {
            const StageConfig *previous = stage_get((uint16_t)i - 1);
            assert(stage->time_limit < previous->time_limit && stage->enemy->speed > previous->enemy->speed);
            assert(stage->obstacle_count > previous->obstacle_count && stage->difficulty > previous->difficulty);
        }
        pal = simulate(50, (uint16_t)i); ntsc = simulate(60, (uint16_t)i);
        assert(pal.session.state == ntsc.session.state && pal.session.score == ntsc.session.score);
        assert(pal.session.timer.ticks_remaining == ntsc.session.timer.ticks_remaining);
        assert(pal.enemy.body.x == ntsc.enemy.body.x && pal.enemy.body.y == ntsc.enemy.body.y);
        for (j = 0; j < stage->obstacle_count; ++j) {
            assert(pal.obstacles.objects[j].x == ntsc.obstacles.objects[j].x);
            assert(pal.obstacles.objects[j].y == ntsc.obstacles.objects[j].y);
        }
    }
    campaign(50); campaign(60); failure_bounds_and_override();
    puts("PASS: three maps/graphs, 40s routes at enemy/player speed, progression, retry, campaign win, limits, PAL/NTSC");
    return 0;
}
