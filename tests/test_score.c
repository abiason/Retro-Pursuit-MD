#include <assert.h>
#include <stdio.h>
#include "core/game_session.h"

int main(void)
{
    const uint32_t maximum = (uint32_t)0xFFFFFFFFUL;
    const ScoreRules large = {65535, 65535, 65535};
    GameTimer timer;
    unsigned tick;
    assert(score_capture(0, 50, &game_default_rules.score) == 2000);
    assert(score_capture(300, 0, &game_default_rules.score) == 1800);
    assert(score_add(maximum - 10, 10) == maximum);
    assert(score_add(maximum - 10, 11) == maximum);
    assert(score_capture(maximum - 20, 65535, &large) == maximum);
    timer_init(&timer, 90);
    assert(timer_seconds(&timer) == 90);
    timer_tick(&timer);
    assert(timer_seconds(&timer) == 90);
    for (tick = 1; tick < 120; ++tick) timer_tick(&timer);
    assert(timer_seconds(&timer) == 89);
    for (tick = 120; tick < 90 * 120; ++tick) timer_tick(&timer);
    assert(timer_seconds(&timer) == 0 && !timer.ticks_remaining);
    timer_tick(&timer);
    assert(!timer.ticks_remaining);
    timer_init(&timer, 65535);
    assert(timer.ticks_remaining == (uint32_t)65535 * 120 && timer_seconds(&timer) == 65535);
    puts("PASS: capture/time/round bonuses, score saturation, timer rounding and zero boundary");
    return 0;
}
