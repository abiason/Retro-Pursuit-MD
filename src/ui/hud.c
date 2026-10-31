#include <genesis.h>
#include "hud.h"

static struct {
    uint32_t score;
    uint16_t seconds, lives, round, valid, protected;
    GameState state;
} cache;

static void number(uint32_t value, uint16_t digits, uint16_t x, uint16_t y, char padding)
{
    char text[11];
    uint16_t i = digits;
    text[i] = 0;
    while (i) {
        --i;
        if (value || i == digits - 1) text[i] = (char)('0' + (uint16_t)(value % 10));
        else text[i] = padding;
        value /= 10;
    }
    VDP_drawText(text, x, y);
}
void hud_init(void)
{
    cache.valid = 0;
    VDP_drawText("PTS            TIME", 1, 1);
    VDP_drawText("LIVES       STAGE", 1, 2);
    VDP_drawText("/", 21, 2);
    number(stage_count(), 2, 22, 2, '0');
}
void hud_render(const GameSession *session)
{
    const uint16_t seconds = timer_seconds(&session->timer);
    const uint16_t protected = session->state == GAME_STATE_PLAYING && session->invulnerability_ticks != 0;
    if (!cache.valid || cache.score != session->score) number(session->score, 10, 5, 1, '0');
    if (!cache.valid || cache.seconds != seconds) number(seconds, 5, 21, 1, ' ');
    if (!cache.valid || cache.lives != session->lives) number(session->lives, 5, 7, 2, ' ');
    if (!cache.valid || cache.round != session->round) number(session->round, 2, 19, 2, '0');
    if (!cache.valid || cache.state != session->state || cache.protected != protected) {
        VDP_drawText("                                        ", 0, 25);
        VDP_drawText("                                        ", 0, 26);
        switch (session->state) {
        case GAME_STATE_PAUSED:
            VDP_drawText("PAUSED", 17, 25);
            VDP_drawText("A: CONFIRM  START: CONTINUE", 7, 26);
            break;
        case GAME_STATE_CAPTURING:
            VDP_drawText("CAPTURED!", 15, 25); break;
        case GAME_STATE_LEVEL_COMPLETE:
            VDP_drawText("STAGE CLEAR", 14, 25);
            VDP_drawText("A: NEXT STAGE   START: MENU", 6, 26); break;
        case GAME_STATE_ATTEMPT_FAILED:
            VDP_drawText("TIME UP - LIFE LOST", 10, 25);
            VDP_drawText("A: RETRY   START: MENU", 9, 26); break;
        case GAME_STATE_CAMPAIGN_COMPLETE:
            VDP_drawText("ALL STAGES CLEAR!", 12, 25);
            VDP_drawText("A: NEW GAME   START: MENU", 7, 26); break;
        case GAME_STATE_GAME_OVER:
            VDP_drawText("GAME OVER", 15, 25);
            VDP_drawText("A: NEW GAME   START: MENU", 7, 26); break;
        default:
            if (protected) VDP_drawText("HIT! TEMPORARY SHIELD", 10, 25);
            else VDP_drawText("CATCH THE RUNNER!", 12, 25);
            break;
        }
    }
    cache.protected = protected;
    cache.valid = 1;
    cache.score = session->score;
    cache.seconds = seconds;
    cache.lives = session->lives;
    cache.round = session->round;
    cache.state = session->state;
}
