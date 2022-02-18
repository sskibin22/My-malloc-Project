#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main(int argc, char**argv)
{
    int *p = malloc(1024);
    printf("malloc was used to allocate 1024 bytes\n");
    int *q = malloc(500);
    printf("malloc was used to allocate 500 bytes\n");

    free(p);
    printf("memory was freed from the heap\n");

    free(q);
    printf("memory was freed from the heap\n");
    return EXIT_SUCCESS;
}