#include <genesis.h>
#include "video.h"
#include "player_tiles.h"
#include "enemy_tiles.h"
#include "obstacle_tiles.h"
#include "logo_tiles.h"
#include "assets.h"
#include "ui/hud.h"
#include "ui/menu.h"
#include "world/level.h"

static u16 video_rate;
static struct {
    u16 valid, frontend, selection;
    GameState state;
    const LevelConfig *level;
} view;

static const u32 world_tiles[WORLD_TILE_COUNT * 8] = {
    0x11111111,0xDDDDDDDD,0x12222221,0x12A22A21,0x12222221,0x22222222,0x2A2222A2,0x22222222,
    0xA300003A,0xA300003A,0xA333333A,0xA300003A,0xA300003A,0xA300003A,0xA333333A,0xA300003A,
    0x88888888,0x88888888,0x88888888,0x88888888,0x88888888,0x88888888,0x88888888,0x88888888,
    0x99999999,0x98C88889,0x98CC8889,0x988CC889,0x9888CC89,0x98888C89,0x98888889,0x99999999,
    0x99999999,0x98888889,0x9C888889,0x9CC88889,0x98CC8889,0x988CC889,0x9888CC89,0x99999999,
    0x8AA99988,0x8AA99988,0x8AA99988,0x8AA99988,0x8AA99988,0x8AA99988,0x8AA99988,0x8AA99988,
    0x88888888,0x8BBBBBB8,0x8B8BB8B8,0x8BBBBBB8,0x8BB8BBB8,0x8BBBBBB8,0x88888888,0x88888888,
    0x88888888,0x8A8A8A88,0x88888888,0x8A8A8A88,0x88888888,0x8A8A8A88,0x88888888,0x88888888
};
static void draw_shops(void)
{
    u16 row, x;
    for (row = 7; row <= 19; row += 6) {
        for (x = 4; x <= 34; x += 6) {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ASSET_WORLD_BASE + 6), x, row);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ASSET_WORLD_BASE + 6), x+1, row);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ASSET_WORLD_BASE + 3), x, row+1);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ASSET_WORLD_BASE + 4), x+1, row+1);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ASSET_WORLD_BASE + 7), x, row+2);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ASSET_WORLD_BASE + 7), x+1, row+2);
        }
    }
}
static void draw_pause_backdrop(void)
{
    u16 x, y;
    for (y = 8; y <= 20; ++y)
        for (x = 8; x <= 31; ++x)
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ASSET_WORLD_BASE + 2), x, y);
}

static void draw_world(const LevelConfig *level)
{
    u16 i;
    s16 x, y;
    draw_shops();
    for (i = 0; i < level->solid_count; ++i) {
        const WorldRect *r = &level->solids[i];
        for (y = r->y; y < r->y + r->height; y += 8)
            for (x = r->x; x < r->x + r->width; x += 8)
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE,
                    ASSET_WORLD_BASE), x / 8, y / 8);
    }
    for (i = 0; i < level->ladder_count; ++i) {
        const Ladder *ladder = &level->ladders[i];
        for (y = ladder->top; y < ladder->bottom; y += 8)
            for (x = ladder->x; x < ladder->x + ladder->width; x += 8)
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE,
                    ASSET_WORLD_BASE + 1), x / 8, y / 8);
    }
}

static void draw_frontend_background(void)
{
    u16 x, y;
    const u16 tile = TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, ASSET_WORLD_BASE);
    for (x = 2; x <= 37; ++x) {
        VDP_setTileMapXY(BG_B, tile, x, 3);
        VDP_setTileMapXY(BG_B, tile, x, 23);
    }
    for (y = 4; y < 23; ++y) {
        VDP_setTileMapXY(BG_B, tile, 2, y);
        VDP_setTileMapXY(BG_B, tile, 37, y);
    }
}

