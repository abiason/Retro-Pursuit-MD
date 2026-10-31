#include <genesis.h>
#include "core/game.h"
#include "platform/input.h"
#include "platform/video.h"
#include "platform/audio.h"

int main(bool hardReset)
{
    static Game game;
    game_init(&game, video_init());
    input_init();
    audio_init(game.refresh_rate);
    while (TRUE)
    {
        game_update(&game, input_update());
        audio_update(&game);
        video_render_scene(&game);
        SYS_doVBlankProcess();
    }
    return 0;
}
