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
    char delims[10] = " \n";
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    while(1)
    {
        fgets(line, sizeof(line), stdin);
        tok = strtok(line, delims);

        if(strcmp(tok,"exit")==0)
        {
            return exitKjell(); 
        }
        else if(strcmp(tok,"pwd")==0)
        {
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        }
        else
        {
            while(tok != NULL)
            {
                printf("%s ", tok);
                tok = strtok(NULL, delims);
            }
            printf("\n");
        }
    }   
}
