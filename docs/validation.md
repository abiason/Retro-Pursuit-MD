# Validação

## Registro da Fase 1

Data: 30/10/2026.

- GCC SGDK 6.3.0 e Make 3.81 executados com sucesso fora do sandbox.
- Build release concluído com SGDK 1.90, sem avisos na compilação final.
- Recompilação após make clean concluída; ROM de 524288 bytes.
- Cabeçalho Mega Drive, título e checksum XOR do SGDK 1.90 verificados por tools/verify_rom.py. O checksum próprio do SDK difere da soma convencional do Mega Drive; o verificador segue o algoritmo do sizebnd instalado.
- SHA-256: 3d1409f364ebba030deef8342dc380fd744e524b7c88cd2c01d564a6b26eb119.
- Testes do host compilados em C99 com -Wall -Wextra -Werror e executados: PASS.
- Os testes verificam 120 pixels em um segundo em PAL e NTSC e os quatro limites de movimento.
- Verificação visual e resposta ao controle em emulador: pendentes de conclusão; a automação apresentou falhas de cache de elementos e capturas no diálogo do Gens. Erros observados: "no screenshot targets found" e "element ... is not available in cached app state". A tentativa adicional de iniciar o emulador com a ROM por argumento não produziu uma janela de jogo verificável.
- Hardware real: não testado.

A compilação e os testes de domínio não comprovam, por si só, aceitação no emulador. Não iniciar a Fase 2 antes de sua aprovação explícita.


## Fase 2 — personagem

- Implementação autorizada pelo usuário após revisão da Fase 1.
- Build SGDK 1.90 concluído sem avisos; ROM de 524288 bytes, cabeçalho e checksum XOR verificados.
- SHA-256 da ROM: ecdeff1397e005bf36980643b2288b96e7923650ec7509ee94c6a083819d601f.
- Testes C99 com -Wall -Wextra -Werror: PASS para aceleração, frenagem, limites laterais, salto, aterrissagem, ausência de salto duplo, borda do botão A, faixa de quadros e equivalência PAL/NTSC em intervalos iguais.
- Gens KMod: ROM aberta; tela da Fase 2, sprite original, piso e identificação NTSC observados visualmente.
- Controle físico: não testado. O emulador principal está mapeado para joystick. Uma cópia local foi preparada com teclas sem alterar a configuração principal, mas os toques enviados pela automação não produziram deslocamento observável. Controle e salto permanecem sem confirmação em emulação.
- PAL no emulador: não confirmado; a automação não conseguiu selecionar o submenu fora dos limites da janela. Paridade da lógica comprovada nos testes do host.
- Hardware real: não testado.

Para aceite jogável, abrir out/rom.bin e verificar esquerda/direita, caminhada, salto com A, aterrissagem e novo salto após soltar A. A Fase 3 não foi iniciada.

## Fase 3 — cenário e colisões (31/10/2026)

Implementação autorizada pelo usuário após a entrega da Fase 2. Os registros acima descrevem as versões anteriores.

- Cenário constante com três andares, duas escadas, paredes, teto e dois degraus de teste; sem IA ou dano.
- Build final release com SGDK 1.90 concluído sem avisos; ROM de 524288 bytes.
- Cabeçalho e checksum XOR verificados por tools/verify_rom.py: PASS (D6B4).
- SHA-256: 115d9c0785a42bb389ac43fc20e6addcd9488682385b52d14136da78d1ac718b.
- tools/test.ps1: três suítes C99 com -Wall -Wextra -Werror concluídas com PASS.
- Regressão do personagem: aceleração, frenagem, salto, aterrissagem, borda do botão A, animação e paridade PAL/NTSC em cenário plano isolado.
- Colisões: AABB, contato tangente, retângulo vazio, parede mais próxima independentemente da ordem, travessia de superfície em alta velocidade, aterrissagem, teto e perda de apoio na borda.
- Cenário: subir e descer as duas escadas, parar no meio, impedir salto e saída lateral dentro do piso, sair lateralmente em espaço livre e ignorar cima longe de escadas.
- Percurso controlado por entradas de teste: início no piso inferior, primeira escada, salto sobre degrau e segunda escada até o terceiro andar, sem sobreposição de sólidos fora da travessia de escada.
- Paridade PAL/NTSC da navegação por escada verificada em intervalos iguais de simulação.
- Gens KMod: ROM aberta e tela da Fase 3 inspecionada, com personagem inicial, três andares, escadas amarelas, degraus e identificação NTSC.
- Resposta ao controle físico e percurso interativo no emulador: ainda não confirmados por automação. Verificação manual pendente; não confundir testes de domínio com teste do controle físico.
- PAL no emulador e hardware real: não testados nesta fase.

