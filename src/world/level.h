#ifndef RP_LEVEL_H
#define RP_LEVEL_H
#include "core/types.h"
typedef struct { int16_t x, y, width, height; } WorldRect;
/* Ladder top/bottom are floor surface heights, measured at the player's feet. */
typedef struct { int16_t x, width, top, bottom; } Ladder;
typedef struct {
    const WorldRect *solids;
    uint16_t solid_count;
    const Ladder *ladders;
    uint16_t ladder_count;
    int16_t spawn_x, spawn_y;
} LevelConfig;
extern const LevelConfig level_test;
#endif
