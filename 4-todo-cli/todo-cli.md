# Projeto 4: lista de tarefas no terminal

Gerenciador de tarefas que guarda tudo em um arquivo de texto. Estimativa: **25 a 40 minutos** (o núcleo fecha em uns 25).

## O que fazer

```
$ ./todo add "estudar ponteiros"
tarefa adicionada.
$ ./todo add "fazer exercicio de arquivos"
tarefa adicionada.
$ ./todo list
1. [ ] estudar ponteiros
2. [ ] fazer exercicio de arquivos
$ ./todo done 1
tarefa 1 concluida.
$ ./todo list
1. [x] estudar ponteiros
2. [ ] fazer exercicio de arquivos
```

## Regras

1. C11, compilar com `gcc -std=c11 -Wall -Wextra -Werror -o todo todo.c`.
2. Um único arquivo `todo.c`.
3. Só libc (`stdio.h`, `stdlib.h`, `string.h`, `errno.h`).
4. Os dados ficam no arquivo `tarefas.txt` no diretório atual. Se não existir, o programa cria quando necessário.
5. Formato do arquivo, uma tarefa por linha: `status|texto`, onde status é `0` (pendente) ou `1` (feita).
   ```
   0|estudar ponteiros
   1|fazer exercicio de arquivos
   ```
6. Comandos via `argv`:
   - `todo add "texto"`: acrescenta uma linha no fim do arquivo (modo `"a"`)
   - `todo list`: lista com numeração a partir de 1 e `[ ]` ou `[x]`
   - `todo done N`: marca a tarefa N como feita
7. Argumentos faltando ou comando desconhecido: imprimir uso em `stderr` e `return 1`.
8. `list` sem arquivo ou com arquivo vazio imprime `nenhuma tarefa.` (não é erro).
9. Texto da tarefa limitado a 200 caracteres. Se passar, truncar ou recusar, mas sem estourar buffer.
10. O texto não pode conter `\n`. Se vier um, recusar.
11. Sempre `fclose` o que `fopen`. Sempre checar se `fopen` retornou `NULL`.
12. Uma função por comando: `cmd_add`, `cmd_list`, `cmd_done`.

## O que é núcleo e o que é extra

- **Núcleo**: `add` e `list` (uns 20 a 25 minutos)
- **Extra principal**: `done N` (mais 15 minutos). Aqui está a parte interessante: não dá para editar uma linha no meio do arquivo, então você lê tudo, altera na memória e reescreve.

## Dica de lógica para `done N`

Duas abordagens:

1. **Array em memória**: ler todas as linhas para `char linhas[100][256]`, mudar o primeiro caractere da linha N-1 para `'1'` e reescrever o arquivo inteiro com `fopen(..., "w")`.
2. **Arquivo temporário**: ler de `tarefas.txt` e escrever em `tarefas.tmp` linha por linha, alterando a linha N, depois `rename("tarefas.tmp", "tarefas.txt")`.

A 2 é mais robusta (se o programa morrer no meio, o original continua intacto). A 1 é mais simples. Escolha uma e anote no código por quê.

Converter `N` de texto para número: `strtol`, igual ao projeto 3. Validar que N está entre 1 e o total de tarefas.

## Testes

```
rm -f tarefas.txt
./todo list                       # nenhuma tarefa.
./todo add "a"                    # tarefa adicionada.
./todo add "b c d"                # tarefa adicionada.
./todo list                       # 1. [ ] a / 2. [ ] b c d
cat tarefas.txt                   # 0|a e 0|b c d
./todo done 2                     # tarefa 2 concluida.
./todo done 9                     # erro, status 1
./todo done abc                   # erro, status 1
./todo                            # uso, status 1
```

## Extras

- `todo undo N` para desmarcar
- `todo rm N` para remover uma tarefa
- `todo list --pendentes` para mostrar só as não concluídas
- Trocar o array fixo por `malloc`/`realloc` para suportar qualquer quantidade de tarefas
