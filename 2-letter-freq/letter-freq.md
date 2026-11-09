# Projeto 2: frequência de letras

Lê um texto e mostra quantas vezes cada letra aparece, em forma de histograma. Estimativa: **15 a 25 minutos**.

## O que fazer

Programa `freq` que conta as letras de `a` a `z` (sem diferenciar maiúscula de minúscula) em um arquivo ou na entrada padrão.

```
$ printf "Ola mundo\n" | ./freq
a: 1 #
d: 1 #
l: 1 #
m: 1 #
n: 1 #
o: 2 ##
u: 1 #

mais frequente: o (2)
```

## Regras

1. C11, compilar com `gcc -std=c11 -Wall -Wextra -Werror -o freq freq.c`.
2. Um único arquivo `freq.c`.
3. Só libc (`stdio.h`, `ctype.h`, `stdlib.h`).
4. Usar um **array de 26 inteiros** para os contadores. Nada de 26 variáveis soltas.
5. Ler caractere por caractere com `fgetc` (guardar o retorno em `int`, não em `char`).
6. Sem argumento lê de `stdin`. Com argumento abre o arquivo. Se falhar ao abrir: `perror` e `return 1`.
7. Ignorar tudo que não for letra (números, pontuação, espaços). Acentos também são ignorados, é normal.
8. Mostrar **só** as letras que apareceram pelo menos uma vez, em ordem alfabética.
9. Cada `#` representa 1 ocorrência. Se algum contador passar de 50, escalar o histograma para caber (opcional, ver extras).
10. No final, imprimir a letra mais frequente. Em empate, vale a primeira em ordem alfabética.
11. Se nenhuma letra for encontrada, imprimir `nenhuma letra encontrada`.

## Dica de lógica

Transformar letra em índice:

```c
if (isalpha(c)) {
    contagem[tolower(c) - 'a']++;
}
```

Para imprimir, percorrer `i` de 0 a 25 e converter de volta com `'a' + i`.

Cuidado: `isalpha` com acento pode se comportar de forma estranha dependendo do locale. Se `tolower(c) - 'a'` sair de 0..25, você vai escrever fora do array. Garanta com `c >= 'a' && c <= 'z'` depois do `tolower`.

## Testes

```
printf "aaa" | ./freq              # a: 3 ###, mais frequente: a (3)
printf "AaBb" | ./freq             # a: 2, b: 2, mais frequente: a (2)
printf "123 !!!" | ./freq          # nenhuma letra encontrada
printf "" | ./freq                 # nenhuma letra encontrada
./freq /nao/existe                 # erro, status 1
```

## Extras

- Escalar o histograma quando algum contador passar de 50
- Ordenar a saída da mais para a menos frequente
- Mostrar também o percentual de cada letra
