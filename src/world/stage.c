#include "stage.h"
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

/* Stage 1 references the original approved map, motor tuning and hazards. */
static const WorldRect gallery_solids[] = {
    {8, 48, 8, 152}, {304, 48, 8, 152}, {8, 40, 304, 8},
    {16, 192, 288, 8}, {16, 144, 288, 8}, {16, 96, 288, 8},
    {96, 184, 16, 8}, {176, 136, 16, 8}
};
static const Ladder gallery_ladders[] = {{240, 16, 144, 192}, {64, 16, 96, 144}};
static const LevelConfig gallery_level = {
    gallery_solids, COUNT_OF(gallery_solids), gallery_ladders, COUNT_OF(gallery_ladders), 32, 168
};
static const NavigationNode gallery_nodes[] = {
    {32, 192}, {240, 192}, {288, 192},
    {32, 144}, {64, 144}, {240, 144}, {288, 144},
    {32, 96}, {64, 96}, {288, 96}
};
static const NavigationEdge gallery_edges[] = {
    {0, 1, -1}, {1, 2, -1}, {3, 4, -1}, {4, 5, -1}, {5, 6, -1},
    {7, 8, -1}, {8, 9, -1}, {1, 5, 0}, {4, 8, 1}
};
static const NavigationGraph gallery_navigation = {
    gallery_nodes, COUNT_OF(gallery_nodes), gallery_edges, COUNT_OF(gallery_edges)
};
static const EnemyConfig gallery_enemy = {9, 216, 112, 48};
static const ObstacleConfig gallery_obstacles[] = {
    {OBSTACLE_STATIC, 96, 88, 6, 4, 0, 0, 0},
    {OBSTACLE_HORIZONTAL, 152, 88, 6, 4, 152, 168, 160},
    {OBSTACLE_STATIC, 216, 88, 6, 4, 0, 0, 0},
    {OBSTACLE_VERTICAL, 272, 56, 6, 6, 56, 88, 48}
};
static const WorldRect security_solids[] = {
    {8, 48, 8, 152}, {304, 48, 8, 152}, {8, 40, 304, 8},
    {16, 192, 288, 8}, {16, 144, 288, 8}, {16, 96, 288, 8},
    {176, 184, 16, 8}, {96, 136, 16, 8}
};
static const Ladder security_ladders[] = {{48, 16, 144, 192}, {160, 16, 96, 144}};
static const LevelConfig security_level = {
    security_solids, COUNT_OF(security_solids), security_ladders, COUNT_OF(security_ladders), 32, 168
};
static const NavigationNode security_nodes[] = {
    {32, 192}, {48, 192}, {288, 192},
    {32, 144}, {48, 144}, {160, 144}, {288, 144},
    {32, 96}, {160, 96}, {288, 96}
};
static const NavigationEdge security_edges[] = {
    {0, 1, -1}, {1, 2, -1}, {3, 4, -1}, {4, 5, -1}, {5, 6, -1},
    {7, 8, -1}, {8, 9, -1}, {1, 4, 0}, {5, 8, 1}
};
static const NavigationGraph security_navigation = {
    security_nodes, COUNT_OF(security_nodes), security_edges, COUNT_OF(security_edges)
};
static const EnemyConfig security_enemy = {9, 240, 128, 30};
static const ObstacleConfig security_obstacles[] = {
    {OBSTACLE_STATIC, 64, 88, 6, 4, 0, 0, 0},
    {OBSTACLE_HORIZONTAL, 104, 88, 6, 4, 104, 120, 192},
    {OBSTACLE_STATIC, 200, 88, 6, 4, 0, 0, 0},
    {OBSTACLE_HORIZONTAL, 232, 88, 6, 4, 232, 248, 192},
    {OBSTACLE_VERTICAL, 272, 56, 6, 6, 56, 88, 64}
};
static const StageConfig stages[] = {
    {1, 1, 90, "MAIN ATRIUM", &level_test, &navigation_test, &enemy_test_config,
        obstacles_test_config, 3, 0x0C84, 0x0642, 0x00AE},
    {2, 2, 80, "EAST GALLERY", &gallery_level, &gallery_navigation, &gallery_enemy,
        gallery_obstacles, COUNT_OF(gallery_obstacles), 0x04C6, 0x0262, 0x00AE},
    {3, 3, 70, "SECURITY HUB", &security_level, &security_navigation, &security_enemy,
        security_obstacles, COUNT_OF(security_obstacles), 0x048C, 0x0246, 0x00AE}
};
typedef char stage_capacity_check[(COUNT_OF(stages) > 0 && COUNT_OF(stages) <= MAX_LEVELS) ? 1 : -1];
uint16_t stage_count(void) { return (uint16_t)COUNT_OF(stages); }
const StageConfig *stage_get(uint16_t index)
{
    return index < stage_count() ? &stages[index] : 0;
}
