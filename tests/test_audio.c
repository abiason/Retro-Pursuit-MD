#include <assert.h>
#include <stdio.h>
#include "core/game.h"
#include "systems/audio_sequencer.h"
#include "systems/animation.h"

static void bounds(const AudioFrame *frame)
{
    unsigned i;
    for (i = 0; i < 3; ++i)
        assert(!frame->frequency[i] || (frame->frequency[i] >= 110 && frame->frequency[i] <= 4095));
    for (i = 0; i < 4; ++i) assert(frame->attenuation[i] <= 15);
}
static void equal(const AudioSequencer *a, const AudioSequencer *b)
{
    assert(a->song == b->song && a->stage == b->stage && a->remainder == b->remainder);
    assert(a->music_step == b->music_step && a->music_tick == b->music_tick);
    assert(a->effect == b->effect && a->effect_tick == b->effect_tick);
}
static AudioSequencer simulate(uint16_t hz, uint16_t stage, GameState state)
{
    AudioSequencer audio;
    AudioFrame frame;
    AudioContext context = {state, stage, 0};
    unsigned i;
    audio_sequencer_init(&audio, hz);
    for (i = 0; i < hz * 10U; ++i) {
        context.events = i % (hz * 2U) == 0 ? GAME_EVENT_JUMP : 0;
        audio_sequencer_update(&audio, context, &frame);
        bounds(&frame);
    }
    return audio;
}
static void effects_and_priority(void)
{
    const uint16_t events[] = {GAME_EVENT_MENU_MOVE, GAME_EVENT_MENU_CONFIRM, GAME_EVENT_STAGE_START,
        GAME_EVENT_JUMP, GAME_EVENT_COLLISION, GAME_EVENT_DAMAGE, GAME_EVENT_CAPTURE,
        GAME_EVENT_STAGE_CLEAR, GAME_EVENT_TIMEOUT, GAME_EVENT_GAME_OVER, GAME_EVENT_CAMPAIGN_CLEAR};
    unsigned i, tick;
    AudioSequencer audio;
    AudioFrame frame;
    AudioContext context = {GAME_STATE_PLAYING, 1, 0};
    for (i = 0; i < sizeof(events)/sizeof(events[0]); ++i) {
        audio_sequencer_init(&audio, 60);
        context.events = events[i];
        audio_sequencer_update(&audio, context, &frame);
        assert(audio.effect == (AudioEffect)(i + 1));
        context.events = 0;
        for (tick = 0; tick < 70; ++tick) {
            audio_sequencer_update(&audio, context, &frame);
            bounds(&frame);
        }
        assert(audio.effect == AUDIO_EFFECT_NONE);
    }
    audio_sequencer_init(&audio, 60);
    context.events = GAME_EVENT_DAMAGE | GAME_EVENT_JUMP;
    audio_sequencer_update(&audio, context, &frame);
    assert(audio.effect == AUDIO_EFFECT_DAMAGE);
    context.events = GAME_EVENT_MENU_MOVE;
    audio_sequencer_update(&audio, context, &frame);
    assert(audio.effect == AUDIO_EFFECT_DAMAGE && audio.effect_tick == 4);
    context.events = GAME_EVENT_CAPTURE;
    audio_sequencer_update(&audio, context, &frame);
    assert(audio.effect == AUDIO_EFFECT_CAPTURE);
    context.events = GAME_EVENT_GAME_OVER;
    audio_sequencer_update(&audio, context, &frame);
    assert(audio.effect == AUDIO_EFFECT_LOSS);
    assert(audio_effect_duration((AudioEffect)99) == 0);
    assert(audio_effect_duration((AudioEffect)-1) == 0);
}
static void pause(uint16_t hz, GameState state)
{
    AudioSequencer audio, frozen, reference;
    AudioFrame frame, expected;
    AudioContext context = {state, 2, GAME_EVENT_CAPTURE};
    unsigned i, channel;
    audio_sequencer_init(&audio, hz);
    for (i = 0; i < 7; ++i) {
        audio_sequencer_update(&audio, context, &frame);
        context.events = 0;
    }
    frozen = reference = audio;
    context.state = GAME_STATE_PAUSED;
    for (i = 0; i < hz * 5U; ++i) {
        audio_sequencer_update(&audio, context, &frame);
        equal(&audio, &frozen);
        for (channel = 0; channel < 4; ++channel) assert(frame.attenuation[channel] == 15);
    }
    context.state = state;
    audio_sequencer_update(&audio, context, &frame);
    audio_sequencer_update(&reference, context, &expected);
    equal(&audio, &reference);
    for (channel = 0; channel < 3; ++channel) assert(frame.frequency[channel] == expected.frequency[channel]);
}
static void events(void)
{
    Game game;
    const InputState idle = {0,0,0,0}, jump = {0,1,0,0}, down = {0,0,1,0}, left = {-1,0,0,0};
    unsigned i;
    uint16_t landed = 0;
    game_init(&game, 60);
    assert(!game.events);
    game_update(&game, down); assert(game.events & GAME_EVENT_MENU_MOVE);
    game_update(&game, down); assert(!game.events);
    game_update(&game, jump); assert(game.events & GAME_EVENT_MENU_CONFIRM);
    game_init(&game, 60); game_start_session(&game);
    game_update(&game, jump); assert(game.events & GAME_EVENT_JUMP);
    game_update(&game, jump); assert(!(game.events & GAME_EVENT_JUMP));
    for (i = 0; i < 60; ++i) {
        game_update(&game, idle);
        if (game.events & GAME_EVENT_COLLISION) landed = 1;
    }
    assert(landed);
    game.player.x = 16 * POSITION_ONE;
    game_update(&game, left); assert(game.events & GAME_EVENT_COLLISION);
    game_update(&game, left); assert(!(game.events & GAME_EVENT_COLLISION));
    game_init(&game, 60); game_start_session(&game);
    game.enemy.body.x = game.player.x; game.enemy.body.y = game.player.y;
    game_update(&game, idle); assert(game.events & GAME_EVENT_CAPTURE);
    assert(game.player.frame == PLAYER_FRAME_CAPTURE);
    while (game.session.state == GAME_STATE_CAPTURING) game_update(&game, idle);
    assert(game.events & GAME_EVENT_STAGE_CLEAR);
    game_update(&game, idle); assert(!game.events);
    game_init(&game, 60); game_start_session(&game);
    game.enemy.active = 0; game.session.lives = 1;
    game.player.x = 112 * POSITION_ONE; game.player.y = 72 * POSITION_ONE;
    game_update(&game, idle);
    assert((game.events & (GAME_EVENT_DAMAGE | GAME_EVENT_GAME_OVER)) ==
        (GAME_EVENT_DAMAGE | GAME_EVENT_GAME_OVER));
    game_init(&game, 60); game_start_session(&game);
    game.session.timer.ticks_remaining = 1;
    game_update(&game, idle); assert(game.events & GAME_EVENT_TIMEOUT);
}
int main(void)
{
    AudioSequencer pal, ntsc;
    unsigned stage;
    effects_and_priority(); events();
    pause(50, GAME_STATE_PLAYING); pause(60, GAME_STATE_PLAYING);
    pause(50, GAME_STATE_CAPTURING); pause(60, GAME_STATE_CAPTURING);
    pal = simulate(50, 1, GAME_STATE_TITLE); ntsc = simulate(60, 1, GAME_STATE_TITLE);
    equal(&pal, &ntsc);
    for (stage = 1; stage <= 3; ++stage) {
        pal = simulate(50, (uint16_t)stage, GAME_STATE_PLAYING);
        ntsc = simulate(60, (uint16_t)stage, GAME_STATE_PLAYING);
        equal(&pal, &ntsc);
    }
    assert(PLAYER_FRAME_COUNT == 8);
    puts("PASS: original scores, effect bounds/priority, game events, no wall spam, pause/resume, PAL/NTSC");
    return 0;
}
