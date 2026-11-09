# Projeto: mini wc em C

Clone simples do comando `wc`. Estimativa: **15 a 30 minutos**.

## O que fazer

Um programa `mwc` que conta **linhas**, **palavras** e **bytes** de um arquivo ou da entrada padrão.

```
./mwc arquivo.txt
  12  57  310 arquivo.txt

cat arquivo.txt | ./mwc
  12  57  310
```

## Regras

1. Linguagem C11, compilar com `gcc -std=c11 -Wall -Wextra -Werror -o mwc mwc.c`.
2. Um único arquivo `mwc.c`.
3. Só libc (`stdio.h`, `ctype.h`, `stdlib.h`). Nada de `system()`.
4. Ler caractere por caractere com `fgetc`. Nada de carregar o arquivo inteiro na memória.
5. Sem argumento, lê de `stdin`.
6. Com um argumento, abre o arquivo. Se não abrir, `perror` e `return 1`.
7. Saída no formato: linhas, palavras, bytes, e o nome do arquivo (se houver), separados por espaço.
8. Fechar o arquivo com `fclose` ao terminar.

## Definições

- **linha**: cada `\n` encontrado
- **palavra**: sequência de caracteres sem espaço (`isspace` para decidir), separada por espaços, tabs ou quebras de linha
- **byte**: cada caractere lido

## Dica de lógica

Use uma flag `em_palavra`. Ao ler um caractere:
- se `isspace(c)`: `em_palavra = 0`
- senão, se `em_palavra == 0`: `palavras++` e `em_palavra = 1`

## Testes

Compare com o `wc` de verdade:

```
printf "ola mundo\n" | ./mwc          # 1 2 10
printf "" | ./mwc                     # 0 0 0
printf "   \n\n" | ./mwc              # 2 0 5
printf "sem quebra final" | ./mwc     # 0 3 16
./mwc /nao/existe                     # erro, status 1
```

## Extras (se sobrar tempo)

- Flags `-l`, `-w`, `-c` para mostrar só um dos contadores
- Aceitar vários arquivos e mostrar uma linha `total` no final
