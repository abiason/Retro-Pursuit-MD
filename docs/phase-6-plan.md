# Fase 6 — obstáculos, dano e proteção

Autorizada pelo usuário após a Fase 5. Objetivo: obstáculos estáticos/móveis com colisão correta, perda de vida e invulnerabilidade temporária.

Novos arquivos: entities/obstacle.*, platform/obstacle_tiles.h, tools/generate_obstacles.py e tests/test_obstacle.c. Alterações: Game, GameRules, GameSession, Video, HUD e runner de testes.

Estruturas: ObstacleConfig, Obstacle, ObstaclePool; duração e contador de invulnerabilidade na sessão. Funções: obstacles_init/step/hit e session_damage. Pool máximo de dezesseis objetos, dados const e lógica C sem dependência do SGDK.

Os três tipos atuais são cone estático, carrinho horizontal e objeto vertical. Foram dispostos no andar superior, com hitboxes ajustáveis na base dos desenhos, para oferecer espaço de salto preservando a física anterior. Dano retorna ao início com proteção de dois segundos; na última vida, causa Game Over.

Validação: sete suítes C99, capacidade/limites, reversão e pausa dos objetos, AABB varrida, saltos sobre os três tipos, dano único, expiração exata da proteção, prioridades, reinício e paridade PAL/NTSC por vinte segundos. Build SGDK e inspeção visual no Gens. Evidências em validation.md.

Fase 7 exige nova autorização.
