#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        perror("passe argumentos");
        exit(EXIT_FAILURE);
    }

    int showLines = 0;
    int showWords = 0;
    int showBytes = 0;

    for (int i = 0; i < argc; i++)
    {
        printf("%s \n", argv[i]);

        if (argv[i][0] == '-')
        {

            if (argv[i][1] == 'l')
            {
                printf("flag -l encontrada\n");
                showLines = 1;
            }

            if (argv[i][1] == 'w')
            {
                printf("flag -w encontrada\n");
                showWords = 1;
            }

            if (argv[i][1] == 'c')
            {
                printf("flag -c encontrada\n");
                showBytes = 1;
            }
        }
    }

    printf("linhas: %d | palavras: %d | bytes: %d\n", showLines, showWords, showBytes);

    return 0;
}