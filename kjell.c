#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFERSIZE 100

int exitKjell()
{
    return 0;
}

int main()
{
    char *tok;
    char line[BUFFERSIZE];
    char cwd[BUFFERSIZE];
    char delims[10] = " \t\n";
    char prompt[10] = "> ";
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    while(1)
    {   
        printf("%s",prompt);
        fgets(line, sizeof(line), stdin);
        tok = strtok(line, delims);
        while(tok != NULL)
        {
            if(strcmp(tok,"exit")==0)
            {
                return exitKjell(); 
            }
            else if(strcmp(tok,"pwd")==0)
            {
                getcwd(cwd, sizeof(cwd));
                printf("%s\n", cwd);
                tok = strtok(NULL, delims);
            }
            else
            {
                printf("%s ", tok);
                tok = strtok(NULL, delims);
            }
        }
        printf("\n");
    }   
}
