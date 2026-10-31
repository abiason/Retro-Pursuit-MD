#ifndef RP_SCENE_MANAGER_H
#define RP_SCENE_MANAGER_H
#include "types.h"
#include "game_state.h"
enum { TITLE_START, TITLE_INSTRUCTIONS, TITLE_CREDITS, TITLE_OPTION_COUNT };
enum { PAUSE_CONTINUE, PAUSE_MAIN_MENU, PAUSE_OPTION_COUNT };
typedef struct {
    uint16_t title_selection, pause_selection;
    GameState paused_state;
} SceneManager;
void scene_init(SceneManager *scene);
uint16_t scene_selection_step(uint16_t selection, int8_t direction, uint16_t count);
uint16_t scene_is_frontend(GameState state);
uint16_t scene_can_pause(GameState state);
GameState scene_simulation_state(const SceneManager *scene, GameState state);
#endif
