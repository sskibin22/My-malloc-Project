#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"


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
    // coalesce the next and previous chunks with the current chunk (pointed to by *p)
    // return the number of chunks coalesced with the current chunk
    int num_coal = 0;

    // try to coalesce the current and next chunks
    header_t *p_header = get_header(p);
    header_t *next_header = p_header->next;

    if (next_header != NULL && !next_header->allocated) {
        p_header->next = next_header->next;
        p_header->size += next_header->size + sizeof(header_t);
        num_coal++;
    }

    // try to coalesce the previous and current chunks
    // start at beginning of memory, traverse linked list until we reach the previous chunk
    header_t *h = (header_t *)memory;
    while (h < p_header) {
        if (h->next == p_header && !h->allocated) {
            h->next = p_header->next;
            h->size += p_header->size + sizeof(header_t);
            num_coal++;
            break;
        }
        else {
            h = h->next;
        }
    }

    return num_coal;
}

header_t *get_header(void *p) {
    // given a pointer to a chunk of memory, return a pointer to that chunk's header
    // TODO: decide whether to test that p points to an address assigned by mymalloc()
    return (header_t *)p - 1;
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