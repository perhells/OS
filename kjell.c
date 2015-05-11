#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define BUFFERSIZE 100
#define READ 0
#define WRITE 1

int exitKjell()
{
    return 0;
}

int main(int argc, char* argv[], char* envp[])
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
        printf("%s:%s",cwd,prompt);
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
            else if(strcmp(tok,"cd")==0)
            {
                tok = strtok(NULL, delims);
                if(chdir(tok)==0)
                {
                    getcwd(cwd, sizeof(cwd));
                    //printf("%s ", tok);
                }
            }
            else if(strcmp(tok,"checkEnv")==0)
            {
                char *arguments = "";
                printf("checkEnv: %s", arguments);
                tok = strtok(NULL,delims);
                printf("%s",arguments);
                //if(tok != NULL)
                //{
                    //arguments = tok;
                    //printf(".");
                //} 
                
                if(tok == NULL)
                {   
                    pid_t pid;
                    int status, pipa1[2], pipa2[2];
                    char pager[10] = "less";
                    if(-1 == pipe(pipa1));
                    if(-1 == pipe(pipa2));
                    if((pid = fork()) == 0) 
                    {
                        dup2(pipa1[WRITE], WRITE);
                        close(pipa1[READ]);
                        close(pipa1[WRITE]);
                        execlp("printenv", "printenv", NULL);
                    }
                    if((pid = fork()) == 0) 
                    {
                        dup2(pipa1[READ], READ);
                        close(pipa1[READ]);
                        close(pipa1[WRITE]);
                        dup2(pipa2[WRITE], WRITE);
                        close(pipa2[READ]);
                        close(pipa2[WRITE]);
                        execlp("sort", "sort", NULL);
                    }
                    close(pipa1[READ]);
                    close(pipa1[WRITE]);
                    wait(&status);
                    wait(&status);
                    if((pid = fork()) == 0) 
                    {
                        dup2(pipa2[READ], READ);
                        close(pipa2[READ]);
                        close(pipa2[WRITE]);
                        execlp(pager, pager, NULL);
                    }
                    close(pipa2[READ]);
                    close(pipa2[WRITE]);
                    wait(&status);
                    wait(&status);
                }
                else
                {   
                    printf("ja?");
                    char grep[100] = "grep ";
                    strcat(grep, tok);

                    pid_t pid;
                    int status, pipa1[2], pipa2[2], pipa3[2];
                    char pager[10] = "less";
                    if(-1 == pipe(pipa1));
                    if(-1 == pipe(pipa2));
                    if(-1 == pipe(pipa3));
                    if((pid = fork()) == 0) 
                    {
                        printf("1");
                        dup2(pipa1[WRITE], WRITE);
                        close(pipa1[READ]);
                        close(pipa1[WRITE]);
                        execlp("printenv", "printenv", NULL);
                    }
                    if((pid = fork()) == 0) 
                    {
                        printf("2");
                        dup2(pipa1[READ], READ);
                        close(pipa1[READ]);
                        close(pipa1[WRITE]);
                        dup2(pipa2[WRITE], WRITE);
                        close(pipa2[READ]);
                        close(pipa2[WRITE]);
                        execlp(grep, grep , NULL);
                    }
                    close(pipa1[READ]);
                    close(pipa1[WRITE]);
                    wait(&status);
                    wait(&status);
                    if((pid = fork()) == 0) 
                    {
                        printf("3");
                        dup2(pipa2[READ], READ);
                        close(pipa2[READ]);
                        close(pipa2[WRITE]);
                        dup2(pipa3[WRITE], WRITE);
                        close(pipa3[READ]);
                        close(pipa3[WRITE]);
                        execlp("sort", "sort", NULL);
                    }
                    close(pipa2[READ]);
                    close(pipa2[WRITE]);
                    wait(&status);
                    wait(&status);
                    if((pid = fork()) == 0) 
                    {
                        printf("4");
                        dup2(pipa3[READ], READ);
                        close(pipa3[READ]);
                        close(pipa3[WRITE]);
                        execlp(pager, pager, NULL);
                    }
                    close(pipa3[READ]);
                    close(pipa3[WRITE]);
                    wait(&status);
                    wait(&status);
                }
                tok = strtok(NULL, delims);
            }
            else
            {
                tok = strtok(NULL, delims);
            }
        }
    }   
}
