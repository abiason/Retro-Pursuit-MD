# Fase 8 — conteúdo, progressão e dificuldade

Autorizada pelo usuário após a Fase 7. Objetivo: fases adicionais jogáveis, avanço consistente e dificuldade orientada a dados, preservando a primeira fase.

Novo módulo: world/stage.* com três StageConfig e limite máximo de dez fases. Mudanças: Game passa a carregar mapa, grafo, inimigo, obstáculos e tempo da fase; Video invalida cenário por mapa e aplica cores; HUD mostra fase atual/total e nome; estado CAMPAIGN_COMPLETE encerra a terceira captura.

Configurações: Main Atrium (90 s, 90 px/s, 3 obstáculos), East Gallery (80 s, 101,25 px/s, 4) e Security Hub (70 s, 112,5 px/s, 5). Escadas e degraus diferentes nos dois mapas novos; física do jogador mantida.

Validação: nove suítes C99, capacidade/índices, grafos e apoios, trajetos de quarenta segundos nas velocidades de cada inimigo e do jogador, saltos, campanha completa por capturas controladas, repetição da fase, reinício, parâmetros sobrescritos e PAL/NTSC. ROM de verificação visual separada com -Campaign para observar carregamentos e conclusão no Gens.

Não inclui novo áudio nem refinamento final de arte. Fase 9 exige nova autorização; evidências em validation.md.
