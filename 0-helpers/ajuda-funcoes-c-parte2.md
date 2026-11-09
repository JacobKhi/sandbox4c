# Ajuda parte 2: o que os projetos 5, 6 e 7 pedem

Complemento do `ajuda-funcoes-c.md`. Cobre arrays 2D, recursão, pilha/fila, sinais, pausa e escape ANSI.

---

## 1. Array 2D e passar para função

```c
#define ALTURA 20
#define LARGURA 40

char grade[ALTURA][LARGURA];          // acessa com grade[y][x]
```

Primeiro índice é a **linha** (y), segundo é a **coluna** (x). É fácil inverter, então use sempre a mesma ordem.

Passando para função: o **primeiro** tamanho pode ficar de fora, os outros **não**.

```c
void desenhar(char g[ALTURA][LARGURA]);    // ok
void desenhar(char g[][LARGURA]);          // ok, equivalente
void desenhar(char g[][]);                 // ERRO: falta a largura
```

Como é array, a função recebe o endereço e **altera o original** (sem precisar de `&`):

```c
void limpar(char g[ALTURA][LARGURA]) {
    memset(g, 0, ALTURA * LARGURA);        // zera tudo
}
limpar(grade);                              // passa o nome, sem &
```

Para **copiar** uma grade inteira (útil no Jogo da Vida):

```c
memcpy(destino, origem, sizeof(char) * ALTURA * LARGURA);
// ou, com a grade declarada de verdade no escopo: sizeof origem
```

`memcpy` e `memset` estão em `<string.h>`.

Inicializar com um padrão:

```c
char grade[3][3] = {
    {0, 1, 0},
    {0, 0, 1},
    {1, 1, 1},
};
```

### Dentro dos limites

Antes de acessar uma vizinha:

```c
if (ny >= 0 && ny < ALTURA && nx >= 0 && nx < LARGURA) { ... }
```

### Wrap-around (grade toroidal)

```c
int ny = (y + dy + ALTURA) % ALTURA;
```

O `+ ALTURA` é obrigatório: em C, `-1 % 20` dá `-1`, e não `19`.

---

## 2. Direções com arrays (evita 4 `if`s repetidos)

```c
const int DY[4] = {-1, 1, 0, 0};    // cima, baixo, esquerda, direita
const int DX[4] = {0, 0, -1, 1};

for (int d = 0; d < 4; d++) {
    int ny = y + DY[d];
    int nx = x + DX[d];
    ...
}
```

---

## 3. Recursão

Função que chama ela mesma. Precisa de:

1. **Caso base**: quando para
2. **Passo recursivo**: chama a si mesma com algo "menor" ou diferente

```c
void cavar(char m[ALTURA][LARGURA], int y, int x) {
    m[y][x] = ' ';
    /* para cada direcao possivel: */
    /*     abre a parede do meio e chama cavar(m, ny, nx); */
    /* quando acabam as direcoes, a funcao simplesmente retorna (backtrack) */
}
```

Cuidados:
- Cada chamada ocupa memória na pilha. Para um labirinto 21x41 não é problema, mas num de 2000x2000 estouraria (`Segmentation fault` sem motivo aparente).
- Se não existe caso base que seja alcançado, é recursão infinita.

---

## 4. Pilha e fila com array

Não existe tipo pronto em C. Faça com array + índice.

### Pilha (último a entrar, primeiro a sair)

```c
int pilha[1000];
int topo = 0;                     // quantos itens tem

pilha[topo++] = valor;            // empilhar (push)
int v = pilha[--topo];            // desempilhar (pop)
if (topo == 0) { /* vazia */ }
```

Usada no Brainfuck para casar `[` com `]`.

### Fila (primeiro a entrar, primeiro a sair)

```c
int fila[ALTURA * LARGURA];
int inicio = 0, fim = 0;

fila[fim++] = valor;              // entra no fim
int v = fila[inicio++];           // sai do inicio
while (inicio < fim) { ... }      // enquanto nao esta vazia
```

Funciona sem "dar a volta" porque no BFS cada célula entra no máximo uma vez. Usada na busca do labirinto.

### Posição 2D em um número só

```c
int id = y * LARGURA + x;         // (y, x) -> id
int y = id / LARGURA;             // id -> y
int x = id % LARGURA;             // id -> x
```

Assim a fila guarda um `int` por célula em vez de uma struct.

---

## 5. Pausa entre quadros: `nanosleep`

**Atenção a um problema comum.** Com `-std=c11`, funções POSIX como `nanosleep` e `usleep` **não são declaradas** por padrão, e com `-Werror` o programa nem compila. Resolva colocando isto na **primeira linha do arquivo**, antes de qualquer `#include`:

```c
#define _POSIX_C_SOURCE 200809L
#include <time.h>
```

Usar:

