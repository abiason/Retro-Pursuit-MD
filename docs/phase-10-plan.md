# Fase 10 — finalização técnica

Autorizada pelo usuário após a Fase 9. Objetivo: regressão, revisão de recursos, documentação consolidada e entrega reproduzível.

Arquivos: cabeçalho da ROM, tools/release.ps1, tools/package_release.py, README e documentação. O script de release executa testes, dois ciclos clean/release, verificação e comparação SHA-256. O empacotador entrega fontes e ROM, com manifesto conferido contra o ZIP.

Revisão: manter armazenamento estático, recursos const, limites de pools e índices; preservar caches de cenário, HUD e PSG, uma sincronização VBlank por quadro e compilação otimizada oficial do SDK. Medir seções do ELF; não confundir espaço livre com medição de pico de pilha ou desempenho.

Validar dez suítes host, dois builds limpos iguais, recursos gerados reproduzíveis, ROM em emulador e integridade do pacote. Registrar os requisitos sem implementação e a validação manual pendente. Não declarar conformidade integral enquanto essas lacunas permanecerem.
