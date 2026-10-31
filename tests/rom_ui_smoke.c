/* Separate visual-test ROM. Production src/main.c never includes this harness. */
#include <genesis.h>
#include "core/game.h"
#include "platform/video.h"
#include "platform/audio.h"

static const InputState idle = {0, 0, 0, 0};
static const InputState action = {0, 1, 0, 0}, start = {0, 0, 0, 1};
#ifndef RP_CAMPAIGN_SMOKE
static const InputState down = {0, 0, 1, 0};
#endif
static void tap(Game *game, InputState input)
{
    game_update(game, idle);
    game_update(game, input);
    game_update(game, idle);
}
#ifdef RP_CAMPAIGN_SMOKE
static void finish_stage(Game *game, uint16_t advance)
{
    uint16_t frame;
    /* Controlled capture fixture: exercise actual progression, not a claimed playthrough. */
    game->enemy.body.x = game->player.x;
    game->enemy.body.y = game->player.y;
    game->enemy.body.vx = game->enemy.body.vy = 0;
    game_update(game, idle);
    if (!advance) return;
    for (frame = 0; frame < game->refresh_rate && game->session.state == GAME_STATE_CAPTURING; ++frame)
        game_update(game, idle);
    if (game->session.state == GAME_STATE_LEVEL_COMPLETE) tap(game, action);
}
static void enter_page(Game *game, u16 page, u16 hz)
{
    if (!page) { game_init(game, hz); game_start_session(game); }
    else if (page < 3) finish_stage(game, 1);
    else if (page == 3) finish_stage(game, 0);
    else tap(game, start);
}
#else
static void enter_page(Game *game, u16 page, u16 hz)
{
    switch (page) {
    case 0: game_init(game, hz); break;
    case 1: tap(game, down); tap(game, action); break;
    case 2: tap(game, action); tap(game, down); tap(game, action); break;
    case 3: tap(game, action); tap(game, down); tap(game, start); break;
    case 4: tap(game, start); break;
    case 5:
        /* Synthetic results exercise rendering; they do not claim a gameplay victory/loss. */
        game->session.state = GAME_STATE_GAME_OVER;
        game->session.lives = 0; game->session.score = 12345;
        break;
    case 6:
        game->session.state = GAME_STATE_LEVEL_COMPLETE;
        game->session.lives = 3; game->session.round = game->stage->id;
        break;
    case 7:
        game->session.state = GAME_STATE_ATTEMPT_FAILED;
        game->session.timer.ticks_remaining = 0; game->session.lives = 2;
        break;
    default: tap(game, start); break;
    }
}
#endif
int main(bool hardReset)
{
    static Game game;
    const u16 hz = video_init();
    u16 page = 0, frame = 0;
    enter_page(&game, page, hz);
    audio_init(hz);
    while (TRUE) {
        game_update(&game, idle);
        audio_update(&game);
        video_render_scene(&game);
#ifdef RP_CAMPAIGN_SMOKE
        VDP_drawText("CAMPAIGN TEST", 0, 27);
#else
        VDP_drawText("VISUAL TEST", 0, 27);
#endif
        SYS_doVBlankProcess();
        if (++frame >= hz * 6) {
            frame = 0;
#ifdef RP_CAMPAIGN_SMOKE
            page = (page + 1) % 5;
#else
            page = (page + 1) % 9;
#endif
            enter_page(&game, page, hz);
        }
    }
    return 0;
}
