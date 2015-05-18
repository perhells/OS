#define _POSIX_SOURCE
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

/* Function that kills all process in the current processgroup */
int exitKjell()
{
    signal(SIGQUIT, SIG_IGN);   /* Ignore the quit signal for the shell */
    kill(0, SIGQUIT);           /* Send the quit signal to all processes in group */
    exit(0);                    /* Exits shell */
}

/* A signal handler for background processes */
void sandler(int signum)
{   
    int pid = waitpid(-1,NULL,WNOHANG); /* Returns the process id of a background process if it has terminated, else 0 or -1 */
    if(pid > 0) /* Checks if a process has terminated */
    {
        fprintf(stderr,"Process with pid %d has terminated\n",pid); /* Prints a message */
    }
}

/* A handler for ctrl-C interrupts */
void intHandler(int signum)
{
    kill(signum, SIGCHLD); /* Kill the process in which the command was issued */
    while(waitpid(-1,NULL,WNOHANG)>0){} /* Kill the process */
}

void intKjell(int signum)
{
    fseek(stdin,0,SEEK_END);
}

/* Main function */
int main(int argc, char* argv[], char* envp[])
{
    struct sigaction sa;
    FILE *fr;                                   /* Used for reading the hostname */
    char *tok;                                  /* Holds current token when parsing a line */
    char line[BUFFERSIZE];                      /* Line from input */
    char cwd[BUFFERSIZE];                       /* Current working directory */
    char delims[10] = " \t\n";                  /* Delimiters for tokens */
    char prompt[10] = "> ";                     /* Prompt after user/host/cwd info */
    char pager[BUFFERSIZE] = "less";            /* Which pager to use, less is default */
    char hostname[BUFFERSIZE];                  /* Machines hostname */
    char username[BUFFERSIZE];                  /* Username */
    int status, pipa1[2], pipa2[2], pipa3[2];   /* Status for wait, all three pipes used in checkEnv */
    int foreground;                             /* Keeps track of whether a process should be executed in foreground */
    char ** res;                                /* Argument list */
    char * p;                                   /* Used when tokenizing arguments */
    int n_spaces = 0;                           /* Counting arguments */
    pid_t pid;                                  /* Process ID */
    struct timeval tvalBefore, tvalAfter;       /* Used to measure time of processes */

    sigemptyset(&sa.sa_mask);
    strcpy(username,getenv("USER"));            /* Reads environment variable USER to username */
    if(getenv("PAGER")!=NULL)                   /* If environment variable PAGER exists */
    {
        strcpy(pager,getenv("PAGER"));          /* Overwrite less with current PAGER */
    }
    fr = fopen ("/etc/hostname", "rt");         /* Open file that contains hostname */
    if(fgets(hostname, BUFFERSIZE, fr)==NULL)   /* Reads hostname */
    {
        fprintf(stderr,"fgets failed\n");
    }
    strcpy(hostname,strtok(hostname,"\n"));     /* Remove trailing newline */
    fclose(fr);                                 /* Close file */
    if(getcwd(cwd, sizeof(cwd))==NULL)                   /* Read current working directory */
    {
        fprintf(stderr,"getcwd failed\n");
    }

    /* Main loop */
    while(1)    
    {   
        signal(SIGINT,intKjell);    /* Register interrupt handler for ctrl-C */
        tok = NULL;                 /* Reset token */
        strcpy(line,"");            /* Reset input line */
        if(!SIGDET)     /* Checks if polling of background processes should be used */
        {
            pid = waitpid(-1, &status, WNOHANG);    /* Checks if any background process has terminated */
            while(pid > 0)                          /* While something has terminated */
            {
                printf("Process with pid %d has terminated.\n", pid);   /* Print it's process ID */
                pid = waitpid(-1, &status, WNOHANG);                    /* Check if there is another procees which has terminated */
            }
        }
        printf("[%s@%s %s] %s", username, hostname, cwd, prompt);   /* Print full prompt */
        if(fgets(line, sizeof(line), stdin)==NULL);   /* Read input */
        tok = strtok(line, delims);         /* Get first token */
        while(tok != NULL)                  /* If there is a token */
        {   
            if(strcmp(tok,"exit")==0)       /* If token is exit */
            {
                return exitKjell();         /* Call exit function */
            }
            else if(strcmp(tok,"pwd")==0)   /* If token is pwd */
            {
                tok = strtok(NULL, delims);     /* Check next token */
                if(strtok(NULL,delims) == NULL) /* Check that it is NULL */
                {
                    if(getcwd(cwd, sizeof(cwd))==NULL)   /* Read current working directory */
                    {
                        fprintf(stderr,"getcwd failed\n");
                    }
                    printf("%s\n", cwd);        /* Print it */
                }
                else    /* pwd doesn't take any arguments */
                {
                    printf("Too many arguments!\n");
                    break;
                }
            }
            else if(strcmp(tok,"cd")==0)    /* If token is cd */
            {
                tok = strtok(NULL, delims); /* Read next token */
                if(strtok(NULL,delims) == NULL && chdir(tok)==0)    /* Check that next token is NULL, and directory change to token is okay */
                {
                    if(getcwd(cwd, sizeof(cwd))==NULL)   /* Update current working directory */
                    {
                        fprintf(stderr,"getcwd failed\n");
                    }
                }
                else    /* Too many arguments, or invalid directory */
                {
                    printf("Failed to change directory!\n");
                    break;
                }
            }
            else if(strcmp(tok,"checkEnv")==0)  /* If token is checkEnv */
            {   
                tok = strtok(NULL, delims);     /* Read next token */
                if(tok == NULL)                 /* Check if any arguments is given */
                {   
                    if(-1 == pipe(pipa1));          /* Create first pipe */
                    if(-1 == pipe(pipa2));          /* Create second pipe */
                    if((pid = fork()) == 0)         /* Fork and execute in child process */
                    {
                        dup2(pipa1[WRITE], WRITE);  /* stdout writes to first pipe */
                        close(pipa1[READ]);         
                        close(pipa1[WRITE]);
                        if(execlp("printenv", "printenv", NULL)==-1)fprintf(stderr,"\"printenv\" failed\n");    /* Try to execute printenv */
                    }
                    else if(pid == -1)              /* If fork returns -1 it failed */
                    {
                        fprintf(stderr,"Fork failed");
                    }
                    if((pid = fork()) == 0)         /* Fork and execute in child process */
                    {
                        dup2(pipa1[READ], READ);    /* stdin reads from first pipe */
                        close(pipa1[READ]);
                        close(pipa1[WRITE]);
                        dup2(pipa2[WRITE], WRITE);  /* stdout writes to second pipe */
                        close(pipa2[READ]);
                        close(pipa2[WRITE]);
                        if(execlp("sort", "sort",  NULL)==-1)fprintf(stderr,"\"sort\" failed\n");   /* Try to execute sort */
                    }
                    else if(pid == -1)              /* If fork returns -1 it failed */
                    {
                        fprintf(stderr,"Fork failed");
                    }
                    close(pipa1[READ]);     /* Done using first pipe */
                    close(pipa1[WRITE]);    /* Done using first pipe */
                    if((pid = fork()) == 0)         /* Fork and execute in child process */
                    {
                        dup2(pipa2[READ], READ);    /* stdin reads from second pipe */
                        close(pipa2[READ]);         
                        close(pipa2[WRITE]);
                        if(execlp(pager, pager, NULL) == -1)        /* If pager cannot be executed */
                        {
                            if(execlp("less","less",NULL) == -1)    /* Try less */
                            {
                                if(execlp("more","more",NULL)==-1)fprintf(stderr,"No pager found, \"less\" and \"more\" failed");   /* Finally try more */
                            }
                        }
                    }
                    else if(pid == -1)              /* If fork returns -1 it failed */
                    {
                        fprintf(stderr,"Fork failed");
                    }
                    close(pipa2[READ]);
                    close(pipa2[WRITE]);
                    wait(&status);                  /* Wait for first process */
                    wait(&status);                  /* Wait for second process */
                    wait(&status);                  /* Wait for third process */
                }
                else
                {   
                    res  = NULL;    /* Make sure there are no old arguments */
                    p = tok;        /* First argument */
                    n_spaces = 0;   /* Reset argument count */
                    
                    res = realloc (res, sizeof (char*) * ++n_spaces); /* Allocate memory for first argument */
                    if (res == NULL)    /* If res still is NULL, memory allocation failed */
                    {
                        fprintf(stderr, "Error while allocating memory");
                    }
                    res[n_spaces-1] = "grep"; /* First argument should be grep */
                    
                    while (p) { /* While there is an argument */
                        res = realloc (res, sizeof (char*) * ++n_spaces);   /* Allocate memory for it */

                        if (res == NULL)    /* If res is NULL, memory allocation failed */
                        {
                            fprintf(stderr, "Error while allocating memory");
                        }
                        
                        res[n_spaces-1] = p;        /* Add current argument to list of arguments */
                        
                        p = strtok (NULL, delims);  /* Read next argument */
                    }
                    
                    /* Allocate one extra element for the last NULL */
                    
                    res = realloc (res, sizeof (char*) * (n_spaces+1)); /* Allocate space for the last element */
                    res[n_spaces] = NULL;   /* Arguments should be NULL terminated */

                    if(-1 == pipe(pipa1));  /* Create first pipe */
                    if(-1 == pipe(pipa2));  /* Create second pipe */
                    if(-1 == pipe(pipa3));  /* Create third pipe */
                    if((pid = fork()) == 0) /* Fork and execute in child process */
                    {
                        dup2(pipa1[WRITE], WRITE);  /* stdout writes to first pipe */
                        close(pipa1[READ]);
                        close(pipa1[WRITE]);
                        if(execlp("printenv", "printenv",NULL)==-1)fprintf(stderr,"\"printenv\" failed\n");    /* Try to execute printenv */
                    }
                    else if(pid == -1)  /* If fork returns -1 it has failed */
                    {
                        fprintf(stderr,"Fork failed");
                    }
                    if((pid = fork()) == 0) /* Fork and execute in child process */
                    {
                        dup2(pipa1[READ], READ);    /* stdin reads from first pipe */
                        close(pipa1[READ]);
                        close(pipa1[WRITE]);
                        dup2(pipa2[WRITE], WRITE);  /* stdout writes to second pipe */
                        close(pipa2[READ]);
                        close(pipa2[WRITE]);
                        if(execvp(res[0], res)==-1)fprintf(stderr,"\"grep\" failed\n"); /* Try to execute grep with all arguments */
                    }
                    else if(pid == -1)  /* If fork returns -1 it has failed */
                    {
                        fprintf(stderr,"Fork failed");
                    }
                    close(pipa1[READ]);
                    close(pipa1[WRITE]);
                    if((pid = fork()) == 0) /* Fork and execute in child process */
                    {
                        dup2(pipa2[READ], READ);    /* stdin reads from second pipe */
                        close(pipa2[READ]);
                        close(pipa2[WRITE]);
                        dup2(pipa3[WRITE], WRITE);  /* stdout writes to third pipe */
                        close(pipa3[READ]);
                        close(pipa3[WRITE]);
                        if(execlp("sort", "sort",  NULL)==-1)fprintf(stderr,"\"sort\" failed\n");   /* Try to execute sort */
                    }
                    else if(pid == -1)  /* If fork returns -1 is has failed */
                    {
                        fprintf(stderr,"Fork failed");
                    }
                    close(pipa2[READ]);
                    close(pipa2[WRITE]);
                    if((pid = fork()) == 0) /* Fork and execute in child process */
                    {
                        dup2(pipa3[READ], READ);    /* stdin reads from third pipe */
                        close(pipa3[READ]);
                        close(pipa3[WRITE]);
                        if(execlp(pager, pager, NULL) == -1)        /* If pager cannot be executed */
                        {
                            if(execlp("less","less",NULL) == -1)    /* Try less */
                            {
                                if(execlp("more","more",NULL)==-1)fprintf(stderr,"No pager found, \"less\" and \"more\" failed");   /* Finally try more */
                            }
                        }
                    }
                    else if(pid == -1)  /* If fork returns -1 it has failed */
                    {
                        fprintf(stderr,"Fork failed");
                    }
                    close(pipa3[READ]);
                    close(pipa3[WRITE]);                    
                    wait(&status);  /* Wait for the first process */
                    wait(&status);  /* Wait for the second process */
                    wait(&status);  /* Wait for the third process */
                    wait(&status);  /* Wait for the fourth process */

                    /* Free the memory allocated for arguments */
                    
                    free (res);
                }
                tok = NULL; /* Make sure there are no arguments still in tok */
            }
            else
            {   
                res = NULL;     /* Reset argument list */
                p = tok;        /* Read first argument */
                n_spaces = 0;   /* Reset argument count */
                foreground = 1; /* Process should run in foreground by default */
                
                while (p) {     /* While there is at least one more argument */
                    tok = strtok(NULL,delims);  /* Read next token */
                    if(strcmp(p,"&")==0 && tok == NULL) /* If the argument is & and it is the last argument*/
                    {
                        foreground = 0;             /* The process should execute in the background */
                        p = NULL;                   /* Loop is finished */
                    }

                    res = realloc (res, sizeof (char*) * ++n_spaces); /* Allocate space for one more token */
                    if (res == NULL)    /* If res is NULL allocation has failed */
                    {
                        fprintf(stderr, "Error while allocating memory");
                    }
                    
                    res[n_spaces-1] = p;    /* Set element to token */
                        
                    p = tok;                /* Set token to next token */
                }
                
                /* Allocate memory for one extra element for the last NULL */
                res = realloc (res, sizeof (char*) * (n_spaces+1));
                res[n_spaces] = NULL;

                if(foreground)  /*If the process should be run in the foreground */
                {
                    gettimeofday (&tvalBefore, NULL);   /* Save the starting time */
                }
                if(SIGDET)  /* If signal handler should be used instead of polling */
                {   
                    sa.sa_flags = 0;
                    sa.sa_handler = sandler;
                    if(sigaction(SIGCHLD,&sa,NULL)==-1)fprintf(stderr,"sigaction failed\n");  /* Register signal handler for SIGCHLD */
                    if(foreground)signal(SIGINT,intHandler);    /* Register interrupt handler */
                    pid = fork();   /* Try to fork */
                    if(pid == -1)   /* If fork returns -1 it has failed */
                    {
                        perror("Fork failed");
                        exit(1);
                    }
                    else if(pid == 0)       /* Execute in child process */
                    {   
                        int exitStatus = 0; /* Default exit status is 0 */
                        if(execvp(res[0],res)==-1)  /* If execution fails */
                        {
                            exitStatus = 1; /* Set exit status to 1 */
                        }
                        exit(exitStatus);   /* Return exit status */
                    }
                    else if(foreground)     /* If the process should be executed in foreground */
                    {
                        waitpid(pid,&status,0); /* Wait for it to finish */
                    }
                }
                else    /* If polling should be used */
                {
                    if(foreground)signal(SIGINT,intHandler);    /* Register interrupt handler for sigchld */
                    pid = fork();   /* Try to fork */
                    if(pid == -1)   /* If fork returns -1 it has failed */
                    {
                        perror("Fork failed");
                        exit(1);
                    }
                    else if(pid == 0)   /* Execute in child process */
                    {   
                        int exitStatus = 0; /* Default exit status is 0 */
                        if(execvp(res[0],res)==-1)  /* If execution fails */
                        {
                            exitStatus = 1; /* Set exit status to 1 */
                        }
                        exit(exitStatus);   /* Return exit status */
                    }
                    else if(foreground) /* If the process should be executed in foreground */
                    {   
                        waitpid(pid,&status,0); /* Wait for it to finish */
                    }
                }
                if(foreground)  /* If the process was executed in foreground */
                {
                    gettimeofday (&tvalAfter, NULL);    /* Get the stop time */
                    printf("Time in microseconds: %ld microseconds\n",((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L+tvalAfter.tv_usec) - tvalBefore.tv_usec);    /* Print the execution time */
                }
                free(res);      /* Free the memory used */
                foreground = 1; /* Reset foreground variable */
                tok = NULL;     /* Reset tok */
            }
        }
    }   
}
