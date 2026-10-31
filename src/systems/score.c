#include "score.h"
uint32_t score_add(uint32_t score, uint32_t points)
{
    const uint32_t maximum = (uint32_t)0xFFFFFFFFUL;
    return points > maximum - score ? maximum : score + points;
}
uint32_t score_capture(uint32_t score, uint16_t seconds_left, const ScoreRules *rules)
{
    score = score_add(score, rules->capture_points);
    score = score_add(score, (uint32_t)seconds_left * rules->time_points_per_second);
    return score_add(score, rules->level_bonus);
}
