#include <stdio.h>
#include <stdlib.h>
#define BUFFERSIZE 20
int main()
{
    char buf[BUFFERSIZE];

    puts(fgets(buf, sizeof(buf), stdin));
}