A Fase 4 não foi iniciada. Para aceite manual, seguir o percurso descrito em gameplay.md.


## Fase 4 — criminoso e IA (31/10/2026)

Implementação autorizada pelo usuário após a entrega da Fase 3. Os registros acima descrevem as versões anteriores.

- Grafo estático: dez nós e nove ligações, com limite explícito de dezesseis nós para busca BFS.
- Criminoso com estados IDLE, PATROL, ESCAPE, CLIMB e DESCEND, visual original, configuração estática e motor de física compartilhado.
- tools/test.ps1: quatro suítes C99 com -Wall -Wextra -Werror concluídas com PASS; regressões das fases anteriores preservadas.
- IA: 40 segundos simulados de patrulha com retorno aos dois extremos, passagem pelos três andares e estados de subida, descida, caminhada e pausa, sem sobreposição de sólidos fora das escadas.
- Navegação: próximos passos conhecidos, destino desconectado, índices inválidos, grafo acima da capacidade e inicialização inválida tratados.
- Fuga: percepção, deslocamento em ESCAPE e preferência por subir quando o jogador está ao lado na entrada da segunda escada. O teste confirma que permanece no andar superior, evitando retorno imediato em direção à ameaça.
- Integração: simulações de 30 segundos a 50 e 60 quadros por segundo resultam em posição, estado, destino e memória de ameaça iguais.
- Build SGDK 1.90 final concluído sem avisos; ROM de 524288 bytes.
- Cabeçalho e checksum XOR verificados: PASS (E3F0).
- SHA-256: b0f3f930e4b24ad38b0a42a8201f7e4cd94a9a1fc07281b93720138c13257bbc.
- Gens KMod: tela da Fase 4 e dois personagens inspecionados; criminoso observado no andar intermediário e posteriormente atravessando a escada em direção ao inferior, confirmando movimento autônomo em emulação NTSC.
- Reação ao jogador usando controle físico: não testada por automação. O comportamento de fuga foi exercitado no domínio C; conferir manualmente aproximando o jogador.
- PAL em emulador e hardware real: não testados nesta fase.

Captura, tempo, pontuação, vidas e vitória/derrota continuam reservados à Fase 5, que não foi iniciada.


## Fase 5 — mecânica principal (31/10/2026)

Implementação autorizada pelo usuário após a entrega da Fase 4. Os registros anteriores descrevem versões anteriores.

