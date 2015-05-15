#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#define BUFFERSIZE 80
#define READ 0
#define WRITE 1
#ifndef SIGDET
#define SIGDET 1
#endif

FILE *fr;

int exitKjell()
{
    signal(SIGQUIT, SIG_IGN);
    kill(0, SIGQUIT);
    exit(0);
}

void sandler(int signum)
{   
    int pid = waitpid(-1,NULL,WNOHANG);
    if(pid > 0)
    {
        fprintf(stderr,"Process with pid %d has terminated\n",pid);
    }
}

void intHandler(int signum)
{
    kill(signum, SIGCHLD);
    while(waitpid(-1,NULL,WNOHANG)>0){}
}

int main(int argc, char* argv[], char* envp[])
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

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
    int foreground;
    char ** res;
    char * p;
    int n_spaces = 0;
    pid_t pid;
    struct timeval tvalBefore, tvalAfter;
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
        if(!SIGDET)
        {
            pid = waitpid(-1, &status, WNOHANG);
            if(pid > 0)
            {
                printf("Process with pid %d has terminated.\n", pid);
            }
        }
        printf("[%s@%s %s] %s", username, hostname, cwd, prompt);
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
                foreground = 1;
                
                while (p) {
                    if(strcmp(p,"&")==0)
                    {
                        p = strtok(NULL,delims);
                        foreground = 0;
                    }
                    else
                    {
                        res = realloc (res, sizeof (char*) * ++n_spaces);
                        if (res == NULL)
                        {
                            printf("FEL");
                        }
                        
                        res[n_spaces-1] = p;
                        
                        p = strtok (NULL, delims);
                    }
                }
                
                /* realloc one extra element for the last NULL */
                
                res = realloc (res, sizeof (char*) * (n_spaces+1));
                res[n_spaces] = NULL;
                if(foreground)
                {
                    gettimeofday (&tvalBefore, NULL);
                }
                if(SIGDET)
                {   
                    memset(&sa, 0, sizeof(sa));
                    sa.sa_handler = sandler;
                    //if(sigaction(SIGCHLD,&sa,0))
                    //{
                        //perror("sigaction");
                        //return 1;
                    //}
                    if(!foreground)signal(SIGCHLD,sandler);
                    signal(SIGINT,intHandler);
                    pid = fork();
                    if(pid == -1)
                    {
                        perror("Fork failed");
                        exit(1);
                    }
                    else if(pid == 0)
                    {   
                        int exitStatus = 0;
                        if(execvp(res[0],res)==-1)
                        {
                            exitStatus = 1;
                        }
                        exit(exitStatus);
                    }
                    else if(foreground)
                    {   
                        waitpid(pid,&status,0);
                    }
                }
                else
                {
                    pid = fork();
                    if(pid == -1)
                    {
                        perror("Fork failed");
                        exit(1);
                    }
                    else if(pid == 0)
                    {   
                        int exitStatus = 0;
                        if(execvp(res[0],res)==-1)
                        {
                            exitStatus = 1;
                        }
                        exit(exitStatus);
                    }
                    else if(foreground)
                    {   
                        waitpid(pid,&status,0);
                    }
                }
                if(foreground)
                {
                    gettimeofday (&tvalAfter, NULL);
                    printf("Time in microseconds: %ld microseconds\n",((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L+tvalAfter.tv_usec) - tvalBefore.tv_usec);
                }
                free(res);
                foreground = 0;
                tok = strtok(NULL, delims);
            }
        }
    }   
}
