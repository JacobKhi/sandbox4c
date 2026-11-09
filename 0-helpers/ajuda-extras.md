# Ajuda extras: o que faltava nos outros dois guias

Complemento de `ajuda-funcoes-c.md` e `ajuda-funcoes-c-parte2.md`. Foco em quem está **sem internet**.

---

## 1. `man`: a documentação que já está no seu PC

Funciona offline. É a primeira coisa a tentar antes de procurar em outro lugar.

```
man 3 fgetc        # funcao da biblioteca C (secao 3)
man 3 printf
man 3 strtol
man 2 open         # chamada de sistema (secao 2)
man -k "string"    # busca por palavra em todas as paginas (igual a apropos)
```

Dentro do `man`:

| Tecla | Faz |
|---|---|
| `/texto` + Enter | busca `texto`; `n` vai para a próxima ocorrência |
| `espaço` / `b` | desce / sobe uma página |
| `q` | sai |

Como ler uma página:
- **SYNOPSIS**: o `#include` necessário e a assinatura da função. Veja aqui qual header usar.
- **DESCRIPTION**: o que faz.
- **RETURN VALUE**: o que retorna em sucesso e em erro. É a parte mais importante.
- **EXAMPLES**: nem toda página tem, mas ajuda quando tem.

No Arch, se der `No manual entry`, falta instalar `man-db` e `man-pages` (precisa de internet uma vez):

```
sudo pacman -S man-db man-pages
```

Teste agora, enquanto ainda há internet: `man 3 fgetc`.

---

## 2. `printf` com largura dinâmica: `%*d`

Com `%5d` a largura é fixa no código. Com `*`, ela vem de uma variável:

```c
int largura = 6;
int n = 42;
printf("%*d\n", largura, n);     // imprime "    42"
```

Regra: cada `*` consome **um argumento extra `int` antes** do valor. Então `%*d` leva dois argumentos: largura, depois número.

Funciona com outros formatos e com mais de um:

```c
printf("%*d %*d\n", larg, a, larg, b);
printf("%-*s|\n", 10, "oi");        // texto alinhado a esquerda em 10 colunas
```

### Descobrir quantos caracteres um número ocupa

`snprintf` com buffer `NULL` e tamanho `0` **não escreve nada**, só devolve quantos caracteres escreveria:

```c
int digitos = snprintf(NULL, 0, "%d", 12345);    // 5
```

Outro jeito, sem `snprintf`: dividir por 10 em loop, contando as voltas. Cuidado com o número `0`, que também ocupa 1 caractere.

---

## 3. Flags em `argv`: o padrão de loop

`argv` é um array de strings. Percorra de `1` até `argc - 1` (o `0` é o nome do programa).

Exemplo genérico com uma flag `-v` e um nome de arquivo:

```c
int verbose = 0;
const char *arquivo = NULL;

for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {                 // comeca com '-': e flag
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else {
            fprintf(stderr, "flag desconhecida: %s\n", argv[i]);
            return 1;
        }
    } else {
        arquivo = argv[i];                   // nao e flag: e o arquivo
    }
}

if (arquivo == NULL) { /* nenhum arquivo: usa stdin */ }
```

Pontos importantes:
- `argv[i][0]` é o **primeiro caractere** da string `argv[i]`. Compare com `'-'` (aspas simples, é um `char`).
- Compare a string inteira com `strcmp(...) == 0`, **nunca** com `==`.
- `"-"` sozinho (só o hífen) começa com `-` mas muitos programas o tratam como "stdin". Decida se quer tratar.
- Variável `const char *` que começa em `NULL` é um jeito simples de saber "ainda não achei o arquivo".

### Flags juntas, como `-lw`

A string `"-lw"` tem 3 caracteres úteis: `-`, `l`, `w`. Percorra do índice `1` até o fim da string:

```c
for (int j = 1; argv[i][j] != '\0'; j++) {
    switch (argv[i][j]) {
        case 'l': /* liga a opcao l */ break;
        case 'w': /* liga a opcao w */ break;
        default:  /* flag desconhecida */ break;
    }
}
```

### Padrão "liga tudo se nenhuma foi pedida"

Guarde cada opção em uma variável `0`/`1`. Depois do loop, se **todas** estão em 0, ligue todas. Assim o comportamento padrão aparece sem flag nenhuma.

### Imprimir valores separados por espaço sem sobrar espaço

