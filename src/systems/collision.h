#ifndef RP_COLLISION_H
#define RP_COLLISION_H
#include "entities/player.h"
#include "world/level.h"
uint16_t collision_bodies_overlap(const Player *a, const Player *b);
uint16_t collision_aabb(WorldRect a, WorldRect b);
uint16_t collision_player_overlaps(const Player *player, const LevelConfig *level);
void collision_move(Player *player, const LevelConfig *level);
#endif
