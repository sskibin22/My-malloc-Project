#include <stdio.h>
#include <stdlib.h>
#include "mymalloc_burke.h"


#define MEMSIZE 4096

static char memory[MEMSIZE];

void *mymalloc(size_t size, char *file, int line)
{
    //TODO
    return NULL;
}

void myfree(void *p, char *file, int line)
{
    //TODO
}

int coalesce(void *p) {
    // coalesce the subsequent and previous chunks with the chunk pointed to by *p
    // return the number of chunks coalesced with the chunk pointed to by *p
    return 1;
}

int is_initialized() {
    // idea: an initialized memory array will always have a header at the beginning
    // with a non-zero size element; an uninitialized memory array should contain all zeroes
    int *p = memory;
    if (*(p + 1) == 0) {
        return 0;
    }
    else {
        return 1;
    }
}