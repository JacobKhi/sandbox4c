# Projeto 6: Jogo da Vida (Conway) animado no terminal

Simulação de células que nascem e morrem seguindo 4 regras simples. Estimativa: **30 a 45 minutos**.

É um dos projetos mais legais para ver rodando: padrões como o "glider" andam pela tela sozinhos.

## O que fazer

```
$ ./life
```

Uma grade de células aparece no terminal e evolui geração por geração, animada, até você apertar `Ctrl+C`.

## As regras do jogo

Cada célula está **viva** ou **morta**. A cada geração, olhando os 8 vizinhos:

1. Célula viva com **menos de 2** vizinhos vivos: morre (solidão)
2. Célula viva com **2 ou 3** vizinhos vivos: continua viva
3. Célula viva com **mais de 3** vizinhos vivos: morre (superpopulação)
4. Célula morta com **exatamente 3** vizinhos vivos: nasce

**Atenção:** todas as células mudam ao mesmo tempo. Você precisa de duas grades: uma com a geração atual e outra para calcular a próxima.

## Regras do projeto

1. C11, compilar com `gcc -std=c11 -Wall -Wextra -Werror -o life life.c`.
2. Um único arquivo `life.c`.
3. Só libc e POSIX. Para dormir entre as gerações use `nanosleep` (ver `../0-helpers/ajuda-funcoes-c-parte2.md`). Nada de `ncurses`.
4. Grade fixa com `#define LARGURA 40` e `#define ALTURA 20`, usando `char grade[ALTURA][LARGURA]`.
5. A grade é **toroidal**: o que sai pela direita volta pela esquerda, e o mesmo para cima/baixo. Use `%` para isso.
6. Desenhar com `#` para viva e `.` ou espaço para morta.
7. Limpar a tela a cada quadro com sequência ANSI (`"\033[H"` volta o cursor ao topo, sem piscar). Dar `fflush(stdout)` depois de desenhar.
8. Pausa de uns 100 ms entre as gerações.
9. Mostrar o número da geração e a quantidade de células vivas abaixo da grade.
10. Funções separadas: `contar_vizinhos`, `proxima_geracao`, `desenhar`, `inicializar`.
11. Tratar `Ctrl+C` com `signal(SIGINT, ...)` e uma variável `volatile sig_atomic_t`. Ao sair, mostrar o cursor de novo se você o escondeu.
12. Nenhuma variável global além da flag do sinal.

## Estado inicial

Comece com um **glider** (ele anda na diagonal) para validar as regras:

```
. # .
. . #
# # #
```

Depois adicione um modo aleatório: se rodar com `./life -r`, preencher a grade com ~25% de células vivas (`rand`).

## Dica de lógica

Contar vizinhos com o wrap-around:

```c
int contar_vizinhos(char g[ALTURA][LARGURA], int y, int x) {
    int total = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;           // nao conta a propria celula
            int ny = (y + dy + ALTURA) % ALTURA;        // + ALTURA evita negativo
            int nx = (x + dx + LARGURA) % LARGURA;
            total += g[ny][nx];                         // 1 se viva, 0 se morta
        }
    }
    return total;
}
```

Guarde as células como `0` e `1` (não como `'#'`), e converta para caractere só na hora de desenhar.

## Testes

- Glider: deve andar uma célula na diagonal a cada 4 gerações e dar a volta na tela.
- **Bloco** (quadrado 2x2 vivo): nunca muda.
- **Blinker** (3 células em linha): alterna entre horizontal e vertical a cada geração.
- Grade toda morta: continua morta, contador de vivas em 0.
- Rodar um tempo e apertar `Ctrl+C`: o terminal precisa voltar ao normal.

## Extras

- Carregar o padrão inicial de um arquivo texto (`#` e `.`)
- Detectar quando a grade parou de mudar (estável) e encerrar
- Usar o tamanho real do terminal (`ioctl` com `TIOCGWINSZ`)
- Cores ANSI diferentes para células recém-nascidas
