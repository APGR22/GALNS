#include <galns/galns.h>
#include <stdio.h>

int main()
{
    puts("LOG: Init");
    GALNS_Init();

    puts("LOG: Set Error Test");
    GALNS_SetErrorMsg("Error berhasil!");

    printf("Test: %s\n", GALNS_GetErrorMsg());

    GALNS_Quit();

    return 0;
}