- Sessão com captura, timer, pontos, três vidas, resultado de tentativa e Game Over; regras padrão configuradas em game_default_rules.
- tools/test.ps1: seis suítes C99 com -Wall -Wextra -Werror concluídas com PASS; regressões de personagem, colisões, cenário e IA preservadas.
- Pontuação: captura, tempo restante e bônus de rodada; saturação em 32 bits e limites máximos de configuração verificados.
- Timer: arredondamento da exibição, 120 ticks por segundo, limite máximo de configuração e saturação em zero verificados.
- Captura: contato AABB em subpixels, borda tangente sem captura, pontos concedidos uma vez, posições/timer congelados e confirmação incapaz de pular a animação ou repetir o prêmio.
- Sessão: próxima rodada preserva pontos/vidas; tempo esgotado perde uma vida uma única vez; terceira falha produz Game Over; nova sessão restaura três vidas, zero pontos e rodada 1.
- Entrada: confirmação exige nova pressão de A e não provoca salto automático na nova tentativa.
- Prazo: tempo esgotado tem prioridade sobre captura no mesmo tick. Inimigo inativo não é capturado e contador de rodadas não sofre wrap em 65535.
- PAL/NTSC: fluxos de captura, falha e reinício testados nas duas regiões; simulação integrada de 20 segundos produz estado, vidas, pontos e timer iguais.
- Build SGDK 1.90 final concluído sem avisos; ROM de 524288 bytes.
- Cabeçalho e checksum XOR verificados: PASS (311C).
- SHA-256 da ROM final: 0f7dc973eb362e4c3d8febaf5015859b4325f8671af835e140ec652d802b3c98.
- Gens KMod NTSC: HUD com pontos, tempo, vidas e rodada inspecionado. Contagem regressiva observada chegando a zero; mensagem TIME UP - LIFE LOST e redução de três para duas vidas confirmadas com tentativa parada aguardando A.
- O corte no texto da região foi corrigido e a ROM final foi reaberta para conferência do alinhamento.
- Captura/ROUND CLEAR, confirmação com controle físico e Game Over no emulador: ainda precisam de conferência manual. Esses caminhos passaram nos testes do domínio C; não alegar validação completa em emulação.
- PAL em emulador e hardware real: não testados nesta fase.

As rodadas repetem o cenário e dificuldade atuais. Obstáculos com dano e invulnerabilidade pertencem à Fase 6, que não foi iniciada. Menus e pausa pertencem à Fase 7; novos mapas e dificuldade à Fase 8.


## Fase 6 — obstáculos, dano e proteção (31/10/2026)

Implementação autorizada pelo usuário após a entrega da Fase 5. Registros anteriores descrevem versões anteriores.

- Pool estático de até dezesseis obstáculos, com configurações const, validação de capacidade, dimensões, posição, limites e velocidade.
- Três tipos no andar superior: cone parado, carrinho horizontal e objeto vertical. Hitboxes configuráveis, centradas na base dos sprites.
- tools/test.ps1: sete suítes C99 com -Wall -Wextra -Werror concluídas com PASS. Regressões de personagem, colisões, cenário, IA, pontuação/tempo e sessão preservadas.
- Movimento: milhares de ticks dentro dos limites, reversão nos extremos, limites de amplitude zero e rejeição de configurações inválidas.
- Colisão varrida: contato estático, bordas tangentes, passagem sobre o obstáculo, travessia de hitbox fina sem sobreposição nos extremos, contato apenas em canto e objeto móvel atingindo jogador parado.
- Percursos de salto sem dano sobre os três tipos verificados em posições/fases de movimento controladas e alcançáveis. A geometria e a física anteriores do personagem foram preservadas.
- Dano: uma vida perdida, retorno ao início, pontos/rodada/tempo preservados e contato repetido ignorado durante a proteção.
- Proteção: expiração exata em ticks, novo dano aceito ao zerar, Game Over na última vida e ausência de underflow de vidas.
- Prioridades: prazo antes de dano, dano sem proteção antes de captura e captura válida sob proteção.
- Estados: obstáculos congelados durante captura e Game Over; reinício restaura posições e remove proteção. Segurar A durante dano não provoca salto automático no respawn.
- PAL/NTSC: vinte segundos de simulação nas duas regiões produzem vidas, proteção, timer e posições dos obstáculos iguais.
- Build SGDK 1.90 final concluído sem avisos; ROM de 524288 bytes.
- Cabeçalho e checksum XOR verificados: PASS (8FC7).
- SHA-256: 2ca661d8ad310392ce7e8a4e0d2b714e6bd5b7b83b02933475f9817220c6b6f3.
- Gens KMod NTSC: tela da Fase 6 inspecionada; cone e dois móveis visíveis no andar superior. Capturas sucessivas confirmaram deslocamento horizontal do carrinho e vertical do objeto.
- Na inspeção foi observado ROUND CLEAR com 2370 pontos, três vidas e 87 segundos restantes; criminoso oculto e cenário parado no resultado. A origem dos comandos de jogo não foi controlada pela automação, portanto registrar como resultado observado, não como roteiro de entradas reproduzido.
- Dano, respawn e efeito de invulnerabilidade com controle no emulador: ainda precisam de conferência manual. Esses caminhos passaram nos testes C.
- PAL em emulador e hardware real: não testados nesta fase.

