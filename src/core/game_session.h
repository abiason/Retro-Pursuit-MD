#ifndef RP_GAME_SESSION_H
#define RP_GAME_SESSION_H
#include "game_state.h"
#include "systems/timer.h"
#include "systems/score.h"
typedef struct {
    uint16_t time_limit, initial_lives, capture_duration;
    ScoreRules score;
    uint16_t invulnerability_duration;
} GameRules;
typedef struct {
    GameState state;
    GameTimer timer;
    uint32_t score;
    uint16_t lives, round, capture_ticks, invulnerability_ticks;
} GameSession;
extern const GameRules game_default_rules;
void session_init(GameSession *session, const GameRules *rules);
void session_start_attempt(GameSession *session, const GameRules *rules);
void session_tick(GameSession *session);
void session_capture(GameSession *session, const GameRules *rules);
uint16_t session_damage(GameSession *session, const GameRules *rules);
#endif
