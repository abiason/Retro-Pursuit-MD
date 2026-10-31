#ifndef RP_GAME_H
#define RP_GAME_H
#include "entities/player.h"
#include "entities/enemy.h"
#include "game_session.h"
#include "scene_manager.h"
#include "entities/obstacle.h"
#include "world/stage.h"
#include "game_events.h"
enum { GAME_WIDTH = 320, GAME_HEIGHT = 224 };
typedef struct { int8_t horizontal; uint16_t jump_held; int8_t vertical; uint16_t start_held; } InputState;
typedef struct {
    GameSession session;
    GameRules rules;
    SceneManager scene;
    Player player;
    Enemy enemy;
    ObstaclePool obstacles;
    const LevelConfig *level;
    const StageConfig *stage;
    uint16_t stage_index, custom_rules, events, wall_contact;
    uint16_t tick_remainder, previous_jump, previous_start, refresh_rate;
    int8_t previous_vertical;
} Game;
void game_init(Game *game, uint16_t refresh_rate);
void game_init_with_rules(Game *game, uint16_t refresh_rate, const GameRules *rules);
uint16_t game_begin_stage(Game *game, uint16_t index);
void game_start_session(Game *game);
void game_show_title(Game *game);
void game_update(Game *game, InputState input);
#endif
