# Retro Pursuit 1.0 — candidata de entrega

Campanha arcade original com três mapas, perseguição por andares, salto, escadas, obstáculos, vidas, pontuação, menus, pausa e trilha PSG. Os valores da campanha e controles estão no README e em gameplay.md.

## Usar

Abrir Retro-Pursuit-1.0.bin em emulador Mega Drive/Genesis. Na árvore de desenvolvimento, a mesma ROM está em out/rom.bin. A/START confirma o menu; direcional move/usa escadas, A pula e START pausa. O progresso existe apenas na sessão atual; não há SRAM.

Recompilar exige SGDK 1.90, Java e Make do SDK; testar exige GCC nativo. Python 3 verifica e empacota. O pacote inclui os fontes gerados necessários, sem precisar regenerar os gráficos. O SDK e o emulador não são distribuídos.

## Evidências e limites

Dez suítes host passaram. Dois builds limpos são comparados byte a byte por SHA-256; o recibo e os logs acompanham o pacote. validation.md contém os resultados concretos, checksums, medições e observações no Gens. SHA256SUMS.txt cobre cada arquivo entregue, exceto o próprio manifesto.

Elevadores e coletáveis previstos no PDF ainda não foram implementados. Audição no emulador, campanha humana completa, apresentação PAL e hardware real permanecem pendentes; requirements.md detalha a cobertura. Não há garantia medida de orçamento de CPU por quadro. Esta entrega permite revisar e jogar a campanha existente, sem declarar aceite integral do PDF.

## Origem dos recursos

Personagens, mapas, logotipo, efeitos e melodias foram criados para este projeto em código. Nenhum recurso da obra de referência foi incorporado. O boot e bibliotecas de suporte são do SGDK de Stephane Dallongeville; consultar SGDK-LICENSE.txt e os avisos preservados em src/boot/sega.s. Esta nota não concede uma licença nova ao código original do projeto.
