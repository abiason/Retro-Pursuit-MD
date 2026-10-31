# Fase 4 — criminoso e IA

Autorizada pelo usuário após a entrega da Fase 3. Objetivo: inimigo com patrulha, fuga e navegação vertical em trajetos válidos, sem implementar captura ou pontuação.

Novos arquivos: entities/enemy.*, world/navigation.*, platform/enemy_tiles.h e tests/test_enemy.c. Alterações: Game, motor Physics, renderização, gerador de tiles, runner de testes e documentação.

Estruturas: EnemyState, EnemyConfig, Enemy, NavigationNode, NavigationEdge e NavigationGraph. Funções centrais: enemy_init/update, navigation_next_hop e physics_step_with_speed. Dados e armazenamento estáticos; sem novas dependências externas.

Validação: regressões das fases anteriores, grafo e limites, 40 segundos de patrulha, três andares, duas escadas, fuga vertical, igualdade PAL/NTSC por 30 segundos, build SGDK e observação no Gens.

A Fase 5 exige nova autorização. Evidências e limites em validation.md.
