# Regras — Fase 8

A ROM abre no menu principal. Cima/baixo seleciona START GAME, INSTRUCTIONS ou CREDITS; A ou START confirma. Nas páginas auxiliares, A ou START retorna ao menu. Cada pressão do direcional muda uma opção; segurar não repete a navegação.

Capture o criminoso roxo antes que o tempo termine. A campanha começa com três vidas e a primeira fase. Os limites de tempo são 90, 80 e 70 segundos nas fases 1, 2 e 3.

## Movimento

- Esquerda/direita para andar; A para pular com os pés apoiados.
- Cima/baixo, alinhado à escada amarela, para mudar de andar.
- Soltar cima/baixo para parar na escada. Esquerda/direita permite sair apenas com o corpo fora do piso.
- Não há salto duplo ou salto no meio da escada. Segurar A não repete o salto.

Paredes, tetos, pisos e degraus bloqueiam o personagem. Somente escadas permitem atravessar pisos pelo seu eixo. Suba a primeira escada para o andar intermediário, salte o degrau e use a segunda para chegar ao superior.

## Captura e pontos

O contato com sobreposição entre os corpos captura o criminoso; apenas encostar nas bordas não basta. A captura para os personagens e o relógio, exibe uma breve animação e concede uma vez:

- 1000 pontos pela captura;
- 10 pontos por segundo restante mostrado no relógio;
- 500 pontos pela fase concluída.

Se o relógio chegar a zero no mesmo tick do contato, vale o tempo esgotado. Depois de STAGE CLEAR, solte e pressione A para avançar. Pontos e vidas são preservados, mas o próximo mapa tem novo tempo, rota e obstáculos. Após a terceira captura, ALL STAGES CLEAR encerra a campanha; A inicia outra sessão ou START retorna ao menu.

## Tempo esgotado e Game Over

Ao zerar o tempo, uma vida é perdida e a tentativa para. Pressione A novamente para tentar a mesma fase com tempo e posições reiniciados, preservando pontos. Ao perder a última vida, aparece GAME OVER; A inicia uma nova sessão com três vidas, zero pontos e fase 1.

Segurar A enquanto um resultado aparece não confirma automaticamente: é preciso soltar e pressionar de novo. O HUD informa pontos, tempo, vidas e rodada. START permite voltar ao menu a partir dos resultados.


## Obstáculos e dano

No andar superior há um cone parado, um carrinho que vai e volta e um objeto que sobe e desce. Os obstáculos são gatilhos de dano: não servem de plataforma e não bloqueiam fisicamente o jogador. Use salto e espere um momento favorável do movimento para passar. O objeto vertical também abre passagem por baixo quando está alto. O criminoso não recebe dano desses obstáculos.

Um impacto sem proteção tira uma vida e retorna o jogador ao ponto inicial. Pontos, fase e tempo restante são preservados; o relógio, o criminoso e os obstáculos continuam ativos. Por dois segundos, o jogador pisca e o HUD informa a proteção temporária; contatos nesse intervalo não retiram outra vida. A proteção permite capturar o criminoso normalmente.

Perder a última vida por impacto causa Game Over imediatamente. A inicia uma nova sessão. O tempo esgotado continua tendo prioridade sobre dano no mesmo tick; dano sem proteção tem prioridade sobre uma captura simultânea.

Novas tentativas ou fases restauram as posições iniciais dos obstáculos e removem a proteção. As áreas perigosas são menores que os desenhos, centradas na base, para permitir saltos com margem. A disposição no andar superior de cada fase oferece a altura necessária sem mudar a física do personagem.


## Pausa e retorno ao menu

Durante a tentativa ou animação de captura, START pausa. O relógio, movimento, IA, obstáculos, quadros de animação e invulnerabilidade ficam congelados. START continua imediatamente, independentemente da opção selecionada. Também é possível escolher CONTINUE com cima/baixo e confirmar com A.

Na pausa, MAIN MENU retorna ao menu inicial. Nos resultados de vitória, tempo esgotado e Game Over, START também leva ao menu. START GAME sempre começa uma sessão nova com três vidas, zero pontos, fase 1 e 90 segundos. Não há opção de recuperar uma sessão abandonada pelo menu.

Segurar START não alterna pausa continuamente, e o A usado para confirmar não vira salto automático ao continuar. B e C permanecem reservados.


## Campanha e dificuldade

1. MAIN ATRIUM preserva o primeiro cenário: suba a escada da esquerda e depois a da direita. Limite de 90 segundos, criminoso a 90 px/s e três obstáculos.
2. EAST GALLERY inverte o trajeto: primeira escada à direita, segunda à esquerda. Limite de 80 segundos, criminoso a 101,25 px/s e quatro obstáculos.
3. SECURITY HUB usa primeira escada à esquerda e segunda ao centro. Limite de 70 segundos, criminoso a 112,5 px/s e cinco obstáculos.

O criminoso percebe o jogador a distâncias maiores e pausa menos nas fases avançadas. O jogador mantém sua velocidade máxima de 120 px/s, pulo e gravidade. Espere uma fase favorável dos obstáculos móveis antes de saltar. Dano retorna ao ponto inicial da fase atual; perder por tempo permite repetir esse mesmo mapa com A. Game Over ou nova campanha começa novamente na fase 1 com três vidas e zero pontos.

O HUD mostra STAGE e o total de três fases, além do nome do cenário. Não existe quarta fase nem repetição infinita dos mapas nesta versão.


## Áudio e apresentação (Fase 9)

Menu e perseguição têm temas PSG originais; a campanha varia andamento e transposição. Seleção, confirmação, início, salto, contato, dano, captura, conclusão, tempo esgotado, derrota e vitória têm efeitos. A pausa silencia o áudio e conserva sua posição. O contato contínuo com a parede não dispara sons repetidos.

Guarda e criminoso possuem oito poses: repouso, dois passos, salto, duas poses de escada, captura e dano. O início da proteção usa a pose de dano. Logotipo, vitrines e detalhes do prédio refinam a apresentação; os colisores e regras permanecem os da campanha.
