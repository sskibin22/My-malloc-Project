#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

//define total byte size of heap
#define MEMSIZE 4096

//declare contiguous array that acts as the 'heap'. Set it's size equal to MEMSIZE
static char memory[MEMSIZE];

//Points to the main block of memory which is initially free
header_t *freeLL = (void*)memory;

// given a pointer to a chunk of memory, return a pointer to that chunk's header
header_t *get_header(void *p) {
    // TODO: decide whether to test that p points to an address assigned by mymalloc()
    return (header_t*)p - 1;
}

int is_initialized() {
    // idea: an initialized memory array will always have a header at the beginning
    // with a non-zero size element; an uninitialized memory array should contain all zeroes
    if (*((size_t*)memory) == 0) {
        return 0;
    }
    else {
        return 1;
    }
}
//initialize first header and point it to the next block in memory
void initialize(){
    freeLL->size = MEMSIZE - sizeof(header_t);
    freeLL->alloc = 0;
    freeLL->next = NULL;
}
/*Use best-fit alg to traverse LL and find smallest free chunk large enough to fit requested memory size.
    loop ends when at the end of the list and conditions for requested memory are not met*/
header_t *best_fit_search(header_t *curr, header_t *prev, size_t size_req){
    header_t *best_fit;
    if (curr->size >= size_req && curr->alloc == 0){
            best_fit = curr;
        }
    prev = curr;
    curr = curr->next;
    while(curr->next != NULL){
        if ((curr->size >= size_req && curr->size < prev->size) && curr->alloc == 0){
            best_fit = curr;
        }
        prev = curr;
        curr = curr->next;
    }
    return (header_t *)best_fit;
}
//splits a free block that is larger then the size requested
void split(header_t *alloc_split, size_t size_req){
    header_t *free_split = (void*)((void*)alloc_split + size_req + sizeof(header_t)); //(void*) not neccessary 
    free_split->size = (alloc_split->size) - size_req - sizeof(header_t);
    free_split->alloc = 0;
    free_split->next = alloc_split->next;
    alloc_split->size = size_req;
    alloc_split->alloc = 1;
    alloc_split->next = free_split;
}

void *mymalloc(size_t size_req, char *file, int line){
    header_t *curr, *prev = NULL;
    void *result;
    //if mymalloc() is not initilized, initilize it
    if(!(freeLL->size)){
        initialize();
        printf("Memory initalized\n");
    }
    //point temporary curr pointer to start of initilized free linked list
    curr = freeLL;
    //call best-fit algorithm
    header_t *best_fit = best_fit_search(curr, prev, size_req);
    /*if size of chunk found in LL is equal to size of requested memory
    return pointer to the whole chunk*/
    if ((best_fit->size) == size_req){
        best_fit->alloc = 1;
        result = (void*)(best_fit+1);
        printf("Exact fitting block allocated\n");
        return result;
    }
    /*else if size of chunk found in LL is larger then requested memory size 
    call split method and return pointer to allocated memory chunk*/
    else if(best_fit->size > (size_req + sizeof(header_t))){
        split(best_fit, size_req);
        result = (void*)(best_fit+1);
        printf("Fitting block allocated with split\n");
        return result;
    }
    //else if conditions are not met return NULL
    else{
        printf("No sufficient memory to allocate\n");
        return NULL;
    }
}

int coalesce(void *p) {
    // coalesce the next and previous chunks with the current chunk (pointed to by *p)
    // return the number of chunks coalesced with the current chunk
    int num_coal = 0;

    // try to coalesce the current and next chunks
    header_t *p_header = get_header(p);
    header_t *next_header = p_header->next;

    if (next_header != NULL && !next_header->alloc) {
        p_header->next = next_header->next;
        p_header->size += next_header->size + sizeof(header_t);
        num_coal++;
    }

    // try to coalesce the previous and current chunks
    // start at beginning of memory, traverse linked list until we reach the previous chunk
    header_t *h = (header_t*)memory;
    while (h < p_header) {
        if (h->next == p_header && !h->alloc) {
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

void myfree(void *p, char *file, int line)
{
    if((void*)memory <= p && p <= (void*)(memory+MEMSIZE)){
        header_t *curr = p;
        --curr;
        curr->alloc = 0;
        //coalesce(p);
    }
    else{
        printf("Please provide a valid pointer allocated by mymalloc()\n");
    }
}