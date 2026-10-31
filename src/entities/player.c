#include "player.h"
#include "systems/physics.h"
#include "systems/animation.h"
void player_init(Player *player, const LevelConfig *level)
{
    player->x = (int32_t)level->spawn_x * POSITION_ONE;
    player->y = (int32_t)level->spawn_y * POSITION_ONE;
    player->vx = player->vy = 0;
    player->grounded = 1;
    player->animation_ticks = 0;
    player->frame = PLAYER_FRAME_IDLE;
    player->facing = 1;
    player->ladder_index = -1;
}
void player_update(Player *player, const LevelConfig *level, int8_t horizontal,
                   int8_t vertical, uint16_t jump_pressed)
{
    if (horizontal) player->facing = horizontal < 0 ? -1 : 1;
    physics_step(player, level, horizontal, vertical, jump_pressed);
    animation_update(player);
}
int16_t player_pixel_x(const Player *player) { return (int16_t)(player->x / POSITION_ONE); }
int16_t player_pixel_y(const Player *player) { return (int16_t)(player->y / POSITION_ONE); }
