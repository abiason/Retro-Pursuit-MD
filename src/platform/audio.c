#include <genesis.h>
#include "audio.h"
#include "systems/audio_sequencer.h"
static AudioSequencer sequencer;
static AudioFrame cache;
static uint16_t cache_valid;
void audio_init(uint16_t refresh_rate)
{
    PSG_reset();
    PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK8);
    audio_sequencer_init(&sequencer, refresh_rate);
    cache_valid = 0;
}
void audio_update(const Game *game)
{
    const AudioContext context = {game->session.state, game->stage->id, game->events};
    AudioFrame frame;
    uint16_t i;
    audio_sequencer_update(&sequencer, context, &frame);
    for (i = 0; i < 3; ++i) {
        if (!cache_valid || cache.frequency[i] != frame.frequency[i]) PSG_setFrequency(i, frame.frequency[i]);
        cache.frequency[i] = frame.frequency[i];
    }
    for (i = 0; i < 4; ++i) {
        if (!cache_valid || cache.attenuation[i] != frame.attenuation[i]) PSG_setEnvelope(i, frame.attenuation[i]);
        cache.attenuation[i] = frame.attenuation[i];
    }
    cache_valid = 1;
}