```c
struct timespec pausa = { .tv_sec = 0, .tv_nsec = 100 * 1000000L };   // 100 ms
nanosleep(&pausa, NULL);
```

`nanosleep` recebe um **ponteiro** para a struct (por isso o `&`). O segundo argumento (`NULL`) é onde ele devolveria o tempo que faltou se for interrompido por sinal.

`usleep(100000)` também funciona (microssegundos), mas é considerada obsoleta.

---

## 6. Escape ANSI: controlar o terminal

Sequências que o terminal interpreta. Começam com `\033[` (o caractere ESC).

| Sequência | Efeito |
|---|---|
| `"\033[H"` | cursor volta para o canto superior esquerdo |
| `"\033[2J"` | limpa a tela inteira |
| `"\033[?25l"` | esconde o cursor |
| `"\033[?25h"` | mostra o cursor |
| `"\033[31m"` | texto vermelho (`32` verde, `33` amarelo, `34` azul) |
| `"\033[0m"` | volta à cor normal |

Para animação sem piscar: limpe a tela **uma vez** no começo e, a cada quadro, use só `\033[H` (volta ao topo) e redesenhe por cima.

```c
printf("\033[2J");                 // uma vez
while (rodando) {
    printf("\033[H");              // todo quadro
    desenhar(grade);
    fflush(stdout);                // sem isso a saida pode ficar presa no buffer
    nanosleep(&pausa, NULL);
}
printf("\033[?25h");               // devolve o cursor ao sair
```

---

## 7. Sinais: capturar o `Ctrl+C`

Por padrão `Ctrl+C` mata o programa na hora, e o terminal pode ficar com o cursor escondido. Para sair de forma limpa:

```c
#include <signal.h>

static volatile sig_atomic_t parar = 0;

static void ao_receber_sinal(int sig) {
    (void) sig;               // evita warning de parametro nao usado
    parar = 1;                // so isso: dentro de handler faca o minimo
}

int main(void) {
    signal(SIGINT, ao_receber_sinal);
    while (!parar) {
        /* ... */
    }
    printf("\033[?25h");      // limpeza fora do handler
    return 0;
}
```

Regras:
- A variável tem que ser `volatile sig_atomic_t`, senão o compilador pode "otimizar" o `while` e nunca enxergar a mudança.
- Dentro do handler só mude a flag. Não use `printf`, `malloc` nem `free` ali.

---

## 8. Ler um arquivo inteiro para a memória (Brainfuck)

```c
FILE *f = fopen(caminho, "rb");
if (!f) { perror(caminho); return 1; }

fseek(f, 0, SEEK_END);
long tam = ftell(f);              // tamanho em bytes
rewind(f);                        // volta ao inicio

char *codigo = malloc((size_t) tam + 1);
if (!codigo) { fclose(f); return 1; }

size_t lidos = fread(codigo, 1, (size_t) tam, f);
codigo[lidos] = '\0';
fclose(f);

/* ... usa codigo ... */
free(codigo);
```

Detalhes:
- `+ 1` no `malloc` é para o `'\0'` final.
- `fread` devolve quantos bytes **de fato** leu. Use esse valor, não `tam`, para o terminador.
- `ftell` pode devolver `-1` em erro; num projeto simples dá para ignorar, mas se quiser ser rigoroso, cheque.

---

## 9. `switch`

Bom para decidir por caractere (Brainfuck, menus, comandos):

```c
switch (c) {
    case '+': celula++; break;       // sem break, cai no proximo case!
    case '-': celula--; break;
    default:  break;                 // qualquer outro caractere
}
```

O esquecimento do `break` é o erro clássico: a execução continua no `case` seguinte.

---

## 10. Aleatoriedade com semente fixa

```c
srand(42);              // mesma semente = mesma sequencia, sempre
int n = rand() % 4;     // 0 a 3
```

Útil para depurar: se o labirinto "bugado" aparece com a semente 7, você consegue reproduzir toda vez. Sem argumento, use `srand((unsigned) time(NULL))`.

---

## 11. Erros comuns desses projetos

- [ ] Inverteu `[y][x]` por `[x][y]` em algum lugar
- [ ] `%` com número negativo (esqueceu de somar o tamanho)
- [ ] Atualizou a grade "no lugar" no Jogo da Vida (precisa de uma segunda grade)
- [ ] Esqueceu `fflush(stdout)` e a animação não aparece
- [ ] Faltou o `#define _POSIX_C_SOURCE 200809L` e `nanosleep` não compila
- [ ] Pilha/fila pequena demais, estourando o array
- [ ] Esqueceu o `break` em um `case`
- [ ] Labirinto com altura ou largura **par**
- [ ] Recursão sem condição de parada
- [ ] Deixou o cursor escondido ao sair (`Ctrl+C` sem tratar o sinal). Para consertar o terminal: `printf '\033[?25h'` ou `reset`
