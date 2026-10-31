#include "animation.h"
void animation_update(Player *player)
{
    if (player->ladder_index >= 0) {
        if (player->vy) player->animation_ticks = (player->animation_ticks + 1) % 24;
        player->frame = player->animation_ticks < 12 ? PLAYER_FRAME_CLIMB_A : PLAYER_FRAME_CLIMB_B;
    } else if (!player->grounded) {
        player->frame = PLAYER_FRAME_JUMP;
        player->animation_ticks = 0;
    } else if (player->vx) {
        player->animation_ticks = (player->animation_ticks + 1) % 24;
        player->frame = player->animation_ticks < 12 ? PLAYER_FRAME_WALK_A : PLAYER_FRAME_WALK_B;
    } else {
        player->frame = PLAYER_FRAME_IDLE;
        player->animation_ticks = 0;
    }
}
