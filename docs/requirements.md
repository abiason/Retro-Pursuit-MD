# Cobertura da especificação v1.0

Fonte: Especificacao_Retro_Pursuit_Mega_Drive_v1.0.pdf, 30/10/2026. Este registro compara a implementação com o documento; não altera os requisitos aprovados. Os planos de cada fase e validation.md preservam o histórico.

| Requisito | Implementação | Evidência / limite |
| --- | --- | --- |
| RF01 menu | Iniciar, instruções e créditos | test_ui; inspeção Gens |
| RF02 controle | Direcional, A, START; B/C reservados | entradas de domínio testadas; controle físico requer aceite manual |
| RF03 movimento | Aceleração, frenagem, gravidade, salto, escadas e sólidos | test_foundation, test_collision, test_world |
| RF04 perseguição | Patrulha, fuga, escadas e dificuldade crescente | test_enemy, test_stage |
| RF05 captura | Congela, anima, pontua e avança | test_session, test_stage; capturas controladas no harness |
| RF06 tempo | Limite por fase e perda de vida | test_score, test_session |
| RF07 vidas | Três vidas, dano, proteção e Game Over | test_obstacle, test_session |
| RF08 pontos | Captura, tempo e bônus configuráveis; saturação | test_score; **parcial: não há itens especiais coletáveis** |
| RF09 obstáculos | Estáticos, horizontais, verticais e saltáveis | test_obstacle; saltos sobre os 12 obstáculos em test_stage |
| RF10 progressão | Três mapas, velocidade, tempo, trajetos e obstáculos | test_stage; campanha visual no Gens |
| RNF01 regiões | Simulação inteira a 120 Hz; apresentação 50/60 Hz | paridade lógica nas suítes; PAL em emulador e desempenho por quadro não medidos |
| RNF02 recursos | Arrays/pools estáticos, limites explícitos, recursos const | revisão de código, medição do ELF; sem medição de pico de pilha |
| RNF03–05 módulos | Domínio C portátil, plataforma SGDK, interfaces .h/.c | dez suítes host; diagrama em architecture.md |
| RNF06 documentação | Regras, arquitetura, build e validação | docs/ e README |
| Áudio e vídeo | Temas/efeitos PSG, sprites, tiles, HUD e menus | test_audio; inspeção visual Gens; audição pendente |
| Finalização | Dois builds limpos idênticos, verificação de ROM e pacote | release-build.json, logs e SHA256SUMS.txt |

## Diferenças e pendências de aceite

- A visão geral menciona elevadores: **não implementados**. Travessia vertical ocorre por escadas. Obstáculos verticais causam dano e não transportam personagens.
- RF08 e a seção de colisões mencionam itens especiais/coletáveis: **não implementados**. Não há pool nem pontuação de coletáveis.
- O ambiente previsto era Linux/Make/Git. A execução ocorreu em Windows/PowerShell com Make do SGDK 1.90 e GCC nativo para testes. Não foi inicializado repositório Git nem exercitado build Linux.
- Recursos visuais e musicais próprios são definidos em código e cabeçalhos gerados, sem árvore res/ de arquivos externos. O boot vem do SGDK; sua licença acompanha a entrega.
- Não há medição de tempo de CPU por quadro, ensaio em hardware real, avaliação auditiva no emulador ou campanha humana completa registrada. A saída WAV é aproximação offline.

A entrega 1.0 é uma candidata técnica com essas pendências explícitas; não representa aceite integral de todos os itens do PDF. Completar elevadores/coletáveis exige uma etapa de implementação e regressão, e não foi incorporado silenciosamente à finalização.
