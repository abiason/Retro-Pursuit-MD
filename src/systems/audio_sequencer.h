#ifndef RP_AUDIO_SEQUENCER_H
#define RP_AUDIO_SEQUENCER_H
#include "core/types.h"
#include "core/game_state.h"
#include "core/game_events.h"
typedef enum { AUDIO_SONG_SILENT, AUDIO_SONG_MENU, AUDIO_SONG_CHASE } AudioSong;
typedef enum {
    AUDIO_EFFECT_NONE, AUDIO_EFFECT_MOVE, AUDIO_EFFECT_CONFIRM, AUDIO_EFFECT_START,
    AUDIO_EFFECT_JUMP, AUDIO_EFFECT_COLLISION, AUDIO_EFFECT_DAMAGE, AUDIO_EFFECT_CAPTURE,
    AUDIO_EFFECT_CLEAR, AUDIO_EFFECT_TIMEOUT, AUDIO_EFFECT_LOSS, AUDIO_EFFECT_VICTORY,
    AUDIO_EFFECT_COUNT
} AudioEffect;
typedef struct { GameState state; uint16_t stage, events; } AudioContext;
typedef struct { uint16_t frequency[3], attenuation[4]; } AudioFrame;
typedef struct {
    uint16_t refresh_rate, remainder, stage, music_step, music_tick, effect_tick;
    uint16_t paused, context_valid;
    GameState previous_state;
    AudioSong song;
    AudioEffect effect;
} AudioSequencer;
void audio_sequencer_init(AudioSequencer *audio, uint16_t refresh_rate);
void audio_sequencer_update(AudioSequencer *audio, AudioContext context, AudioFrame *output);
uint16_t audio_effect_duration(AudioEffect effect);
#endif