Menus, instruções, créditos, pausa e fluxo completo de interface pertencem à Fase 7, que não foi iniciada.


## Fase 7 — interface e fluxo completo (31/10/2026)

Implementação autorizada pelo usuário após a Fase 6. Registros anteriores descrevem versões anteriores.

- ROM principal abre em TITLE com START GAME, INSTRUCTIONS e CREDITS. A/START confirma; cima/baixo navega uma vez por pressão.
- START pausa PLAYING e CAPTURING. Opções CONTINUE e MAIN MENU; START sempre retoma. Resultados mantêm A para continuar/reiniciar e acrescentam START para o título.
- tools/test.ps1: oito suítes C99 com -Wall -Wextra -Werror concluídas com PASS; regressões das fases anteriores preservadas. Os testes antigos passam a iniciar a sessão explicitamente via test_support.h; o boot com menu é coberto por test_ui.c.
- Menus: boot, imobilidade da simulação, instruções/créditos, retorno, navegação circular, índice inválido e botões mantidos verificados.
- Pausa: timer, pontos, vidas, rodada, proteção, contador de captura, acumulador fracionário, posição/velocidade/animação dos dois atores, estado/rota/memória da IA e posições/velocidades dos obstáculos preservados exatamente por dez segundos simulados em PAL e NTSC.
- Retomada: primeiro tick após continuar coincide com uma simulação contínua de referência; A de confirmação não vira salto e START mantido não pausa repetidamente.
- Captura pausada: animação/contagem e prêmio não avançam ou repetem durante pausa, e a conclusão ocorre após retomada.
- Saídas: retorno ao título da pausa, conclusão, tentativa falha e Game Over; nova sessão reinicia pontos/vidas/rodada e atores.
- Paridade: simulação com cinco segundos de pausa e seis de gameplay coincide com execução contínua equivalente e produz valores iguais em PAL/NTSC.
- Build SGDK 1.90 da ROM principal e da ROM visual concluídos sem avisos; ROM principal de 524288 bytes.
- Cabeçalho e checksum XOR da ROM principal verificados: PASS (7642).
- SHA-256: 53d24cbefa442742c4474e3617cdb60d91168b05e6fa1032ed0dddff84133242, preservado após o build do harness visual.
- Gens KMod NTSC: inspeção da ROM visual identificada VISUAL TEST confirmou título, instruções, créditos, partida, pausa, Game Over, conclusão, tempo esgotado e retorno ao título. Capturas consecutivas da pausa mostraram o mesmo timer, atores e obstáculos. Não foram observados restos de sprites ou textos das telas anteriores.
- A ROM visual usa entradas controladas para menus/início/pausa e estados sintéticos para resultados. Esses resultados não alegam perdas ou vitórias reais; as regras correspondentes são cobertas pelas suítes de domínio.
- Ao final, a ROM principal foi reaberta no Gens, confirmando o título normal sem marca VISUAL TEST.
- Controle físico, PAL no emulador e hardware real: não exercitados por roteiro automatizado nesta fase. O harness não lê o controle físico.

A Fase 8 (conteúdo, fases adicionais e dificuldade) não foi iniciada.


## Fase 8 — campanha e dificuldade (31/10/2026)

Implementação autorizada pelo usuário após a entrega da Fase 7. Registros anteriores descrevem versões anteriores.

