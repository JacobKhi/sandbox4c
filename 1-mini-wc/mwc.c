#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    FILE *f;

    if (argc < 2)
    {
        f = stdin;
    }
    else
    {
        f = fopen(argv[1], "r");
    }

    if (f == NULL)
    {
        perror("Erro ao ler o arquivo");
        exit(EXIT_FAILURE);
    }

    int currentChar;
    int byteCount = 0;
    int lineCount = 0;
    int wordCount = 0;
    int inWord = 1;
    while ((currentChar = fgetc(f)) != EOF)
    {
        byteCount++;

        if (currentChar == '\n')
        {
            lineCount++;
        }

        if (inWord == 1 && !isspace(currentChar))
        {
            wordCount++;
            inWord = 0;
        }
        else if (isspace(currentChar))
        {
            inWord = 1;
        }
    }

    if (f != stdin) fclose(f);

    printf("%8d %8d %8d", lineCount, wordCount, byteCount);
    if (argc >= 2) printf(" %s", argv[1]);
    printf("\n");

    return 0;
}