#ifndef RP_AUDIO_H
#define RP_AUDIO_H
#include "core/game.h"
void audio_init(uint16_t refresh_rate);
void audio_update(const Game *game);
#endif
