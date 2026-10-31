#include "timer.h"
enum { TIMER_TICKS_PER_SECOND = 120 };
void timer_init(GameTimer *timer, uint16_t seconds)
{
    timer->ticks_remaining = (uint32_t)seconds * TIMER_TICKS_PER_SECOND;
}
void timer_tick(GameTimer *timer)
{
    if (timer->ticks_remaining) --timer->ticks_remaining;
}
uint16_t timer_seconds(const GameTimer *timer)
{
    return (uint16_t)((timer->ticks_remaining + TIMER_TICKS_PER_SECOND - 1) / TIMER_TICKS_PER_SECOND);
}
