#include "scene_manager.h"
void scene_init(SceneManager *scene)
{
    scene->title_selection = TITLE_START;
    scene->pause_selection = PAUSE_CONTINUE;
    scene->paused_state = GAME_STATE_PLAYING;
}
uint16_t scene_selection_step(uint16_t selection, int8_t direction, uint16_t count)
{
    if (!count) return 0;
    if (selection >= count) selection = 0;
    if (direction > 0) return selection + 1 < count ? selection + 1 : 0;
    if (direction < 0) return selection ? selection - 1 : count - 1;
    return selection;
}
uint16_t scene_is_frontend(GameState state)
{
    return state == GAME_STATE_TITLE || state == GAME_STATE_INSTRUCTIONS || state == GAME_STATE_CREDITS;
}
uint16_t scene_can_pause(GameState state)
{
    return state == GAME_STATE_PLAYING || state == GAME_STATE_CAPTURING;
}
GameState scene_simulation_state(const SceneManager *scene, GameState state)
{
    return state == GAME_STATE_PAUSED ? scene->paused_state : state;
}
