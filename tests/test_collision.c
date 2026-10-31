#include <assert.h>
#include <stdio.h>
#include "systems/collision.h"

static void place(Player *p, const LevelConfig *level, int16_t x, int16_t y,
                  int16_t vx, int16_t vy)
{
    player_init(p, level);
    p->x = (int32_t)x * POSITION_ONE;
    p->y = (int32_t)y * POSITION_ONE;
    p->vx = vx;
    p->vy = vy;
}
int main(void)
{
    const WorldRect a = {0, 0, 16, 24}, touching = {16, 0, 16, 24};
    const WorldRect overlap = {15, 23, 8, 8}, empty = {0, 0, 0, 4};
    const WorldRect geometry[] = {
        {96, 40, 8, 160}, {64, 40, 8, 160},
        {0, 144, 64, 8}, {0, 80, 64, 8}
    };
    const LevelConfig level = {geometry, 4, 0, 0, 24, 100};
    Player p;
    assert(!collision_aabb(a, touching));
    assert(collision_aabb(a, overlap));
    assert(!collision_aabb(a, empty));
    place(&p, &level, 24, 100, 30000, 0);
    collision_move(&p, &level);
    assert(player_pixel_x(&p) == 48 && p.vx == 0);
    assert(!collision_player_overlaps(&p, &level));
    place(&p, &level, 120, 100, -30000, 0);
    collision_move(&p, &level);
    assert(player_pixel_x(&p) == 104 && p.vx == 0);
    place(&p, &level, 24, 110, 0, 4096);
    collision_move(&p, &level);
    assert(player_pixel_y(&p) == 120 && p.grounded && p.vy == 0);
    place(&p, &level, 24, 88, 0, -4096);
    collision_move(&p, &level);
    assert(player_pixel_y(&p) == 88 && p.vy == 0 && !p.grounded);
    place(&p, &level, 24, 120, 0, 24);
    collision_move(&p, &level);
    assert(p.grounded && !collision_player_overlaps(&p, &level));
    place(&p, &level, 104, 120, 0, 24);
    collision_move(&p, &level);
    assert(!p.grounded); /* Contact with a wall edge is not floor support. */
    puts("PASS: AABB, touching edges, nearest wall, swept landing, ceiling, support");
    return 0;
}
