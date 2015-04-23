#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int exitKjell()
{
    return 0;
}

int main()
{
    char token[100];
    char *tok;
    char line[100];
    char exit[10] = "exit\n";
    char cwd[100];
    char tokenized[2][100];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    fgets(line, sizeof(line), stdin);
    while(strcmp("exit\n",line)!=0)
    {
        printf("%s %d",line,strcmp(exit,line));
        fgets(line, sizeof(line), stdin);
        tok = strtok(line, " ");
        printf("%s\n", tok);
    }   
    return exitKjell();
}
