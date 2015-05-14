#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#define BUFFERSIZE 100
#define READ 0
#define WRITE 1

FILE *fr;

int exitKjell()
{
    return 0;
}

void handler(int signum)
{
    fprintf(stderr,"In handler\n");
}

int main(int argc, char* argv[], char* envp[])
{
    char *tok;
    char line[BUFFERSIZE];
    char cwd[BUFFERSIZE];
    char delims[10] = " \t\n";
    char prompt[10] = "> ";
    char pager[BUFFERSIZE] = "less";
    char hostname[BUFFERSIZE];
    char username[BUFFERSIZE];
    char arguments[BUFFERSIZE];
    int status, pipa1[2], pipa2[2], pipa3[2];
    char ** res;
    char * p;
    int n_spaces = 0;
    pid_t pid;
    clock_t start, stop;
    strcpy(username,getenv("USER"));
    if(getenv("PAGER")!=NULL)
    {
        strcpy(pager,getenv("PAGER"));
    }
    fr = fopen ("/etc/hostname", "rt");
    fgets(hostname, BUFFERSIZE, fr);
    strcpy(hostname,strtok(hostname,"\n"));
    fclose(fr);
    getcwd(cwd, sizeof(cwd));
    while(1)
    {   
        printf("[%s@%s %s] %s", username, hostname, cwd, prompt);
        fgets(line, sizeof(line), stdin);
        start = clock();
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
                if(chdir(tok)==0 && strtok(NULL,delims) == NULL)
                {
                    getcwd(cwd, sizeof(cwd));
                }
                else
                {
                    printf("Failed to change directory!\n");
                    break;
                }
            }
            else if(strcmp(tok,"checkEnv")==0)
            {   
                tok = strtok(NULL, delims);
                if(tok == NULL)
                {   
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
                        execlp("sort", "sort",  NULL);
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
                        if(execlp(pager, pager, NULL) == -1)
                        {
                            if(execlp("less","less",NULL) == -1)
                            {
                                execlp("more","more",NULL);
                            }
                        }
                    }
                    close(pipa2[READ]);
                    close(pipa2[WRITE]);
                    wait(&status);
                    wait(&status);
                }
                else
                {   
                    res  = NULL;
                    p = tok;
                    n_spaces = 0;
                    
                    res = realloc (res, sizeof (char*) * ++n_spaces);
                    if (res == NULL)
                    {
                        printf("FEL");
                    }
                    res[n_spaces-1] = "grep";
                    
                    while (p) {
                        res = realloc (res, sizeof (char*) * ++n_spaces);

                        if (res == NULL)
                        {
                            printf("FEL");
                        }
                        
                        res[n_spaces-1] = p;
                        
                        p = strtok (NULL, delims);
                    }
                    
                    /* realloc one extra element for the last NULL */
                    
                    res = realloc (res, sizeof (char*) * (n_spaces+1));
                    res[n_spaces] = NULL;

                    strcpy(arguments,tok);
                    tok = strtok(NULL,delims);
                    while (tok != NULL)
                    {
                        strcat(arguments, " ");
                        strcat(arguments, tok);
                        tok = strtok(NULL,delims);
                    }

                    if(-1 == pipe(pipa1));
                    if(-1 == pipe(pipa2));
                    if(-1 == pipe(pipa3));
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
                        execvp(res[0], res);
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
                        dup2(pipa3[READ], READ);
                        close(pipa3[READ]);
                        close(pipa3[WRITE]);
                        if(execlp(pager, pager, NULL) == -1)
                        {
                            if(execlp("less","less",NULL) == -1)
                            {
                                execlp("more","more",NULL);
                            }
                        }
                    }
                    close(pipa3[READ]);
                    close(pipa3[WRITE]);
                    wait(&status);
                    wait(&status);
                    
                    /* free the memory allocated */
                    
                    free (res);
                }
                tok = strtok(NULL, delims);
            }
            else
            {   
                res = NULL;
                p = tok;
                n_spaces = 0; 
                
                while (p) {
                    res = realloc (res, sizeof (char*) * ++n_spaces);
                    if (res == NULL)
                    {
                        printf("FEL");
                    }
                    
                    res[n_spaces-1] = p;
                    
                    p = strtok (NULL, delims);
                }
                
                /* realloc one extra element for the last NULL */
                
                res = realloc (res, sizeof (char*) * (n_spaces+1));
                res[n_spaces] = NULL;
                if((pid = fork()) == 0)
                {
                    execvp(tok,res); 
                    exit(0);
                }
                wait(&status);
                tok = strtok(NULL, delims);
            }
        }
        stop = clock();
        printf("%6.3f", start);
        printf("\n\n%6.3f", stop);

        printf("Execution took: %d\n",stop-start);
    }   
}
