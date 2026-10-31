#ifndef RP_PLAYER_H
#define RP_PLAYER_H
#include "core/types.h"
#include "world/level.h"
enum { PLAYER_WIDTH = 16, PLAYER_HEIGHT = 24, POSITION_ONE = 256,
       PLAYER_TICK_HZ = 120 };
typedef struct {
    int32_t x, y;
    int16_t vx, vy;
    uint16_t grounded, animation_ticks, frame;
    int8_t facing;
    int16_t ladder_index;
} Player;
void player_init(Player *player, const LevelConfig *level);
void player_update(Player *player, const LevelConfig *level, int8_t horizontal,
                   int8_t vertical, uint16_t jump_pressed);
int16_t player_pixel_x(const Player *player);
int16_t player_pixel_y(const Player *player);
#endif
