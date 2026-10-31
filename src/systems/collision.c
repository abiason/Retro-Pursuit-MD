#include "collision.h"

uint16_t collision_aabb(WorldRect a, WorldRect b)
{
    return a.width > 0 && a.height > 0 && b.width > 0 && b.height > 0 &&
        (int32_t)a.x < (int32_t)b.x + b.width && (int32_t)a.x + a.width > b.x &&
        (int32_t)a.y < (int32_t)b.y + b.height && (int32_t)a.y + a.height > b.y;
}
static uint16_t interval_overlap(int32_t a, int32_t size, int32_t b, int32_t extent)
{
    return a < b + extent && a + size > b;
}
uint16_t collision_bodies_overlap(const Player *a, const Player *b)
{
    return interval_overlap(a->x, PLAYER_WIDTH * POSITION_ONE, b->x, PLAYER_WIDTH * POSITION_ONE) &&
           interval_overlap(a->y, PLAYER_HEIGHT * POSITION_ONE, b->y, PLAYER_HEIGHT * POSITION_ONE);
}
uint16_t collision_player_overlaps(const Player *player, const LevelConfig *level)
{
    uint16_t i;
    for (i = 0; i < level->solid_count; ++i) {
        const WorldRect *r = &level->solids[i];
        if (interval_overlap(player->x, PLAYER_WIDTH * POSITION_ONE,
                (int32_t)r->x * POSITION_ONE, (int32_t)r->width * POSITION_ONE) &&
            interval_overlap(player->y, PLAYER_HEIGHT * POSITION_ONE,
                (int32_t)r->y * POSITION_ONE, (int32_t)r->height * POSITION_ONE)) return 1;
    }
    return 0;
}
void collision_move(Player *player, const LevelConfig *level)
{
    const int32_t width = PLAYER_WIDTH * POSITION_ONE;
    const int32_t height = PLAYER_HEIGHT * POSITION_ONE;
    int32_t target = player->x + player->vx;
    uint16_t i;
    /* Resolve nearest crossed surface, regardless of the order of level solids. */
    for (i = 0; i < level->solid_count; ++i) {
        const WorldRect *r = &level->solids[i];
        const int32_t left = (int32_t)r->x * POSITION_ONE;
        const int32_t right = (int32_t)(r->x + r->width) * POSITION_ONE;
        if (!interval_overlap(player->y, height, (int32_t)r->y * POSITION_ONE,
                              (int32_t)r->height * POSITION_ONE)) continue;
        if (player->vx > 0 && player->x + width <= left && target + width > left)
            target = left - width;
        if (player->vx < 0 && player->x >= right && target < right) target = right;
    }
    if (target != player->x + player->vx) player->vx = 0;
    player->x = target;
    target = player->y + player->vy;
    player->grounded = 0;
    for (i = 0; i < level->solid_count; ++i) {
        const WorldRect *r = &level->solids[i];
        const int32_t top = (int32_t)r->y * POSITION_ONE;
        const int32_t bottom = (int32_t)(r->y + r->height) * POSITION_ONE;
        if (!interval_overlap(player->x, width, (int32_t)r->x * POSITION_ONE,
                              (int32_t)r->width * POSITION_ONE)) continue;
        if (player->vy > 0 && player->y + height <= top && target + height >= top) {
            target = top - height;
            player->grounded = 1;
        }
        if (player->vy < 0 && player->y >= bottom && target < bottom) target = bottom;
    }
    if (target != player->y + player->vy || player->grounded) player->vy = 0;
    player->y = target;
}
