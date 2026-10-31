#ifndef RP_TEST_SUPPORT_H
#define RP_TEST_SUPPORT_H
#include "core/game.h"
/* Existing motor/session tests deliberately bypass the new startup menu. */
static inline void test_start_game(Game *game, uint16_t hz)
{
    game_init(game, hz);
    game_start_session(game);
}
static inline void test_start_game_with_rules(Game *game, uint16_t hz, const GameRules *rules)
{
    game_init_with_rules(game, hz, rules);
    game_start_session(game);
}
#endif
