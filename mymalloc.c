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
    // if memory is not initialized, exit immediately
    if (!(freeLL->size)) {
        printf("Attempt to free when memory is not initialized\n");
        return;
    }
    // traverse linked list, free if it makes sense to do so,
    // otherwise generate an error message
    header_t *curr = freeLL;
    while (curr != NULL) {
        // test if p points to a payload; p CANNOT be pointing to a header,
        // nor can it point to an address outside of memory[]
        if (p == curr + 1) {
            if (curr->alloc) {
                curr->alloc = 0;
                coalesce(p);
                return;
            }
            // cannot free a chunk that was freed previously
            else {
                printf("Attempt to free memory that is already free\n");
                return;
            }
        }
        // cannot free memory that was not allocated by malloc, part 1
        // for now, provide a specific error message if p points to a header
        else if (p == curr) {
            printf("Attempt to free a memory chunk that starts at a header\n");
            return;
        }        
        curr = curr->next;
    }
    // cannot free memory that was not allocated by malloc, part 2
    // error message if loop terminates without freeing a chunk on the linked list
    // don't know a foolproof way to distinguish between pointers inside
    // and outside of memory[]
    printf("Attempt to free memory not allocated by malloc()\n");
}