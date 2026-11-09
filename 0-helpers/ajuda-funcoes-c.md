# Ajuda: funções e conceitos de C para os projetos

Guia de consulta rápida. Não precisa ler tudo antes: abra quando travar em alguma função.

Compilar sempre com:

```
gcc -std=c11 -Wall -Wextra -Werror -g -o programa programa.c
```

---

## 1. Ponteiro ou referência?

**C não tem referência.** Isso é coisa de C++. Em C existe só **passagem por valor**: a função recebe uma **cópia** do argumento.

Para uma função alterar uma variável de quem chamou, você passa o **endereço** dela (um ponteiro).

```c
void errado(int x) { x = 10; }    // altera so a copia
void certo(int *x) { *x = 10; }   // altera o original

int a = 1;
errado(a);    // a continua 1
certo(&a);    // a agora e 10
```

| Símbolo | Significa | Exemplo |
|---|---|---|
| `&x` | endereço de `x` | `certo(&a)` |
| `int *p` | declara `p` como ponteiro para int | |
| `*p` | o valor para onde `p` aponta | `*p = 10` |
| `p->campo` | campo de struct via ponteiro | `pessoa->idade` |
| `s.campo` | campo de struct direto | `pessoa.idade` |

### Quando usar ponteiro

- A função precisa **modificar** uma variável do chamador
- A função precisa **devolver mais de um valor** (um vai no `return`, outros por ponteiro)
- Você está passando um **array ou string** (sempre chega como ponteiro)
- Você está passando uma **struct grande** (evita copiar; use `const` se não for alterar)
- Você usa `FILE *` (sempre ponteiro)

### Arrays viram ponteiros ao serem passados

```c
void imprime(const int *v, size_t n) {   // equivale a int v[]
    for (size_t i = 0; i < n; i++) printf("%d\n", v[i]);
}

int nums[5] = {1, 2, 3, 4, 5};
imprime(nums, 5);    // sem &, o nome do array ja e o endereco
```

Dentro da função, `sizeof(v)` dá o tamanho do ponteiro, não do array. **Sempre passe o tamanho junto.**

### Strings

String em C é um array de `char` terminado em `'\0'`.

```c
char buf[100];             // buffer que voce pode modificar
const char *msg = "oi";    // literal: NAO modifique, use const
```

Tamanho de `"oi"` na memória: 3 bytes (`'o'`, `'i'`, `'\0'`). `strlen("oi")` é 2.

### Retornar ponteiro

Nunca devolva ponteiro para variável local, ela deixa de existir quando a função termina.

```c
char *ruim(void) { char s[20] = "oi"; return s; }   // BUG
```

---

## 2. Arquivos: `stdio.h`

### `fopen` e `fclose`

```c
FILE *f = fopen("dados.txt", "r");
if (f == NULL) {
    perror("dados.txt");     // imprime: dados.txt: No such file or directory
    return 1;
}
/* ... usa f ... */
fclose(f);
```

- `FILE *` é um ponteiro que o **`fopen` te entrega**. Você não cria nem desreferencia. Só passa adiante para as outras funções.
- **Sempre** checar `NULL`.
- **Sempre** `fclose`.

| Modo | O que faz | Se o arquivo não existe |
|---|---|---|
| `"r"` | só leitura | falha (`NULL`) |
| `"w"` | escrita, **apaga o conteúdo** | cria |
| `"a"` | escrita no fim (append) | cria |
| `"r+"` | leitura e escrita | falha |
| `"w+"` | leitura e escrita, apaga | cria |
| `"a+"` | leitura e append | cria |

Em Linux `"b"` (binário) é opcional, mas use em `fread`/`fwrite` (`"rb"`, `"wb"`) por clareza.

### Streams prontos

`stdin` (entrada), `stdout` (saída), `stderr` (erros). São `FILE *` já abertos. Não dê `fclose` neles.

### Ler caractere: `fgetc`

```c
int c;                                  // int, NAO char!
while ((c = fgetc(f)) != EOF) {
    /* usa c */
}
```

Por que `int`: `EOF` vale -1 e precisa ser distinguível de qualquer byte válido. Com `char` o loop pode nunca terminar ou terminar cedo.

### Ler linha: `fgets`

```c
char linha[256];
while (fgets(linha, sizeof linha, f) != NULL) {
    linha[strcspn(linha, "\n")] = '\0';    // remove o \n do final
    /* usa linha */
}
```

- Assinatura: `char *fgets(char *buf, int tamanho, FILE *f)`
- Lê no máximo `tamanho - 1` caracteres e sempre coloca o `'\0'`.
- **Mantém o `\n`** se a linha coube no buffer. Remova com `strcspn`.
- Retorna `NULL` em EOF ou erro.
- **Nunca use `gets`**. Foi removida do C11 por ser insegura.

### Escrever: `fprintf`, `fputs`, `fputc`

```c
fprintf(f, "%d|%s\n", status, texto);   // como printf, mas para um FILE *
fputs("linha\n", f);                    // nao adiciona \n sozinho
fputc('x', f);
fprintf(stderr, "erro: %s\n", msg);     // mensagens de erro vao para stderr
```

