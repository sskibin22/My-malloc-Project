#include <stdio.h>
#include <stdlib.h>
#include "mymalloc_burke.h"


#define MEMSIZE 4096

static char memory[MEMSIZE];

struct header {
    int allocated;
    int size;
    struct header *next;
};

void *mymalloc(size_t size, char *file, int line)
{
    //TODO
    return NULL;
}

void myfree(void *p, char *file, int line)
{
    //TODO
}

int isInitialized() {
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