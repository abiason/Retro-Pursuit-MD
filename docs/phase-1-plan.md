# Retro Pursuit — plano da Fase 1

Status: implementação autorizada pelo usuário e realizada; consultar validation.md para evidências e pendências de aceite.

## Objetivo

Gerar uma ROM Mega Drive com tela 320 x 224, identificação original e objeto simples controlado pelo direcional. Não incluir física, personagem animado, inimigo ou fases nesta etapa.

## Ambiente verificado em 30/10/2026

- Diretório do projeto inicialmente vazio.
- Windows / PowerShell; GCC nativo disponível via MSYS2.
- Variável SGDK aponta para C:/sgdk190. inc/genesis.h confirma SGDK 1.90.
- Existe também C:/sgdk200; não alterar a versão automaticamente.
- SGDK 1.90 contém makefile.gen, ferramentas e exemplos locais.
- C:/Gens_KMod_v0.7.3/gens.exe existe; emulação ainda não executada.
- Tentativas de executar gcc.exe e make.exe do SGDK 1.90 falharam com erro do Windows de realocação inválida de DLL do sistema. Portanto, toolchain ainda não validada e nenhuma compilação realizada.

## Arquivos e responsabilidades propostos

- Makefile: adaptar variáveis de caminho e incluir makefile.gen da instalação selecionada; saída out/rom.bin.
- src/main.c: inicialização e loop, com uma chamada SYS_doVBlankProcess por quadro.
- src/core/game.h e game.c: estado estático do objeto, atualização e limites da tela; movimento calibrado para PAL e NTSC.
- src/platform/input.h e input.c: leitura do controle 1 e tradução para estrutura de entrada.
- src/platform/video.h e video.c: configuração de vídeo, texto e tile original para objeto de teste.
- res/resources.res: reservado aos recursos desta etapa se necessários.
- README.md, docs/build.md e docs/architecture.md: comandos, controles, arquitetura, limitações e evidências de validação.
- .gitignore: produtos de compilação e temporários.

## Interfaces previstas

- InputState: direções pressionadas, sem tipos dependentes do SGDK na interface do domínio.
- Game: coordenadas e acumulador inteiro para movimento por região; armazenamento estático.
- input_init(), input_update(), input_get_state().
- video_init(), video_render_object().
- game_init(), game_update(), game_render().

APIs confirmadas nos headers locais: JOY_readJoypad, VDP_setScreenWidth320, VDP_loadTileData, VDP_setTileMapXY, SYS_doVBlankProcess e IS_PAL_SYSTEM. Conferir exemplos e contratos completos ao implementar.

## Build e dependências

Usar convenções oficiais do SGDK instalado, preservando a instalação. Investigar a falha das ferramentas antes de escolher alternativa compatível. O host Windows diverge do ambiente Linux previsto no documento; registrar a adaptação e manter o código portável. Não considerar GCC nativo um compilador de ROM 68000.

## Validação e aceite

1. Resolver e registrar execução da toolchain; obter versões reais dos executáveis.
2. Compilar release e verificar ROM gerada e cabeçalho Mega Drive.
3. Executar no Gens KMod, verificar inicialização, texto, controle e limites.
4. Conferir comportamento PAL/NTSC e registrar resultados efetivamente observados.
5. Entregar ROM, fontes e instruções reproduzíveis; solicitar autorização antes da Fase 2.

Não existe ROM nesta entrega de planejamento. A aprovação solicitada é para implementar a Fase 1 e investigar a toolchain, sem alterar requisitos ou instalações globais automaticamente.
