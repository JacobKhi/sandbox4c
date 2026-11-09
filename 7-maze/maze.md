# Projeto 7: gerador e solucionador de labirinto

Gera um labirinto aleatório, depois acha o caminho da entrada até a saída sozinho. Estimativa: **45 a 60 minutos**.

## O que fazer

```
$ ./labirinto 42
#######################
#S..#.....#...........#
#.#.#.###.#.#########.#
#.#...#...#.#.......#.#
...
#####################E#
```

Mesmo número (a "semente") sempre gera o mesmo labirinto. Sem argumento, usa `time(NULL)`.

Saída esperada: primeiro o labirinto, depois o mesmo labirinto com o caminho marcado por `.` e a quantidade de passos.

## Regras

1. C11, compilar com `gcc -std=c11 -Wall -Wextra -Werror -o labirinto labirinto.c`.
2. Um único arquivo `labirinto.c`.
3. Só libc (`stdio.h`, `stdlib.h`, `string.h`, `time.h`).
4. Grade `char mapa[ALTURA][LARGURA]` com `#define ALTURA 21` e `#define LARGURA 41`. **Ambos ímpares**, é isso que faz o algoritmo funcionar.
5. `#` é parede, espaço é corredor. Entrada `S` em `(1,1)` e saída `E` em `(ALTURA-2, LARGURA-2)`.
6. A borda externa é sempre parede.
7. **Geração**: backtracker recursivo (busca em profundidade). Ver algoritmo abaixo.
8. **Solução**: busca em largura (BFS) com fila em array. Isso garante o caminho **mais curto**.
9. Para reconstruir o caminho, guarde de onde cada célula veio (`pai[ALTURA][LARGURA]` ou um array de índices) e ande de volta do `E` até o `S`.
10. Marcar o caminho com `.` sem sobrescrever `S` e `E`.
11. Semente opcional pelo `argv[1]`, convertida com `strtol`. Argumento inválido: mensagem de uso e `return 1`.
12. Funções separadas: `gerar`, `resolver`, `imprimir`.

## Algoritmo de geração (backtracker recursivo)

1. Comece com a grade toda de paredes.
2. Marque `(1,1)` como corredor e chame `cavar(1, 1)`.
3. Em `cavar(y, x)`:
   - Embaralhe as 4 direções (cima, baixo, esquerda, direita).
   - Para cada direção, olhe a célula **2 passos** à frente `(y+2dy, x+2dx)`.
   - Se está dentro da grade (sem tocar a borda) e ainda é parede: abra a célula do meio `(y+dy, x+dx)` e a de destino, e chame `cavar` nela.
4. Quando todas as direções falharem, a função retorna (volta para trás: o "backtrack").

Por isso a grade precisa ser ímpar: as células "de verdade" ficam em coordenadas ímpares, e as paredes entre elas em coordenadas pares.

Embaralhar 4 direções (Fisher-Yates):

```c
int dirs[4] = {0, 1, 2, 3};
for (int i = 3; i > 0; i--) {
    int j = rand() % (i + 1);
    int tmp = dirs[i]; dirs[i] = dirs[j]; dirs[j] = tmp;
}
```

## Algoritmo de solução (BFS)

1. Fila com a posição do `S`; marque como visitada.
2. Enquanto a fila não está vazia: tire a primeira; se é o `E`, pare; senão, coloque na fila os vizinhos (4 direções) que não são parede e não foram visitados, gravando de onde vieram.
3. Do `E`, siga os "pais" até o `S` marcando o caminho.

A fila pode ser um array `int fila[ALTURA * LARGURA]` com dois índices (`inicio`, `fim`). Cada célula entra no máximo uma vez, então o tamanho é suficiente. Codifique a posição como `y * LARGURA + x`.

## Testes

```
./labirinto 1          # sempre o mesmo labirinto
./labirinto 1          # compare com a execucao anterior: tem que ser identico
./labirinto 2          # outro labirinto
./labirinto abc        # uso e status 1
```

Checagens que você pode automatizar:
- Todo labirinto tem solução (o `E` é sempre alcançado)
- O número de passos nunca é menor que `(ALTURA-3) + (LARGURA-3)` (distância mínima sem paredes)
- Nenhuma célula de borda é corredor

## Extras

- Animar a geração (imprimir a cada passo com `\033[H` e `nanosleep`)
- Animar o BFS mostrando as células exploradas
- Trocar a geração por outro algoritmo (Kruskal, Prim) e comparar o "estilo" do labirinto
- Receber altura e largura por `argv` e alocar a grade com `malloc`
- Comparar BFS com DFS na solução e mostrar qual achou o caminho mais curto
