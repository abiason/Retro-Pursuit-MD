#include "obstacle.h"
const ObstacleConfig obstacles_test_config[] = {
    {OBSTACLE_STATIC, 112, 88, 6, 4, 0, 0, 0},
    {OBSTACLE_HORIZONTAL, 168, 88, 6, 4, 168, 216, 128},
    {OBSTACLE_VERTICAL, 264, 56, 6, 6, 56, 88, 32}
};
const uint16_t obstacles_test_count = sizeof(obstacles_test_config) / sizeof(obstacles_test_config[0]);
static uint16_t valid(const ObstacleConfig *c)
{
    const int16_t visual_width = c->kind == OBSTACLE_VERTICAL ? 8 : 16;
    int16_t position;
    if (c->kind != OBSTACLE_STATIC && c->kind != OBSTACLE_HORIZONTAL &&
        c->kind != OBSTACLE_VERTICAL) return 0;
    if (c->width <= 0 || c->width > visual_width || c->height <= 0 || c->height > 8 ||
        c->x < 0 || c->y < 0 || c->x + visual_width > 320 || c->y + 8 > 224) return 0;
    if (c->kind == OBSTACLE_STATIC) return 1;
    position = c->kind == OBSTACLE_HORIZONTAL ? c->x : c->y;
    if (c->minimum < 0 || c->maximum < c->minimum || position < c->minimum ||
        position > c->maximum || c->speed <= 0 || c->speed > POSITION_ONE) return 0;
    return c->kind == OBSTACLE_HORIZONTAL ? c->maximum + visual_width <= 320 : c->maximum + 8 <= 224;
}
uint16_t obstacles_init(ObstaclePool *pool, const ObstacleConfig *config, uint16_t count)
{
    uint16_t i;
    pool->count = 0;
    for (i = 0; i < MAX_OBSTACLES; ++i) pool->objects[i].active = 0;
    if (!config) return 0;
    if (count > MAX_OBSTACLES) count = MAX_OBSTACLES;
    for (i = 0; i < count; ++i) {
        Obstacle *object;
        if (!valid(&config[i])) continue;
        object = &pool->objects[pool->count++];
        object->config = &config[i];
        object->x = object->previous_x = (int32_t)config[i].x * POSITION_ONE;
        object->y = object->previous_y = (int32_t)config[i].y * POSITION_ONE;
        object->velocity = config[i].kind == OBSTACLE_STATIC ? 0 : config[i].speed;
        object->active = 1;
    }
    return pool->count;
}
void obstacles_step(ObstaclePool *pool)
{
    uint16_t i;
    for (i = 0; i < pool->count && i < MAX_OBSTACLES; ++i) {
        Obstacle *object = &pool->objects[i];
        const ObstacleConfig *c;
        int32_t next, low, high;
        if (!object->active) continue;
        c = object->config;
        object->previous_x = object->x;
        object->previous_y = object->y;
        if (c->kind == OBSTACLE_STATIC || c->minimum == c->maximum) continue;
        low = (int32_t)c->minimum * POSITION_ONE;
        high = (int32_t)c->maximum * POSITION_ONE;
        next = (c->kind == OBSTACLE_HORIZONTAL ? object->x : object->y) + object->velocity;
        if (next >= high) { next = high; object->velocity = -c->speed; }
        else if (next <= low) { next = low; object->velocity = c->speed; }
        if (c->kind == OBSTACLE_HORIZONTAL) object->x = next;
        else object->y = next;
    }
}
/* Exact fractions within one tick. Normal motor motion is <= 4 px/tick and
   obstacles <= 1 px/tick, keeping cross products bounded without 64-bit math. */
typedef struct { int32_t numerator, denominator; } Fraction;
static uint16_t less(Fraction a, Fraction b)
{
    return a.numerator * b.denominator < b.numerator * a.denominator;
}
static uint16_t sweep_axis(int32_t p, int32_t size, int32_t o, int32_t extent,
                           int32_t delta, Fraction *entry, Fraction *exit)
{
    int32_t start, end, speed;
    Fraction begin = {0, 1}, finish = {1, 1};
    if (!delta) return p < o + extent && p + size > o;
    if (delta > 0) { start = o - p - size; end = o + extent - p; speed = delta; }
    else { start = p - o - extent; end = p + size - o; speed = -delta; }
    if (start >= speed || end <= 0) return 0;
    if (start > 0) { begin.numerator = start; begin.denominator = speed; }
    if (end < speed) { finish.numerator = end; finish.denominator = speed; }
    if (less(*entry, begin)) *entry = begin;
    if (less(finish, *exit)) *exit = finish;
    return less(*entry, *exit);
}
uint16_t obstacles_hit(const ObstaclePool *pool, const Player *player,
                       int32_t previous_x, int32_t previous_y)
{
    uint16_t i;
    for (i = 0; i < pool->count && i < MAX_OBSTACLES; ++i) {
        const Obstacle *o = &pool->objects[i];
        Fraction entry = {0, 1}, exit = {1, 1};
        int32_t hit_x, hit_y;
        if (!o->active) continue;
        /* Configurable hitboxes are centered horizontally and anchored at the base. */
        hit_x = o->previous_x + ((o->config->kind == OBSTACLE_VERTICAL ? 8 : 16) -
                                o->config->width) * POSITION_ONE / 2;
        hit_y = o->previous_y + (8 - o->config->height) * POSITION_ONE;
        if (sweep_axis(previous_x, PLAYER_WIDTH * POSITION_ONE, hit_x,
                o->config->width * POSITION_ONE,
                player->x - previous_x - o->x + o->previous_x, &entry, &exit) &&
            sweep_axis(previous_y, PLAYER_HEIGHT * POSITION_ONE, hit_y,
                o->config->height * POSITION_ONE,
                player->y - previous_y - o->y + o->previous_y, &entry, &exit)) return 1;
    }
    return 0;
}