`printf(...)` é igual a `fprintf(stdout, ...)`.

### Ler formatado: `fscanf` e `sscanf`

```c
int status;
char texto[200];
if (sscanf(linha, "%d|%199[^\n]", &status, texto) == 2) { /* ok */ }
```

- Retorna quantos campos conseguiu ler. **Sempre compare com o esperado.**
- Precisa de `&` em variáveis simples (`&status`), mas **não** em arrays/strings (`texto`).
- Em `%s` e `%[...]` **sempre limite** o tamanho (`%199s` para buffer de 200).
- Para linhas, prefira `fgets` + `sscanf`/`strtol`. É mais previsível que `fscanf` direto.

### Binário: `fread` e `fwrite`

```c
struct Pessoa { char nome[50]; int idade; };
struct Pessoa p = {"Ana", 20};

fwrite(&p, sizeof p, 1, f);        // escreve 1 struct
fread(&p, sizeof p, 1, f);         // le 1 struct, retorna quantos leu
```

Assinatura: `size_t fread(void *dst, size_t tamanho, size_t quantidade, FILE *f)`. Mesma ordem para `fwrite`.

### Outras úteis

| Função | Para quê |
|---|---|
| `rewind(f)` | volta ao início do arquivo |
| `fseek(f, 0, SEEK_END)` | vai para o fim (com `ftell(f)` você descobre o tamanho) |
| `fflush(stdout)` | força a saída a aparecer agora |
| `remove("a.txt")` | apaga arquivo |
| `rename("a.tmp", "a.txt")` | renomeia (substitui o destino) |

### Armadilha do `feof`

Errado:

```c
while (!feof(f)) {       // roda uma vez a mais
    fgets(linha, sizeof linha, f);
    ...
}
```

Certo: use o retorno da própria função de leitura no `while`, como nos exemplos acima.

---

## 3. Entrada do usuário

### `scanf`: use com cuidado

```c
int n;
if (scanf("%d", &n) != 1) { /* entrada invalida */ }
```

Problemas: deixa o `\n` no buffer, trava em entrada ruim (letras ficam presas), e `%s` sem limite estoura buffer.

### Jeito robusto: `fgets` + `strtol`

```c
char buf[64];
if (fgets(buf, sizeof buf, stdin) == NULL) { /* EOF (Ctrl+D) */ }
buf[strcspn(buf, "\n")] = '\0';

char *fim;
errno = 0;
long n = strtol(buf, &fim, 10);

if (fim == buf || *fim != '\0' || errno == ERANGE) {
    /* invalido: vazio, tem lixo depois do numero, ou grande demais */
}
```

Explicação:

- `fim` é um **ponteiro de saída**: `strtol` grava ali onde parou de ler. Por isso você passa `&fim` (endereço de um `char *`).
- `fim == buf`: não leu nenhum dígito.
- `*fim != '\0'`: sobrou texto depois do número (`"5x"`).
- `errno == ERANGE`: número não cabe em `long`.
- Precisa de `#include <errno.h>` e `<stdlib.h>`.

`atoi` existe mas **não avisa erro** (`atoi("abc")` retorna 0 igual a `atoi("0")`). Evite.

---

## 4. Strings: `string.h`

| Função | O que faz | Observação |
|---|---|---|
| `strlen(s)` | tamanho sem o `'\0'` | retorna `size_t` |
| `strcmp(a, b)` | compara | retorna **0 se iguais**. Nunca use `==` em strings |
| `strncmp(a, b, n)` | compara os n primeiros | |
| `strcpy(dst, src)` | copia | não checa tamanho, evite |
| `strncpy(dst, src, n)` | copia até n | **pode não colocar `'\0'`**, termine à mão |
| `strcat(dst, src)` | concatena | não checa tamanho |
| `strchr(s, c)` | acha 1º caractere | retorna ponteiro ou `NULL` |
| `strstr(s, sub)` | acha substring | retorna ponteiro ou `NULL` |
| `strcspn(s, rejeita)` | posição do 1º char de `rejeita` | truque do `\n` |
| `memset(p, 0, n)` | preenche n bytes | zerar array |

Para copiar/montar strings com segurança, prefira `snprintf`:

```c
char dst[50];
snprintf(dst, sizeof dst, "%s-%d", nome, num);   // nunca passa de 50 bytes
```

Comparar strings:

```c
if (strcmp(cmd, "add") == 0) { ... }     // certo
if (cmd == "add") { ... }                // ERRADO: compara enderecos
```

---

## 5. Caracteres: `ctype.h`

Todas recebem `int` (o valor de um `char` ou `EOF`) e devolvem **não zero** para verdadeiro.

| Função | Testa |
|---|---|
| `isalpha(c)` | letra |
| `isdigit(c)` | dígito `0` a `9` |
| `isalnum(c)` | letra ou dígito |
| `isspace(c)` | espaço, tab, `\n`, `\r`... |
| `isupper(c)` / `islower(c)` | maiúscula / minúscula |
| `toupper(c)` / `tolower(c)` | converte |

Truques:

