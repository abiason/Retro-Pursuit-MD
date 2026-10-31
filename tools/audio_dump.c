/* Host-only preview: emit the actual sequencer's PSG requests, not emulator audio. */
#include <stdio.h>
#include "systems/audio_sequencer.h"
int main(void)
{
    AudioSequencer audio;
    AudioFrame frame;
    AudioContext context = {GAME_STATE_TITLE, 1, 0};
    unsigned i;
    audio_sequencer_init(&audio, 60);
    for (i = 0; i < 1020; ++i) {
        context.events = 0;
        if (i < 180) context.state = GAME_STATE_TITLE;
        else if (i < 420) context.state = GAME_STATE_PLAYING;
        else if (i < 450) context.state = GAME_STATE_PAUSED;
        else if (i < 540) context.state = GAME_STATE_PLAYING;
        else if (i < 570) context.state = GAME_STATE_CAPTURING;
        else if (i < 630) context.state = GAME_STATE_LEVEL_COMPLETE;
        else if (i < 750) { context.state = GAME_STATE_PLAYING; context.stage = 2; }
        else if (i < 870) { context.state = GAME_STATE_PLAYING; context.stage = 3; }
        else context.state = GAME_STATE_CAMPAIGN_COMPLETE;
        if (i == 180 || i == 630 || i == 750) context.events = GAME_EVENT_STAGE_START;
        if (i == 230) context.events = GAME_EVENT_JUMP;
        if (i == 280) context.events = GAME_EVENT_COLLISION;
        if (i == 330) context.events = GAME_EVENT_DAMAGE;
        audio_sequencer_update(&audio, context, &frame);
        printf("%u %u %u %u %u %u %u\n", frame.frequency[0], frame.frequency[1], frame.frequency[2],
            frame.attenuation[0], frame.attenuation[1], frame.attenuation[2], frame.attenuation[3]);
    }
    return 0;
}
