#ifndef RP_SCORE_H
#define RP_SCORE_H
#include "core/types.h"
typedef struct { uint16_t capture_points, time_points_per_second, level_bonus; } ScoreRules;
uint32_t score_add(uint32_t score, uint32_t points);
uint32_t score_capture(uint32_t score, uint16_t seconds_left, const ScoreRules *rules);
#endif
