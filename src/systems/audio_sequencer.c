#include "audio_sequencer.h"
/* Original scores. Note indices start at C3; 255 is a rest. */
static const uint16_t frequencies[] = {
    131,139,147,156,165,175,185,196,208,220,233,247,
    262,277,294,311,330,349,370,392,415,440,466,494,
    523,554,587,622,659,698,740,784,831,880,932,988,
    1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093
};
static const uint16_t menu_lead[32] = {
    24,27,31,34,31,27,26,255,29,32,36,39,38,36,34,31,
    24,26,27,31,29,27,26,22,19,22,26,29,27,26,24,255
};
static const uint16_t chase_lead[32] = {
    24,19,24,27,26,19,26,29,27,24,31,27,29,26,34,31,
    24,27,29,31,34,31,29,27,26,22,26,29,31,29,27,26
};
static const uint16_t bass[32] = {
    0,7,12,7,0,7,12,7,8,3,15,3,8,3,15,3,
    5,12,17,12,5,12,17,12,7,14,19,14,7,14,19,14
};
static const uint16_t duration[AUDIO_EFFECT_COUNT] = {0,8,16,32,24,10,40,60,72,60,120,120};
static const uint16_t priority[AUDIO_EFFECT_COUNT] = {0,1,1,3,2,2,6,7,8,8,10,11};
uint16_t audio_effect_duration(AudioEffect effect)
{
    return (uint16_t)effect < AUDIO_EFFECT_COUNT ? duration[effect] : 0;
}
void audio_sequencer_init(AudioSequencer *a, uint16_t refresh_rate)
{
    a->refresh_rate = refresh_rate == 50 ? 50 : 60;
    a->remainder = a->stage = a->music_step = a->music_tick = a->effect_tick = 0;
    a->paused = a->context_valid = 0;
    a->previous_state = GAME_STATE_TITLE;
    a->song = AUDIO_SONG_SILENT;
    a->effect = AUDIO_EFFECT_NONE;
}
static uint16_t step_duration(const AudioSequencer *a)
{
    if (a->song == AUDIO_SONG_MENU) return 24;
    return a->stage >= 3 ? 16 : a->stage == 2 ? 17 : 18;
}
static AudioEffect requested_effect(uint16_t events)
{
    if (events & GAME_EVENT_CAMPAIGN_CLEAR) return AUDIO_EFFECT_VICTORY;
    if (events & GAME_EVENT_GAME_OVER) return AUDIO_EFFECT_LOSS;
    if (events & GAME_EVENT_STAGE_CLEAR) return AUDIO_EFFECT_CLEAR;
    if (events & GAME_EVENT_TIMEOUT) return AUDIO_EFFECT_TIMEOUT;
    if (events & GAME_EVENT_CAPTURE) return AUDIO_EFFECT_CAPTURE;
    if (events & GAME_EVENT_DAMAGE) return AUDIO_EFFECT_DAMAGE;
    if (events & GAME_EVENT_STAGE_START) return AUDIO_EFFECT_START;
    if (events & GAME_EVENT_COLLISION) return AUDIO_EFFECT_COLLISION;
    if (events & GAME_EVENT_JUMP) return AUDIO_EFFECT_JUMP;
    if (events & GAME_EVENT_MENU_CONFIRM) return AUDIO_EFFECT_CONFIRM;
    if (events & GAME_EVENT_MENU_MOVE) return AUDIO_EFFECT_MOVE;
    return AUDIO_EFFECT_NONE;
}
static void sync_context(AudioSequencer *a, AudioContext context)
{
    AudioSong song;
    AudioEffect effect;
    uint16_t events = context.events;
    const uint16_t resumed = a->context_valid && a->previous_state == GAME_STATE_PAUSED;
    if (context.state == GAME_STATE_PAUSED) {
        a->paused = 1;
        a->previous_state = context.state;
        a->context_valid = 1;
        return;
    }
    a->paused = 0;
    song = context.state == GAME_STATE_PLAYING ? AUDIO_SONG_CHASE :
        (context.state == GAME_STATE_TITLE || context.state == GAME_STATE_INSTRUCTIONS ||
         context.state == GAME_STATE_CREDITS) ? AUDIO_SONG_MENU : AUDIO_SONG_SILENT;
    if (!resumed && (!a->context_valid || a->previous_state != context.state)) {
        if (context.state == GAME_STATE_CAPTURING) events |= GAME_EVENT_CAPTURE;
        if (context.state == GAME_STATE_LEVEL_COMPLETE) events |= GAME_EVENT_STAGE_CLEAR;
        if (context.state == GAME_STATE_CAMPAIGN_COMPLETE) events |= GAME_EVENT_CAMPAIGN_CLEAR;
        if (context.state == GAME_STATE_GAME_OVER) events |= GAME_EVENT_GAME_OVER;
        if (context.state == GAME_STATE_ATTEMPT_FAILED) events |= GAME_EVENT_TIMEOUT;
    }
    if (song != a->song || (song == AUDIO_SONG_CHASE && context.stage != a->stage)) {
        a->song = song;
        a->music_step = a->music_tick = 0;
        if (song != AUDIO_SONG_SILENT) { a->effect = AUDIO_EFFECT_NONE; a->effect_tick = 0; }
    }
    a->stage = context.stage;
    effect = requested_effect(events);
    if (effect && priority[effect] >= priority[a->effect]) {
        a->effect = effect; a->effect_tick = 0;
    }
    a->previous_state = context.state;
    a->context_valid = 1;
}
static void tick(AudioSequencer *a)
{
    if (a->song != AUDIO_SONG_SILENT && ++a->music_tick >= step_duration(a)) {
        a->music_tick = 0;
        a->music_step = (a->music_step + 1) % 32;
    }
    if (a->effect != AUDIO_EFFECT_NONE && ++a->effect_tick >= duration[a->effect]) {
        a->effect = AUDIO_EFFECT_NONE; a->effect_tick = 0;
    }
}
static void render_effect(const AudioSequencer *a, AudioFrame *out)
{
    static const uint16_t start_notes[] = {523,659,784,1047};
    static const uint16_t capture_notes[] = {523,659,784,1047,1319,1568};
    static const uint16_t clear_notes[] = {523,659,784,1047,0,1047};
    static const uint16_t timeout_notes[] = {523,440,349,262,196};
    static const uint16_t loss_notes[] = {392,349,311,262,196,131};
    static const uint16_t victory_notes[] = {523,659,784,1047,988,1047,1319,1568,1319,1047};
    const uint16_t age = a->effect_tick;
    uint16_t frequency = 0, attenuation = 5;
    switch (a->effect) {
    case AUDIO_EFFECT_MOVE: frequency = age < 4 ? 660 : 880; attenuation = 7; break;
    case AUDIO_EFFECT_CONFIRM: frequency = age < 8 ? 880 : 1175; break;
    case AUDIO_EFFECT_START: frequency = start_notes[age / 8]; attenuation = 6; break;
    case AUDIO_EFFECT_JUMP: frequency = 360 + age * 24; attenuation = 4 + age / 8; break;
    case AUDIO_EFFECT_COLLISION: frequency = age < 8 ? 180 - age * 9 : 110; attenuation = 7 + age / 2; break;
    case AUDIO_EFFECT_DAMAGE:
        frequency = 440 - age * 7; attenuation = 4 + age / 10;
        out->attenuation[3] = 4 + age / 6; break;
    case AUDIO_EFFECT_CAPTURE: frequency = capture_notes[age / 10]; attenuation = 4 + age / 20; break;
    case AUDIO_EFFECT_CLEAR: frequency = clear_notes[age / 12]; break;
    case AUDIO_EFFECT_TIMEOUT: frequency = timeout_notes[age / 12]; attenuation = 6; break;
    case AUDIO_EFFECT_LOSS:
        frequency = loss_notes[age / 20];
        if (age < 12) out->attenuation[3] = 6;
        break;
    case AUDIO_EFFECT_VICTORY: frequency = victory_notes[age / 12]; attenuation = 4; break;
    default: break;
    }
    if (frequency && frequency < 110) frequency = 110;
    out->frequency[2] = frequency;
    out->attenuation[2] = frequency ? attenuation : 15;
    if (a->effect && priority[a->effect] >= 2) {
        if (out->attenuation[0] < 12) out->attenuation[0] += 3;
        if (out->attenuation[1] < 13) out->attenuation[1] += 2;
    }
}
static void render(const AudioSequencer *a, AudioFrame *out)
{
    uint16_t channel, note, transpose = 0;
    for (channel = 0; channel < 3; ++channel) out->frequency[channel] = 0;
    for (channel = 0; channel < 4; ++channel) out->attenuation[channel] = 15;
    if (a->paused) return;
    if (a->song != AUDIO_SONG_SILENT) {
        if (a->song == AUDIO_SONG_CHASE) transpose = a->stage >= 3 ? 5 : a->stage == 2 ? 2 : 0;
        note = a->song == AUDIO_SONG_MENU ? menu_lead[a->music_step] : chase_lead[a->music_step];
        if (note != 255) {
            out->frequency[0] = frequencies[note + transpose];
            out->attenuation[0] = (a->song == AUDIO_SONG_MENU ? 7 : 6) + a->music_tick / 8;
        }
        out->frequency[1] = frequencies[bass[a->music_step] + transpose];
        out->attenuation[1] = 10 + a->music_tick / 10;
        if (a->song == AUDIO_SONG_CHASE && a->music_step % 4 == 0 && a->music_tick < 3)
            out->attenuation[3] = 13;
    }
    render_effect(a, out);
}
void audio_sequencer_update(AudioSequencer *a, AudioContext context, AudioFrame *output)
{
    sync_context(a, context);
    if (!a->paused) {
        a->remainder += 120;
        while (a->remainder >= a->refresh_rate) {
            a->remainder -= a->refresh_rate;
            tick(a);
        }
    }
    render(a, output);
}
