#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

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
        perror("erro input");
        exit(EXIT_FAILURE);
    }

    int letterCount[26];
    int currentChar;
    while ((currentChar = fgetc(f)) != EOF)
    {
        if (isalpha(currentChar))
        {
            letterCount[tolower(currentChar) - 'a']++;
        }
    }

    for (int i = 0; i < 26; i++)
    {
        if (letterCount[i] != 0)
        {
            printf("%c ", 'a' + i);
            if (letterCount[i] > 10)
            {   
                printf("[");
                for (int j = 0; j < 10; j++)
                {
                    printf("#");
                }

                printf("]x%d", letterCount[i]);
            }

            printf("\n");
        }
    }

    return 0;
}