uint16_t video_init(void)
{
    const bool pal = IS_PAL_SYSTEM != 0;
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();
    PAL_setColor(0, 0x0200);
    PAL_setColor(17, 0x0EEE);
    PAL_setColor(18, 0x00AE);
    PAL_setColor(19, 0x0048);
    PAL_setColor(20, 0x0E80);
    PAL_setColor(21, 0x0ACE);
    VDP_loadTileData(player_tiles, ASSET_PLAYER_BASE, CHARACTER_TILE_COUNT, CPU);
    PAL_setColor(33, 0x0C84);
    PAL_setColor(34, 0x0642);
    PAL_setColor(35, 0x00AE);
    VDP_loadTileData(world_tiles, ASSET_WORLD_BASE, WORLD_TILE_COUNT, CPU);
    PAL_setColor(49, 0x0222);
    PAL_setColor(50, 0x0A2A);
    PAL_setColor(51, 0x0EEE);
    PAL_setColor(52, 0x0408);
    PAL_setColor(53, 0x0ACE);
    VDP_loadTileData(enemy_tiles, ASSET_ENEMY_BASE, CHARACTER_TILE_COUNT, CPU);
    PAL_setColor(36, 0x002E);
    PAL_setColor(37, 0x00EE);
    PAL_setColor(38, 0x0EEE);
    PAL_setColor(39, 0x0222);
    VDP_loadTileData(obstacle_tiles, ASSET_OBSTACLE_BASE, OBSTACLE_TILE_COUNT, CPU);
    VDP_loadTileData(logo_tiles, ASSET_LOGO_BASE, LOGO_TILE_COUNT, CPU);
    PAL_setColor(22, 0x0C64); PAL_setColor(23, 0x00AE);
    PAL_setColor(24, 0x068A); PAL_setColor(25, 0x0EEE);
    PAL_setColor(54, 0x0E6E); PAL_setColor(55, 0x0444);
    PAL_setColor(56, 0x068A); PAL_setColor(57, 0x002E);
    PAL_setColor(40, 0x0222); PAL_setColor(41, 0x0642);
    PAL_setColor(42, 0x0444); PAL_setColor(43, 0x008A);
    PAL_setColor(44, 0x0864); PAL_setColor(45, 0x0AAA);
    PAL_setColor(46, 0x0026); PAL_setColor(47, 0x0CEE);
    VDP_resetSprites();
    video_rate = pal ? 50 : 60;
    view.valid = 0;
    return pal ? 50 : 60;
}

static void render_world_sprites(const Game *game, GameState state)
{
    const u16 show_enemy = game->enemy.active && state != GAME_STATE_LEVEL_COMPLETE &&
        state != GAME_STATE_CAMPAIGN_COMPLETE;
    const s16 player_x = state == GAME_STATE_PLAYING &&
        game->session.invulnerability_ticks && (game->session.invulnerability_ticks / 6) % 2 ?
        -32 : player_pixel_x(&game->player);
    u16 player_frame = game->player.frame;
    u16 total = 1 + show_enemy, index = 1, i;
    if (state == GAME_STATE_LEVEL_COMPLETE || state == GAME_STATE_CAMPAIGN_COMPLETE)
        player_frame = PLAYER_FRAME_CAPTURE;
    else if (state == GAME_STATE_PLAYING && game->session.invulnerability_ticks >
        (game->rules.invulnerability_duration > 18 ? game->rules.invulnerability_duration - 18 : 0))
        player_frame = PLAYER_FRAME_HURT;
    for (i = 0; i < game->obstacles.count && i < MAX_OBSTACLES; ++i)
        if (game->obstacles.objects[i].active) ++total;
    VDP_setSpriteFull(0, player_x, player_pixel_y(&game->player),
        SPRITE_SIZE(2, 3), TILE_ATTR_FULL(PAL1, TRUE, FALSE,
        game->player.facing < 0, ASSET_PLAYER_BASE + player_frame * CHARACTER_FRAME_TILES), total > 1 ? 1 : 0);
    if (show_enemy) {
        const Player *body = &game->enemy.body;
        const s16 x = state == GAME_STATE_CAPTURING &&
            (game->session.capture_ticks / 6) % 2 ? -32 : player_pixel_x(body);
        VDP_setSpriteFull(index, x, player_pixel_y(body), SPRITE_SIZE(2, 3),
            TILE_ATTR_FULL(PAL3, TRUE, FALSE, body->facing < 0,
                ASSET_ENEMY_BASE + body->frame * CHARACTER_FRAME_TILES), index + 1 < total ? index + 1 : 0);
        ++index;
    }
    for (i = 0; i < game->obstacles.count && i < MAX_OBSTACLES; ++i) {
        const Obstacle *object = &game->obstacles.objects[i];
        u16 tile, width;
        if (!object->active) continue;
        tile = ASSET_OBSTACLE_BASE + object->config->kind * 2;
        width = object->config->kind == OBSTACLE_VERTICAL ? 1 : 2;
        VDP_setSpriteFull(index, (s16)(object->x / POSITION_ONE), (s16)(object->y / POSITION_ONE),
            SPRITE_SIZE(width, 1), TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, tile),
            index + 1 < total ? index + 1 : 0);
        ++index;
    }
    VDP_updateSprites(total, DMA_QUEUE);
}

