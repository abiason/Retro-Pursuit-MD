#include "enemy.h"
#include "systems/physics.h"
#include "systems/animation.h"
#include "systems/collision.h"
const EnemyConfig enemy_test_config = {9, 192, 96, 60};
static int32_t magnitude(int32_t value) { return value < 0 ? -value : value; }
static int8_t direction(int32_t value) { return (value > 0) - (value < 0); }
static int16_t escape_goal(const Enemy *e, const Player *player)
{
    int16_t best = e->current_node;
    int32_t best_score = -0x7FFFFFFF;
    uint16_t i;
    for (i = 0; i < e->graph->node_count; ++i) {
        int16_t hop = navigation_next_hop(e->graph, (uint16_t)e->current_node, i);
        const NavigationNode *node = &e->graph->nodes[i];
        int32_t score;
        int8_t first_direction;
        if (hop < 0) continue;
        score = magnitude((int32_t)node->x - player_pixel_x(player)) +
                magnitude((int32_t)node->feet_y - player_pixel_y(player) - PLAYER_HEIGHT) * 4;
        first_direction = direction((int32_t)e->graph->nodes[hop].x * POSITION_ONE - e->body.x);
        /* Prefer a vertical escape over an initial walk toward a nearby player. */
        if (first_direction && first_direction != direction(e->body.x - player->x) &&
            magnitude(e->body.y - player->y) < PLAYER_HEIGHT * POSITION_ONE) score -= 512;
        if (e->graph->nodes[hop].feet_y != e->graph->nodes[e->current_node].feet_y &&
            e->body.y != player->y &&
            direction((int32_t)e->graph->nodes[hop].feet_y * POSITION_ONE -
                e->body.y - PLAYER_HEIGHT * POSITION_ONE) != direction(e->body.y - player->y))
            score -= 512;
        if (score > best_score) { best = (int16_t)i; best_score = score; }
    }
    return best;
}
void enemy_init(Enemy *e, const LevelConfig *level, const NavigationGraph *graph,
                const EnemyConfig *config)
{
    player_init(&e->body, level);
    e->graph = graph;
    e->state = ENEMY_IDLE;
    e->active = 0;
    e->wait_ticks = e->flee_ticks = 0;
    e->current_node = e->target_node = e->patrol_goal = -1;
    if (!config || !graph || config->spawn_node >= graph->node_count ||
        navigation_nearest(graph, 0, 0) < 0) return;
    e->config = *config;
    if (!e->config.speed || e->config.speed > POSITION_ONE) e->config.speed = 192;
    e->current_node = (int16_t)config->spawn_node;
    e->patrol_goal = config->spawn_node ? 0 : (int16_t)graph->node_count - 1;
    e->body.x = (int32_t)graph->nodes[config->spawn_node].x * POSITION_ONE;
    e->body.y = (graph->nodes[config->spawn_node].feet_y - PLAYER_HEIGHT) * POSITION_ONE;
    e->active = 1;
}
static uint16_t needs_jump(const Enemy *e, const LevelConfig *level, int8_t horizontal)
{
    WorldRect probe;
    uint16_t i;
    int16_t feet = player_pixel_y(&e->body) + PLAYER_HEIGHT;
    if (!e->body.grounded || !horizontal) return 0;
    probe.x = player_pixel_x(&e->body) + (horizontal > 0 ? PLAYER_WIDTH : -6);
    probe.y = feet - 8; probe.width = 6; probe.height = 8;
    for (i = 0; i < level->solid_count; ++i) {
        const WorldRect *r = &level->solids[i];
        if (r->y < feet && r->y >= feet - 16 && collision_aabb(probe, *r)) return 1;
    }
    return 0;
}
static void step(Enemy *e, const LevelConfig *level, int8_t horizontal, int8_t vertical)
{
    if (horizontal) e->body.facing = horizontal;
    physics_step_with_speed(&e->body, level, horizontal, vertical,
                            needs_jump(e, level, horizontal), (int16_t)e->config.speed);
    animation_update(&e->body);
}
void enemy_update(Enemy *e, const LevelConfig *level, const Player *player)
{
    const NavigationNode *target;
    const NavigationEdge *edge;
    int8_t horizontal = 0, vertical = 0;
    uint16_t was_fleeing;
    int16_t goal;
    int32_t distance;
    if (!e->active) return;
    was_fleeing = e->flee_ticks != 0;
    distance = magnitude(e->body.x - player->x) + magnitude(e->body.y - player->y) * 2;
    if (distance <= (int32_t)e->config.detection_radius * POSITION_ONE) e->flee_ticks = 240;
    else if (e->flee_ticks) --e->flee_ticks;
    if (e->flee_ticks && !was_fleeing && e->body.grounded && e->body.ladder_index < 0) {
        e->current_node = navigation_nearest(e->graph, player_pixel_x(&e->body),
                                            player_pixel_y(&e->body) + PLAYER_HEIGHT);
        e->target_node = -1;
        e->wait_ticks = 0;
    }
    if (e->target_node >= 0) {
        target = &e->graph->nodes[e->target_node];
        if (e->body.grounded && e->body.ladder_index < 0 &&
            magnitude(e->body.x - (int32_t)target->x * POSITION_ONE) <= POSITION_ONE &&
            e->body.y == (target->feet_y - PLAYER_HEIGHT) * POSITION_ONE) {
            e->body.x = (int32_t)target->x * POSITION_ONE;
            e->body.vx = 0;
            e->current_node = e->target_node;
            e->target_node = -1;
            if (!e->flee_ticks && e->current_node == e->patrol_goal) {
                e->patrol_goal = e->patrol_goal ? 0 : (int16_t)e->graph->node_count - 1;
                e->wait_ticks = e->config.idle_ticks;
            }
        }
    }
    if (e->wait_ticks) {
        --e->wait_ticks;
        e->state = ENEMY_IDLE;
        step(e, level, 0, 0);
        return;
    }
    if (e->target_node < 0) {
        goal = e->flee_ticks ? escape_goal(e, player) : e->patrol_goal;
        e->target_node = navigation_next_hop(e->graph, (uint16_t)e->current_node, (uint16_t)goal);
        if (e->target_node < 0 || e->target_node == e->current_node) {
            e->target_node = -1;
            e->state = ENEMY_IDLE;
            e->wait_ticks = 12; /* Bound route-search frequency while holding a safe endpoint. */
            step(e, level, 0, 0);
            return;
        }
    }
    target = &e->graph->nodes[e->target_node];
    edge = navigation_edge(e->graph, (uint16_t)e->current_node, (uint16_t)e->target_node);
    if (!edge) { e->state = ENEMY_IDLE; step(e, level, 0, 0); return; }
    if (edge->ladder_index >= 0) {
        if ((uint16_t)edge->ladder_index >= level->ladder_count) {
            e->state = ENEMY_IDLE; step(e, level, 0, 0); return;
        }
        if (e->body.ladder_index < 0 &&
            magnitude(e->body.x - (int32_t)target->x * POSITION_ONE) > POSITION_ONE) {
            horizontal = direction((int32_t)target->x * POSITION_ONE - e->body.x);
            e->state = e->flee_ticks ? ENEMY_ESCAPE : ENEMY_PATROL;
        } else {
            e->body.x = (int32_t)target->x * POSITION_ONE;
            vertical = direction((target->feet_y - PLAYER_HEIGHT) * POSITION_ONE - e->body.y);
            e->state = vertical < 0 ? ENEMY_CLIMB : ENEMY_DESCEND;
        }
    } else {
        horizontal = direction((int32_t)target->x * POSITION_ONE - e->body.x);
        e->state = e->flee_ticks ? ENEMY_ESCAPE : ENEMY_PATROL;
    }
    step(e, level, horizontal, vertical);
}
