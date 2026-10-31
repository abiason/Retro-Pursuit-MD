#ifndef RP_STAGE_H
#define RP_STAGE_H
#include "level.h"
#include "navigation.h"
#include "entities/enemy.h"
#include "entities/obstacle.h"
enum { MAX_LEVELS = 10 };
typedef struct {
    uint16_t id, difficulty, time_limit;
    const char *name;
    const LevelConfig *level;
    const NavigationGraph *navigation;
    const EnemyConfig *enemy;
    const ObstacleConfig *obstacles;
    uint16_t obstacle_count;
    uint16_t outline_color, fill_color, ladder_color;
} StageConfig;
uint16_t stage_count(void);
const StageConfig *stage_get(uint16_t index);
#endif
