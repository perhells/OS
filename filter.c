#include <stdio.h>
#include <unistd.h>

int main()
{
    char tkn;
    while ((tkn = getchar()) != EOF)
    {
        putchar(tkn == 'X' ? '#' : tkn);
    }
}
