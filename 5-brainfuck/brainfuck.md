# Projeto 5: interpretador de Brainfuck

Escreva um interpretador para uma linguagem de programação inteira, em C. Estimativa: **30 a 45 minutos**.

Brainfuck tem só 8 comandos, então o interpretador é pequeno, mas mexe com ponteiros, arrays, pilha e leitura de arquivo.

## O que fazer

```
$ ./bf hello.bf
Hello World!
```

O programa `bf` lê um arquivo com código Brainfuck e executa.

## A linguagem

Existe uma **fita** de 30000 células (cada uma é um byte, começando em 0) e um **ponteiro** que começa na célula 0.

| Comando | O que faz |
|---|---|
| `>` | move o ponteiro para a direita |
| `<` | move o ponteiro para a esquerda |
| `+` | incrementa a célula atual |
| `-` | decrementa a célula atual |
| `.` | imprime a célula atual como caractere |
| `,` | lê um caractere da entrada e guarda na célula |
| `[` | se a célula atual é 0, pula para depois do `]` correspondente |
| `]` | se a célula atual **não** é 0, volta para depois do `[` correspondente |

Qualquer outro caractere do arquivo é **comentário** e deve ser ignorado.

## Regras

1. C11, compilar com `gcc -std=c11 -Wall -Wextra -Werror -o bf bf.c`.
2. Um único arquivo `bf.c`.
3. Só libc (`stdio.h`, `stdlib.h`).
4. Fita: `unsigned char fita[30000]`, zerada. O `unsigned char` já faz a célula dar a volta (255 + 1 vira 0, 0 - 1 vira 255) sem você fazer nada.
5. Ler o arquivo inteiro para um buffer em memória (`fopen`, `fseek`/`ftell` ou `fread`, `malloc`) antes de executar.
6. **Pré-calcular os pares de colchetes** antes de rodar, num array `int salto[tamanho_do_codigo]`. Não procure o colchete correspondente toda vez que passar por ele.
7. Usar uma **pilha** (array de ints) para achar os pares. Colchete `]` sem `[` ou `[` sem `]`: imprimir erro em `stderr` e `return 1`.
8. Se o ponteiro sair da fita (menor que 0 ou maior ou igual a 30000): erro e `return 1`.
9. `,` em EOF: grave 0 na célula (decisão sua, mas documente em um comentário).
10. Sem argumento ou arquivo que não abre: mensagem de uso/`perror` e `return 1`.
11. `free` no buffer e `fclose` no arquivo.

## Dica de lógica

O laço principal é um `for` ou `while` com um índice `ip` (instruction pointer) e um `switch`:

```c
for (size_t ip = 0; ip < tamanho; ip++) {
    switch (codigo[ip]) {
        case '>': ptr++; break;
        case '[': if (fita[ptr] == 0) ip = salto[ip]; break;
        case ']': if (fita[ptr] != 0) ip = salto[ip]; break;
        /* ... */
    }
}
```

Para montar `salto`: percorra o código; em `[` empilhe o índice; em `]` desempilhe `a`, e grave `salto[a] = i` e `salto[i] = a`.

## Testes

Salve em arquivos `.bf` e rode:

```
+++++++++[>++++++++<-]>.
```
Deve imprimir `H` (9 vezes 8 = 72, que é o código ASCII de `H`).

```
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```
Deve imprimir `Hello World!` e uma quebra de linha.

```
++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.
```
Outro Hello World.

Casos de erro:
```
[            -> erro: colchete sem par
]            -> erro: colchete sem par
<            -> erro: ponteiro saiu da fita
```

Eco da entrada: `,[.,]` lê e imprime até acabar a entrada (`echo abc | ./bf eco.bf` imprime `abc`).

## Extras

- Otimizar sequências repetidas: `+++++` vira uma operação "somar 5" em vez de 5 passos
- Comando de depuração `#` que imprime as 10 primeiras células
- Escrever você mesmo um programa em Brainfuck que imprime seu nome
- Fita dinâmica que cresce com `realloc` em vez de ter tamanho fixo
