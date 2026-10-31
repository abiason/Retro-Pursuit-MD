#ifndef RP_ENEMY_H
#define RP_ENEMY_H
#include "player.h"
#include "world/navigation.h"
typedef enum { ENEMY_IDLE, ENEMY_PATROL, ENEMY_ESCAPE, ENEMY_CLIMB, ENEMY_DESCEND } EnemyState;
typedef struct {
    uint16_t spawn_node, speed, detection_radius, idle_ticks;
} EnemyConfig;
typedef struct {
    Player body; /* Shared fixed-point motor and animation, with independent AI state. */
    EnemyState state;
    const NavigationGraph *graph;
    EnemyConfig config;
    int16_t current_node, target_node, patrol_goal;
    uint16_t active, wait_ticks, flee_ticks;
} Enemy;
extern const EnemyConfig enemy_test_config;
void enemy_init(Enemy *enemy, const LevelConfig *level, const NavigationGraph *graph,
                const EnemyConfig *config);
void enemy_update(Enemy *enemy, const LevelConfig *level, const Player *player);
#endif
