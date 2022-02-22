#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

// print errors from mymalloc() and myfree() calls
// caller must include \n in msg, if desired
void print_err(char *file, int line, char *msg)
{
    printf("ERROR in %s, line %d: %s", file, line, msg);
}