Use uma variável `primeiro = 1`. Antes de imprimir cada item: se não é o primeiro, imprima um espaço; depois ponha `primeiro = 0`.

---

## 4. `getopt`: flags prontas (opcional)

Existe no POSIX, trata `-l -w` e `-lw` sozinha. Para aprender, fazer à mão (seção 3) é melhor, mas é bom conhecer.

```c
#define _POSIX_C_SOURCE 200809L     // primeira linha do arquivo, senao getopt nao e declarada com -std=c11
#include <unistd.h>

int opt;
while ((opt = getopt(argc, argv, "lwc")) != -1) {
    switch (opt) {
        case 'l': /* ... */ break;
        case 'w': /* ... */ break;
        case 'c': /* ... */ break;
        default:  return 1;          // flag invalida (getopt ja imprimiu o erro)
    }
}
/* depois do loop, argv[optind] e o primeiro argumento que nao e flag */
```

- A string `"lwc"` lista as flags aceitas. Uma letra seguida de `:` (ex.: `"o:"`) significa que ela recebe valor, que fica em `optarg`.
- `optind` é o índice do próximo argumento ainda não processado.
- Veja `man 3 getopt` para os detalhes.

---

## 5. `ctype.h` com `char` pode dar problema

As funções `isspace`, `isalpha`, etc. esperam um valor entre `0` e `255` ou `EOF`. Um `char` pode ser **negativo** (acentos em UTF-8, por exemplo), e isso é comportamento indefinido.

Se o caractere vem de `fgetc` (que devolve `int`), está tudo certo. Se vem de uma string (`char`), converta:

```c
char c = texto[i];
if (isspace((unsigned char) c)) { ... }
```

---

## 6. Conceitos básicos que aparecem nos exemplos

### `const`
Promessa de que o valor não será alterado. Em ponteiros, `const char *s` significa "não vou modificar o texto para onde `s` aponta". Use em parâmetros de funções que só leem.

### `#define`
Substituição de texto antes de compilar. Bom para constantes:

```c
#define MAX 100
char buf[MAX];
```

Não leva `;` no final nem `=`. Por convenção, nome em maiúsculas.

### `enum`
Nomes para números inteiros, bom para estados:

```c
enum Estado { PARADO, ANDANDO, MORTO };      // valem 0, 1, 2
enum Estado e = ANDANDO;
```

### `static`
- Em função ou variável **global**: ela só é visível dentro daquele arquivo `.c`.
- Em variável **local**: o valor persiste entre chamadas da função.

### `size_t`
Tipo sem sinal para tamanhos e índices (retorno de `strlen`, `sizeof`, `fread`). Imprima com `%zu`. Cuidado: `size_t i = 0; i - 1` não dá `-1`, dá um número enorme.

### `bool`

```c
#include <stdbool.h>
bool achou = false;
if (!achou) { achou = true; }
```

Sem esse header, use `int` com `0` e `1`.

### `time`

```c
#include <time.h>
time_t agora = time(NULL);       // segundos desde 1970; usado como semente de srand
```

### Tipos de largura fixa

```c
#include <stdint.h>
uint8_t  b;     // 0 a 255
int32_t  n;     // sempre 32 bits
uint64_t g;     // 0 a 18 quintilhoes
```

Útil quando o tamanho importa (contadores grandes, Brainfuck com células de 8 bits).

---

## 7. Compilação: lendo os erros

Os flags `-Wall -Wextra -Werror` transformam avisos em erros. Isso é bom: força a corrigir.

| Mensagem | Quase sempre significa |
|---|---|
| `implicit declaration of function 'x'` | faltou o `#include` da função |
| `unused variable 'x'` | variável declarada e nunca usada |
| `comparison between signed and unsigned` | misturou `int` com `size_t` |
| `format '%d' expects argument of type 'int'` | formato do `printf` não bate com o tipo |
| `control reaches end of non-void function` | função com retorno sem `return` em algum caminho |
| `undefined reference to 'x'` | função declarada mas não definida (ou faltou `-lm` para `math.h`) |
| `Segmentation fault` | acesso a memória inválida; recompile com `-fsanitize=address` |

Leia **sempre o primeiro erro** da lista. Os seguintes costumam ser consequência dele.

Se usar `math.h` (`sqrt`, `pow`), adicione `-lm` no **fim** do comando do `gcc`.
