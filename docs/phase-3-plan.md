# Fase 3 — cenário e colisões

Autorizada pelo usuário em 31/10/2026. Objetivo: percurso com pisos, paredes, plataformas e escadas sem atravessar sólidos fora da travessia válida de escada.

Arquivos novos: world/level.*, systems/collision.*, tests/test_collision.c, tests/test_world.c, tools/test.ps1. Alterações: Game, Player, Physics, Animation, Input, Video, cabeçalho da ROM e documentação.

Estruturas: WorldRect para AABB, Ladder para gatilho vertical, LevelConfig com dados const e Player com índice de escada. Funções centrais: collision_aabb, collision_move, collision_player_overlaps; física recebe a fase explicitamente.

Dependências: domínio C convencional; adaptadores SGDK 1.90 para controle e tiles. Validação: regressão do personagem, testes de superfícies/bordas e percurso completo de três andares, paridade PAL/NTSC, build da ROM e inspeção visual no Gens.

A Fase 4 depende de autorização explícita. Resultados e limites em validation.md.