void video_render_scene(const Game *game)
{
    const GameState state = game->session.state;
    const u16 frontend = scene_is_frontend(state);
    const u16 selection = frontend ? game->scene.title_selection : game->scene.pause_selection;
    const u16 map_changed = !view.valid || game->level != view.level;
    const u16 redraw = !view.valid || frontend != view.frontend ||
        (frontend && state != view.state) || (!frontend && map_changed) ||
        ((state == GAME_STATE_PAUSED) != (view.state == GAME_STATE_PAUSED));
    if (redraw) {
        VDP_clearPlane(BG_A, TRUE);
        if (!view.valid || frontend || frontend != view.frontend || map_changed ||
            state == GAME_STATE_PAUSED || view.state == GAME_STATE_PAUSED) {
            VDP_clearPlane(BG_B, TRUE);
            PAL_setColor(33, frontend ? 0x0C84 : game->stage->outline_color);
            PAL_setColor(34, frontend ? 0x0642 : game->stage->fill_color);
            PAL_setColor(35, frontend ? 0x00AE : game->stage->ladder_color);
            if (frontend) draw_frontend_background();
            else { draw_world(game->level); if (state == GAME_STATE_PAUSED) draw_pause_backdrop(); }
        }
        if (!frontend) {
            VDP_drawText("RETRO PURSUIT", 13, 0);
            VDP_drawText("L/R MOVE U/D LADDER A JUMP START PAUSE", 1, 3);
            VDP_drawText(game->stage->name, 2, 4);
            hud_init();
        }
        VDP_drawText(video_rate == 50 ? "PAL 50 HZ" : "NTSC 60 HZ", 29, 27);
    }
    if (frontend) {
        if (redraw || selection != view.selection) menu_render_frontend(state, selection);
        VDP_setSpriteFull(0, -32, -32, SPRITE_SIZE(1, 1), TILE_USER_INDEX, 0);
        VDP_updateSprites(1, DMA_QUEUE);
    } else {
        hud_render(&game->session);
        if (state == GAME_STATE_PAUSED && (redraw || selection != view.selection))
            menu_render_pause(selection);
        if (state == GAME_STATE_PAUSED) {
            VDP_setSpriteFull(0, -32, -32, SPRITE_SIZE(1, 1), ASSET_PLAYER_BASE, 0);
            VDP_updateSprites(1, DMA_QUEUE);
        } else render_world_sprites(game, state);
    }
    view.valid = 1;
    view.frontend = frontend;
    view.selection = selection;
    view.state = state;
    view.level = game->level;
}
