#ifndef RP_OBSTACLE_H
#define RP_OBSTACLE_H
#include "player.h"
enum { MAX_OBSTACLES = 16 };
typedef enum { OBSTACLE_STATIC, OBSTACLE_HORIZONTAL, OBSTACLE_VERTICAL } ObstacleKind;
/* x/y and travel limits are pixels; speed is 1/256 pixel per 120Hz tick.
   Hitboxes are centered horizontally and bottom-aligned within the 16x8 or 8x8 visual. */
typedef struct {
    ObstacleKind kind;
    int16_t x, y, width, height, minimum, maximum, speed;
} ObstacleConfig;
typedef struct {
    const ObstacleConfig *config;
    int32_t x, y, previous_x, previous_y;
    int16_t velocity;
    uint16_t active;
} Obstacle;
typedef struct { Obstacle objects[MAX_OBSTACLES]; uint16_t count; } ObstaclePool;
extern const ObstacleConfig obstacles_test_config[];
extern const uint16_t obstacles_test_count;
/* Inspect at most the first MAX_OBSTACLES configs and skip invalid entries. */
uint16_t obstacles_init(ObstaclePool *pool, const ObstacleConfig *config, uint16_t count);
void obstacles_step(ObstaclePool *pool);
/* Previous player positions must be from the beginning of the same physics tick. */
uint16_t obstacles_hit(const ObstaclePool *pool, const Player *player,
                       int32_t previous_x, int32_t previous_y);
#endif
