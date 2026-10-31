#ifndef RP_PHYSICS_H
#define RP_PHYSICS_H
#include "entities/player.h"
void physics_step(Player *player, const LevelConfig *level, int8_t horizontal,
                  int8_t vertical, uint16_t jump_pressed);
void physics_step_with_speed(Player *player, const LevelConfig *level, int8_t horizontal,
                             int8_t vertical, uint16_t jump_pressed, int16_t speed);
#endif
