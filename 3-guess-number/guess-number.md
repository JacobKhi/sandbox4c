# Projeto 3: adivinhe o número

Jogo no terminal. O computador sorteia um número e você tenta acertar. Estimativa: **20 a 30 minutos**.

## O que fazer

```
$ ./adivinha
Adivinhe o numero entre 1 e 100. Voce tem 7 tentativas.
Tentativa 1: 50
Maior.
Tentativa 2: 75
Menor.
Tentativa 3: abc
Entrada invalida, digite um numero entre 1 e 100.
Tentativa 3: 63
Acertou em 3 tentativas!
Jogar de novo? (s/n): n
```

## Regras

1. C11, compilar com `gcc -std=c11 -Wall -Wextra -Werror -o adivinha adivinha.c`.
2. Um único arquivo `adivinha.c`.
3. Só libc (`stdio.h`, `stdlib.h`, `string.h`, `time.h`, `errno.h`).
4. Sortear com `rand()` e `srand(time(NULL))`. **Chamar `srand` só uma vez**, no início do programa.
5. Número sorteado entre 1 e 100 (inclusive). Máximo de 7 tentativas.
6. **Proibido usar `scanf` para ler o palpite.** Ler a linha com `fgets` e converter com `strtol`. Isso é o ponto principal do exercício.
7. Entrada inválida (letra, vazio, fora de 1..100) **não gasta tentativa** e mostra uma mensagem.
8. Dar a dica `Maior.` ou `Menor.` a cada palpite errado.
9. Ao acertar, mostrar em quantas tentativas. Ao perder, mostrar o número sorteado.
10. Perguntar se quer jogar de novo. `s` repete, qualquer outra coisa encerra.
11. Se `fgets` retornar `NULL` (Ctrl+D), encerrar o programa de forma limpa.
12. Funções separadas: pelo menos `ler_inteiro(...)` e `jogar(void)`.

## Dica de lógica

Uma função que lê um inteiro com segurança:

```c
// retorna 1 se leu um numero valido, 0 se a entrada foi invalida, -1 se EOF
int ler_inteiro(long *saida);
```

Passos dentro dela: `fgets` no buffer, remover o `\n`, `strtol` com `endptr`, checar se `endptr` avançou e se chegou no `'\0'`, checar `errno`. Detalhes na seção de `strtol` do md de ajuda.

Note que `saida` é um **ponteiro**, porque a função precisa devolver o número por ela (e o retorno já está sendo usado para o status).

## Testes (manuais)

```
digitar "abc"        -> entrada invalida, nao gasta tentativa
digitar ""  (enter)  -> entrada invalida
digitar "0" e "101"  -> entrada invalida
digitar "5x"         -> entrada invalida
digitar " 42 "       -> aceito ou invalido, mas decida e seja consistente
Ctrl+D               -> encerra sem travar
errar 7 vezes        -> mostra o numero sorteado
```

Dica para testar sem ficar adivinhando: temporariamente fixe `srand(1)` e imprima o número sorteado.

## Extras

- Níveis de dificuldade (1 a 50 com 6 tentativas, 1 a 1000 com 10 tentativas)
- Guardar o melhor resultado (menos tentativas) em um arquivo `recorde.txt`
- Modo invertido: você pensa em um número e o computador tenta adivinhar com busca binária
