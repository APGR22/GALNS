#include <galns/galns.h>
#include <stdio.h>

int main()
{
    puts("LOG: Init");
    GALNS_Init();

    GALNS_Quit();

    return 0;
}