```c
c - '0'      // '7' vira 7
c - 'a'      // 'd' vira 3 (indice no alfabeto)
'a' + 3      // 'd'
```

---

## 6. Memória e utilidades: `stdlib.h`

### Alocação dinâmica

```c
int *v = malloc(10 * sizeof *v);       // 10 ints
if (v == NULL) { perror("malloc"); return 1; }

v = ... ;
free(v);
v = NULL;                              // evita usar depois de liberar
```

- `sizeof *v` acompanha o tipo sozinho, é mais seguro que `sizeof(int)`.
- `calloc(n, sizeof *v)` aloca já zerado.
- `realloc(p, novo_tamanho)` redimensiona. Guarde em variável temporária, porque se falhar devolve `NULL` e você perderia o ponteiro original.
- Cada `malloc` precisa de exatamente um `free`.

### Outras

| Função | Para quê |
|---|---|
| `exit(EXIT_FAILURE)` | encerra o programa de qualquer lugar |
| `rand()` | número pseudoaleatório de 0 a `RAND_MAX` |
| `srand(semente)` | define a semente. **Chame uma vez só**, no início |
| `qsort(...)` | ordena array (precisa de função de comparação) |
| `abs(n)` | valor absoluto |

Número aleatório entre 1 e 100:

```c
srand((unsigned) time(NULL));   // precisa de <time.h>, uma vez no main
int n = rand() % 100 + 1;
```

---

## 7. Argumentos do programa: `argc` e `argv`

```c
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "uso: %s <arquivo>\n", argv[0]);
        return 1;
    }
    printf("arquivo: %s\n", argv[1]);
    return 0;
}
```

- `argc`: quantidade de argumentos, **contando o nome do programa**.
- `argv[0]`: nome do programa. `argv[1]`: primeiro argumento. `argv[argc]` é `NULL`.
- Todo argumento chega como **string**. Para número, use `strtol`.
- `./prog add "duas palavras"` → `argc` = 3 (as aspas fazem virar um argumento só).

---

## 8. Structs

```c
struct Pessoa {
    char nome[50];
    int idade;
};

struct Pessoa p = {"Ana", 20};
printf("%s %d\n", p.nome, p.idade);      // ponto: variavel normal

void aniversario(struct Pessoa *p) {
    p->idade++;                          // seta: via ponteiro (equivale a (*p).idade++)
}
aniversario(&p);
```

`typedef` evita escrever `struct` toda vez:

```c
typedef struct { char nome[50]; int idade; } Pessoa;
Pessoa p;
```

---

## 9. printf: formatos

| Formato | Tipo |
|---|---|
| `%d` | `int` |
| `%ld` | `long` |
| `%u` | `unsigned int` |
| `%zu` | `size_t` (retorno de `strlen`, `sizeof`) |
| `%c` | `char` |
| `%s` | string |
| `%f` | `double` (no `printf`) |
| `%lf` | `double` (no `scanf`) |
| `%x` | hexadecimal |
| `%p` | ponteiro |
| `%%` | o caractere `%` |

Largura e precisão: `%5d` (largura 5), `%-5d` (alinha à esquerda), `%.2f` (2 casas), `%05d` (zeros à esquerda).

---

## 10. Erros: `errno`, `perror`

```c
FILE *f = fopen("x.txt", "r");
if (!f) {
    perror("x.txt");                          // x.txt: No such file or directory
    // ou: fprintf(stderr, "erro: %s\n", strerror(errno));
    return 1;
}
```

- `perror(prefixo)` já imprime em `stderr` a mensagem do último erro.
- `errno` só tem significado **logo depois** de uma chamada que falhou. Para `strtol`, zere antes (`errno = 0`).
- Convenção: `return 0` (ou `EXIT_SUCCESS`) é sucesso, qualquer outro valor é erro.

---

## 11. Erros comuns (checklist antes de pedir ajuda)

- [ ] `=` no lugar de `==` dentro de `if`
- [ ] Comparou string com `==` em vez de `strcmp`
- [ ] Usou `char c` com `fgetc` (precisa ser `int`)
- [ ] Esqueceu o `\n` que `fgets` deixa na string
- [ ] Buffer pequeno demais (lembre do `'\0'` extra)
- [ ] Índice fora do array (`v[5]` em array de 5)
- [ ] `scanf` sem `&` em variável simples
- [ ] Esqueceu `fclose` ou `free`
- [ ] Usou ponteiro depois do `free`
- [ ] Variável local sem inicializar (lixo de memória)
- [ ] Não checou o retorno de `fopen` / `malloc` / `fgets`
- [ ] Chamou `srand` dentro de um loop (sempre sai o mesmo número)

## 12. Ferramentas para achar bugs

```
gcc -g -fsanitize=address,undefined -o prog prog.c    # pega estouro de buffer e UB na hora
valgrind --leak-check=full ./prog                     # pega vazamento de memoria
gdb ./prog                                            # debugger (break main, run, next, print x)
```

Se o programa crashar com `Segmentation fault`, recompile com `-fsanitize=address` primeiro: ele aponta a linha exata.
