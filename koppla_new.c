#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#define READ 0
#define WRITE 1

void handler(int signum)
{
    fprintf(stderr,"In handler\n");
}

int main()
{
    pid_t pid;
    int status; 

    if((pid = fork()) == 0) //printenv
    {
        int tal;
        signal(SIGCHLD, handler);
        sighold(SIGCHLD);
        puts(">> ");
        scanf("%d", &tal);
        printf("tal = %d\n", tal);
        sigrelse(SIGCHLD);
        exit(0);
    }
    sleep(5);
    kill(pid, SIGCHLD);
    wait(&status);
}
