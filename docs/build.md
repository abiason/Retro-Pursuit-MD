# Build

## Ambiente verificado

Windows / PowerShell, SGDK 1.90 em C:/sgdk190, GCC 6.3.0 para 68000, GNU Make 3.81 e Java no PATH. O GCC de C:/msys64/ucrt64/bin serve aos testes do host, não à ROM.

O sandbox do agente impediu a inicialização de executáveis do SGDK, retornando erro de realocação inválida de DLL. As mesmas ferramentas executaram corretamente fora do isolamento. Não houve alteração de DLLs, instalação do SDK ou configuração de segurança do Windows.

## Comandos no PowerShell

```powershell
& C:\sgdk190\bin\make.exe -f Makefile release
& C:\sgdk190\bin\make.exe -f Makefile clean
& C:\sgdk190\bin\make.exe -f Makefile release
```

Makefile delega compilação, link e checksum ao makefile.gen oficial e habilita inclusão dos arquivos de dependência gerados. O alvo padrão é release. O SGDK gera out/rom.bin, out/rom.out e out/symbol.txt. O build alinha a ROM em blocos de 128 KiB.

Com Python 3 disponível, executar `python tools/verify_rom.py` para verificar cabeçalho e checksum XOR próprio do SGDK 1.90. Esta verificação não usa a soma convencional do Mega Drive: segue o algoritmo da ferramenta sizebnd instalada.

## Testes de domínio

Executar `.\tools\test.ps1` no PowerShell com GCC nativo disponível. No Linux, compilar cada tests/test_*.c separadamente junto a src/core/game.c, src/entities/player.c, src/systems/physics.c, src/systems/animation.c, src/systems/collision.c, src/world/level.c, src/world/navigation.c, src/entities/enemy.c, src/core/game_session.c, src/systems/score.c, src/systems/timer.c, src/entities/obstacle.c, src/core/scene_manager.c src/world/stage.c e src/systems/audio_sequencer.c, com -std=c99 -Wall -Wextra -Werror -Isrc.

## Validação em emulador

Abrir out/rom.bin e conferir três andares, duas escadas, jogador e criminoso roxo em movimento. Observar patrulha, troca de andares e reação à aproximação do jogador. Testar esquerda/direita, pulo com A e cima/baixo nas escadas, paredes, tetos e degraus. Selecionar PAL e NTSC e reiniciar para conferir região e velocidade. Hardware real ainda é necessário para compatibilidade final.


Na Fase 5, conferir também o HUD, captura e ROUND CLEAR, perda de vida por tempo esgotado e reinício com A. A tentativa padrão dura 90 segundos. Os testes de sessão usam limites curtos configuráveis, sem alterar o tempo da ROM final.


Na Fase 6, observar os três obstáculos no andar superior, seus movimentos e colisões. Testar salto com tempo favorável, perda de vida/respawn, jogador piscando por dois segundos, dano ignorado durante a proteção e Game Over no último impacto. O criminoso não recebe dano dos obstáculos.


## Verificação visual da Fase 7

`tools/build-ui-smoke.ps1` copia somente as fontes para out/ui-smoke, substitui o main nessa cópia por tests/rom_ui_smoke.c e compila com o SDK instalado. Pode receber -Sdk com outro caminho. Não modifica src/main.c nem out/rom.bin.

Abrir out/00-ui-smoke.bin no emulador. A cada seis segundos, percorre título, instruções, créditos, partida, pausa, Game Over, conclusão, tempo esgotado e retorno ao título, repetindo o ciclo. O rodapé VISUAL TEST distingue esse artefato. Início, navegação e pausa recebem entradas controladas; as telas de resultado usam estado sintético para inspeção de apresentação, não para alegar validação da regra de jogo.

A ROM principal out/rom.bin abre no menu e usa o controle real. Conferir START/A, navegação, ida/volta das páginas e pausa; na pausa, verificar que tempo e objetos não mudam e que a continuação preserva o progresso. PAL e hardware real permanecem verificações separadas.


## Campanha — Fase 8

`tools/build-ui-smoke.ps1 -Campaign` compila out/00-campaign-smoke.bin separadamente. A cada seis segundos, mostra fase 1, avança por captura controlada à fase 2, avança à fase 3, provoca a captura final e retorna ao menu, repetindo o ciclo. O rodapé CAMPAIGN TEST distingue o artefato. As posições do criminoso são controladas para exercitar a transição; isso não representa uma campanha humana completa.

Conferir troca de cores, escadas, degraus, nomes, contagem de obstáculos e limites de tempo, além de ALL STAGES CLEAR. A ROM principal out/rom.bin mantém o menu normal e só avança após capturas durante a partida. Os dois modos do harness compartilham out/ui-smoke como área temporária e nunca substituem a ROM principal.


## Recursos e prévia de áudio

Regenerar os cabeçalhos com `python tools/generate_player.py` e `python tools/generate_logo.py`, depois recompilar. Recursos são originais e definidos em código; não exigem downloads externos.

Prévia aproximada offline, com GCC nativo e Python 3:

```powershell
gcc -std=c99 -Wall -Wextra -Werror -Isrc tools/audio_dump.c src/systems/audio_sequencer.c -o out/audio_dump.exe
& .\out\audio_dump.exe | Set-Content out/audio_frames.txt -Encoding ascii
python tools/render_audio_preview.py
```

Gera `out/audio-preview.wav`: 17 segundos, mono, 44100 Hz, 16 bits, a partir das saídas reais do sequenciador. É uma síntese aproximada de ondas quadradas e ruído, não uma gravação do emulador; timbre e volume precisam de avaliação auditiva no emulador/hardware.


## Entrega 1.0 reproduzível

```powershell
.\tools\release.ps1 -Sdk C:/sgdk190 -Python python
python tools/package_release.py
```

O primeiro comando executa as dez suítes, faz dois builds limpos e exige SHA-256 idêntico. `-SkipTests` permite usar uma regressão já executada separadamente; o recibo registra essa opção. O segundo cria dist/Retro-Pursuit-1.0 e seu ZIP, incluindo ROM, fontes, testes, documentação, logs dos builds e manifesto SHA256SUMS.txt. O SDK e os emuladores não são incluídos. O empacotador recusa sobrescrever uma entrega existente.

O determinismo foi verificado no ambiente Windows/SGDK 1.90 registrado. Outras versões de compilador ou SDK podem produzir bytes diferentes. A ROM da raiz do pacote chama-se Retro-Pursuit-1.0.bin; recompilar cria out/rom.bin. O jogo não usa SRAM nem salva progresso.