- Campanha finita com três StageConfig const, capacidade máxima de dez entradas e índices validados antes de alterar o jogo.
- Main Atrium preserva dados originais; East Gallery e Security Hub acrescentam mapas/grafos próprios, outros trajetos de escada, degraus, cores e configurações de obstáculos.
- Progressão: 90/80/70 segundos, criminoso a 90/101,25/112,5 px/s, percepção crescente, pausas menores e 3/4/5 obstáculos.
- tools/test.ps1: nove suítes C99 com -Wall -Wextra -Werror concluídas com PASS. O teste de campanha foi ampliado e recompilado separadamente para conferir saltos sobre os doze obstáculos das três fases; PASS.
- Conteúdo: nós apoiados e livres de sólidos/ameaças, ligações de escada correspondentes aos mapas, conectividade de todos os pares e capacidade completa dos pools.
- Rotas: quarenta segundos simulados por mapa nas velocidades de cada inimigo e na velocidade do jogador, alcançando os três andares e ambos os extremos sem atravessar sólidos fora de escadas.
- Obstáculos: percursos de salto sem dano sobre cada configuração em posições/fases de movimento controladas e alcançáveis.
- Progressão integrada por capturas controladas em PAL e NTSC: preserva pontos e vidas, carrega a próxima fase, soma 6900 pontos com capturas imediatas e conclui a terceira em CAMPAIGN_COMPLETE. Nova sessão restaura fase 1, três vidas, zero pontos e 90 segundos.
- Falha por tempo repete a fase atual, reinicializando seu timer, grafo, inimigo e pool. Game Over volta à primeira fase ao reiniciar. Índices inválidos não alteram a sessão; regras de tempo explicitamente fornecidas continuam sobrescrevendo os valores da campanha.
- Paridade: vinte segundos de simulação por fase produzem estados, score, timer e posições de inimigo/obstáculos iguais em PAL e NTSC.
- Build SGDK 1.90 final da ROM principal concluído sem avisos; 524288 bytes.
- Cabeçalho e checksum XOR: PASS (980C).
- SHA-256 da ROM principal: a4bffcb991b146a119f4b06e0f4bdac39d8ac5474048034791096887f60a013b, preservado após os builds dos dois harnesses.
- Gens KMod NTSC: ROM CAMPAIGN TEST inspecionada nas três fases. Confirmadas mudanças de nome, STAGE atual/total, cores, escadas, degraus, inimigo e quantidade de obstáculos; sem elementos remanescentes do mapa anterior.
- Conclusão ALL STAGES CLEAR e retorno ao menu observados na ROM de teste. O harness controla posições para provocar capturas e usa o fluxo real de progressão; isso não equivale a uma campanha humana completa.
- Os modos visual de interface e campanha foram recompilados em sequência e identificados por suas strings exclusivas, verificando que a troca de modo atualiza o harness. Um aviso de variável não usada do harness foi corrigido antes do build final.
- A ROM principal foi reaberta no menu normal, sem marca de teste.
- Equilíbrio fino com jogadores reais, PAL em emulador e hardware real: ainda não avaliados nesta fase.

Áudio e refinamento gráfico pertencem à Fase 9, que não foi iniciada.


## Fase 9 — áudio e arte (31/10/2026)

Implementação autorizada após a Fase 8; registros anteriores são históricos.

- Dez suítes C99 com -Wall -Wextra -Werror: PASS. Áudio cobre eventos, duração, prioridades, limites de frequência/atenuação, contato sem repetição, pausa exata e paridade PAL/NTSC.
- Temas e efeitos PSG originais; oito poses por personagem, logotipo e detalhes de cenário gerados em código. Orçamento de recursos: 205 tiles, 6560 bytes, índices centralizados com limite em compilação.
- Builds SGDK 1.90 de produção e interface visual concluídos sem avisos. ROM: 524288 bytes; cabeçalho e checksum XOR PASS (95B9).
- SHA-256: e1e9788f7686a2cf821b469a0d47e49bf29d1edaa52b33e8dd4bea62f88980a5.
- Gens KMod NTSC, ROM VISUAL TEST: título com logotipo, instruções, créditos, partida com novos personagens e vitrines, pausa com painel e sprites ocultos, Game Over, conclusão e tempo esgotado observados. Não foram observadas sobreposições de tiles ou restos das telas anteriores. Resultados do harness são sintéticos, sem alegação de campanha humana.
- Prévia offline de 17 segundos produzida a partir das saídas do sequenciador: sem clipping (pico 13953), intervalo de pausa silencioso. É síntese aproximada, não gravação do PSG emulado.
- Avaliação auditiva no emulador, controle físico, apresentação PAL em emulador e hardware real permanecem pendentes. A paridade lógica PAL/NTSC foi testada.

A Fase 10 (finalização) não foi iniciada.


