#include <genesis.h>
#include "menu.h"
#include "platform/assets.h"

void menu_render_frontend(GameState state, uint16_t selection)
{
    if (state == GAME_STATE_TITLE) {
        u16 x, y;
        for (y = 0; y < 6; ++y)
            for (x = 0; x < 16; ++x)
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE,
                    ASSET_LOGO_BASE + y * 16 + x), 12 + x, 6 + y);
        VDP_drawText("ARCADE CHASE", 14, 13);
        VDP_drawText("START GAME", 15, 16);
        VDP_drawText("INSTRUCTIONS", 14, 18);
        VDP_drawText("CREDITS", 16, 20);
        VDP_drawText(" ", 12, 16); VDP_drawText(" ", 12, 18); VDP_drawText(" ", 12, 20);
        if (selection < TITLE_OPTION_COUNT) VDP_drawText(">", 12, 16 + selection * 2);
        VDP_drawText("UP/DOWN SELECT   A/START CONFIRM", 4, 25);
    } else if (state == GAME_STATE_INSTRUCTIONS) {
        VDP_drawText("HOW TO PLAY", 14, 5);
        VDP_drawText("LEFT/RIGHT: MOVE", 5, 8);
        VDP_drawText("UP/DOWN: CLIMB LADDERS", 5, 10);
        VDP_drawText("A: JUMP", 5, 12);
        VDP_drawText("START: PAUSE / CONTINUE", 5, 14);
        VDP_drawText("CLEAR 3 STAGES: CATCH THE RUNNER", 4, 17);
        VDP_drawText("AVOID RED HAZARDS - 3 LIVES", 6, 19);
        VDP_drawText("A HIT GIVES A SHORT SHIELD", 7, 21);
        VDP_drawText("A/START: MAIN MENU", 11, 25);
    } else if (state == GAME_STATE_CREDITS) {
        VDP_drawText("CREDITS", 16, 5);
        VDP_drawText("RETRO PURSUIT", 13, 8);
        VDP_drawText("A game by Biason X", 11, 10);
        VDP_drawText("ORIGINAL CODE, ART AND MUSIC", 7, 12);
        VDP_drawText("PSG SOUNDTRACK / C / SGDK", 8, 15);
        VDP_drawText("SGDK: STEPHANE DALLONGEVILLE", 6, 17);
        VDP_drawText("SEGA MEGA DRIVE / GENESIS", 8, 20);
        VDP_drawText("A/START: MAIN MENU", 11, 25);
    }
}
void menu_render_pause(uint16_t selection)
{
    VDP_drawText("+--------------------+", 9, 9);
    VDP_drawText("PAUSED", 17, 11);
    VDP_drawText("CONTINUE", 16, 14);
    VDP_drawText("MAIN MENU", 15, 16);
    VDP_drawText(" ", 12, 14); VDP_drawText(" ", 12, 16);
    if (selection < PAUSE_OPTION_COUNT) VDP_drawText(">", 12, 14 + selection * 2);
    VDP_drawText("+--------------------+", 9, 19);
}
