# Fase 5 — mecânica principal

Autorizada pelo usuário após a Fase 4. Objetivo: uma tentativa pode ser vencida por captura ou perdida por tempo esgotado, com pontos, vidas, avanço e reinício.

Novos módulos: core/game_state.h, core/game_session.*, systems/timer.*, systems/score.*, ui/hud.*. Novos testes: test_score.c e test_session.c. Integração em Game, Collision, Video e no runner de testes.

Estruturas: GameRules, GameSession, GameTimer e ScoreRules. Funções: session_init/start_attempt/tick/capture, timer_tick, score_capture e game_init_with_rules. Dependências: C convencional no domínio e SGDK apenas na apresentação.

Validação: seis suítes C99, captura única, timer congelado, bônus/saturação, vidas, falha, Game Over, confirmação por borda, reinício e paridade PAL/NTSC; build SGDK e inspeção no Gens. Regras e limites em gameplay.md e validation.md.

Fase 6 exige nova autorização. Novos mapas e dificuldade continuam reservados à Fase 8.