## Fase 10 — finalização técnica (31/10/2026)

Autorizada após a Fase 9. Entrega candidata 1.0; a cobertura integral do PDF continua com pendências descritas em requirements.md.

- Regressão final: dez suítes C99 com -Wall -Wextra -Werror executadas e aprovadas: foundation, collision, world, enemy, score, session, obstacle, ui, stage e audio.
- tools/release.ps1 executado com -SkipTests porque a regressão acima já havia sido concluída separadamente. O recibo registra tests_run=false para esse comando, sem negar a execução anterior.
- Dois ciclos completos clean/release com SGDK 1.90, GCC 6.3.0, Make 3.81 e Java: PASS, sem avisos/erros nos dois logs. ROMs de 524288 bytes com SHA-256 idêntico.
- Checksum XOR SGDK: E00A. SHA-256: 522a62215cb59fa30ab23af424d66e2eb803763c95ce483c99e72a0b96481064.
- Cabeçalho identifica RETRO PURSUIT 1.0 e não declara SRAM. Não há salvamento de progresso.
- Os três geradores gráficos reproduziram exatamente os quatro cabeçalhos existentes (jogador, inimigo, obstáculos e logotipo), sem alterações de bytes.
- size do ELF: text=470468, data=72, bss=3388 bytes. Dados estáticos somam 3460 bytes. A diferença até 64 KiB não constitui medição de RAM livre em execução: há suporte do SDK, pilha e possíveis reservas. Não foi medido pico de pilha.
- Revisão de src: nenhuma chamada malloc/calloc/realloc/free nem aritmética float/double; main contém uma chamada SYS_doVBlankProcess. Pools e BFS têm capacidade 16, campanha no máximo dez fases.
- Otimizações preservadas: recursos const, cenários redesenhados por mudança de mapa/tela, HUD e registradores PSG com cache, sprites por DMA, release oficial -O3/LTO. Não foi alegado ganho de desempenho ou orçamento de CPU por quadro sem medição.
- Gens KMod NTSC: ROM final reaberta após os builds, menu normal e logotipo observados, sem marca de teste. A inspeção detalhada de telas e arte permanece registrada na Fase 9; nesta fase as alterações da ROM são somente metadados do cabeçalho.
- Pacote: fontes, testes, docs, ROM final, dois logs e recibo; manifesto SHA-256 conferido contra cada entrada do ZIP, além da integridade CRC do arquivo. Não inclui executáveis de teste, SDK ou ROMs de harness.
- Limitações: elevadores e coletáveis ausentes; audição no emulador, controle físico, campanha humana completa, apresentação PAL em emulador, desempenho por quadro e hardware real ainda sem aceite final. Não foi exercitado build Linux.

A preparação técnica da entrega foi executada. Conformidade integral e aceite final permanecem condicionados às lacunas e verificações acima; não há declaração de conclusão integral da especificação.


## Atualização dos créditos — Biason X (31/10/2026)

Solicitação do usuário: incluir exatamente "A game by Biason X". Frase adicionada centralizada na tela de créditos; demais atribuições preservadas e linhas reespaçadas.

- Dois builds limpos idênticos após a alteração visual; ROM de 524288 bytes, checksum XOR 5796. SHA-256: 22e988e44f2bab8134490379447abc932f909ea88b2d02348a639d4b6eca553e.
- Sem alteração de lógica de domínio. Suítes não repetidas nesta alteração de texto; compilação e checksum conferidos. Pacote e manifesto atualizados; entrega anterior preservada com sufixo pre-Biason-X.


## Publicação de screenshots e pré-release

Capturas reais do Gens KMod NTSC adicionadas em docs/screenshots: título, partida no Main Atrium, créditos e pausa. Geradas pela ROM VISUAL TEST de interface; a marca é preservada e identificada no README. Não representam uma campanha humana. A mensagem "A game by Biason X" foi observada nos créditos.

Pré-release v1.0-rc1 preparada com a ROM candidata já validada e SHA256SUMS.txt. Não houve alteração de código ou bytes da ROM nesta publicação; permanecem válidas as evidências e pendências registradas acima.
