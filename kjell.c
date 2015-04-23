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
    //fgets(line, sizeof(line), stdin);
    while(1)
    {
        //printf("%s %d",line,strcmp(exit,line));
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


        //if(strcmp(tok,"exit")==0)
        //{
            //return exitKjell();
        //}
        //if(
        //while(tok != NULL){
            //printf("%s\n", tok);
            //tok = strtok(NULL, delims);
        //}
    }   
}
