# Fase 7 — interface e fluxo de telas

Autorizada pelo usuário após a Fase 6. Objetivo: menu principal, instruções, créditos, pausa, Game Over e conclusão com fluxo completo pelo controle.

Novos módulos: core/scene_manager.* e ui/menu.*. Alterações: GameState, Game, Input, Video e HUD. Estruturas: SceneManager com seleções e estado suspenso; InputState com START. Funções principais: game_start_session, game_show_title, scene_selection_step e menu_render_frontend/pause.

Validação: oito suítes C99; navegação, botões mantidos, pausa exata de toda a simulação, retomada sem salto, captura pausada, saídas de resultados, sessão nova e PAL/NTSC. Harness tests/rom_ui_smoke.c e tools/build-ui-smoke.ps1 geram ROM separada para inspeção de todas as telas no Gens.

A ROM jogável inicia no título e não contém o harness. Menus e resultados não acrescentam fases ou dificuldade; a Fase 8 exige nova autorização. Evidências em validation.md.
