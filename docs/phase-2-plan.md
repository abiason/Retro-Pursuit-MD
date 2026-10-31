# Fase 2 — personagem

Autorizada pelo usuário. Objetivo: personagem controlável no cenário de teste, sem antecipar escadas, colisões de mapa ou inimigo.

Arquivos: core/types.h, core/game.*, entities/player.*, systems/physics.*, systems/animation.*, platform/input.*, platform/video.*, platform/player_tiles.h, tests/test_foundation.c e tools/generate_player.py.

Estruturas: Player com posição, velocidade, orientação, contato com piso e animação; Game com acumulador de ticks e borda do botão A. Funções: player_init/update, physics_step, animation_update e video_render_player.

Dependências: C convencional para domínio; SGDK 1.90 para entrada, sprite, paleta, tiles e VBlank. Validação: build ROM, testes de física/entrada/animação e inspeção no Gens. Evidências e limitações em validation.md. Fase 3 exige nova aprovação.
