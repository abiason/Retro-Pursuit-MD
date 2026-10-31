#ifndef RP_TIMER_H
#define RP_TIMER_H
#include "core/types.h"
typedef struct { uint32_t ticks_remaining; } GameTimer;
void timer_init(GameTimer *timer, uint16_t seconds);
void timer_tick(GameTimer *timer);
uint16_t timer_seconds(const GameTimer *timer);
#endif
