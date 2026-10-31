#include "game_session.h"
const GameRules game_default_rules = {90, 3, 60, {1000, 10, 500}, 240};
void session_start_attempt(GameSession *session, const GameRules *rules)
{
    timer_init(&session->timer, rules->time_limit);
    session->capture_ticks = 0;
    session->invulnerability_ticks = 0;
    session->state = GAME_STATE_PLAYING;
}
void session_init(GameSession *session, const GameRules *rules)
{
    session->score = 0;
    session->lives = rules->initial_lives;
    session->round = 1;
    session_start_attempt(session, rules);
}
void session_tick(GameSession *session)
{
    if (session->state == GAME_STATE_PLAYING) {
        if (session->invulnerability_ticks) --session->invulnerability_ticks;
        timer_tick(&session->timer);
        if (!session->timer.ticks_remaining) {
            if (session->lives) --session->lives;
            session->state = session->lives ? GAME_STATE_ATTEMPT_FAILED : GAME_STATE_GAME_OVER;
        }
    } else if (session->state == GAME_STATE_CAPTURING) {
        if (session->capture_ticks) --session->capture_ticks;
        if (!session->capture_ticks) session->state = GAME_STATE_LEVEL_COMPLETE;
    }
}
void session_capture(GameSession *session, const GameRules *rules)
{
    if (session->state != GAME_STATE_PLAYING || !session->timer.ticks_remaining) return;
    session->score = score_capture(session->score, timer_seconds(&session->timer), &rules->score);
    session->capture_ticks = rules->capture_duration;
    session->state = GAME_STATE_CAPTURING;
}

uint16_t session_damage(GameSession *session, const GameRules *rules)
{
    if (session->state != GAME_STATE_PLAYING || !session->timer.ticks_remaining ||
        session->invulnerability_ticks || !session->lives) return 0;
    --session->lives;
    if (!session->lives) session->state = GAME_STATE_GAME_OVER;
    else session->invulnerability_ticks = rules->invulnerability_duration;
    return 1;
}
