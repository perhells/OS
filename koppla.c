#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#define READ 0
#define WRITE 1

int main()
{
    pid_t pidPrintenv, pidFilter;
    int status, pipa[2];

    if(-1 == pipe(pipa)) ; //ERROR

    printf("pipa[0] %d pipa[1] %d\n", pipa[READ], pipa[WRITE]);

    if((pidPrintenv = fork()) == 0) //printenv
    {
        dup2(pipa[WRITE], WRITE);
        close(pipa[READ]);
        close(pipa[WRITE]);
        execlp("printenv", "printenv", NULL);
    }
        
    if((pidPrintenv = fork()) == 0) //printenv
    {
        dup2(pipa[READ], READ);
        close(pipa[READ]);
        close(pipa[WRITE]);
        execlp("./filter", "./filter", NULL);
    }

    close(pipa[READ]);
    close(pipa[WRITE]);
    wait(&status);
    wait(&status);
}
