#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

//define total byte size of heap
#define MEMSIZE 4096

//declare contiguous array that acts as the 'heap'. Set it's size equal to MEMSIZE
static char memory[MEMSIZE];

//global variable to keep track of how many chunks(free or not) are in the heap
int chunk_count = 0;

//Points to the main block of memory which is initially free
header_t *freeLL = (void*)memory;

// print information on contents of linked list
int print_LL_contents() {
    header_t *p = freeLL;
    int node = 0;
    if(!freeLL->size) {
        printf("Memory not allocated\n\n");
        return EXIT_FAILURE;
    }
    while (p != NULL) {
        printf("Node %d\n", node);
        printf("********\n");
        printf("Size: %d\n", p->size);
        printf("Allocated: %d\n", p->alloc);
        printf("Next node: ");
        if (p->next != NULL) {
            printf("%d bytes ahead\n", (char *)p->next - (char *)p);
        }
        else {
            printf("NULL\n");
        }
        p = p->next;
        node++;
        printf("\n");
    }
    return EXIT_SUCCESS;
}

// get free memory from linked list
int mem_diagnostics(enum diagnostic diag) {
    header_t *p = freeLL;
    int accum = 0;
    if(!freeLL->size) {
        printf("Memory not allocated\n\n");
        return -1;
    }
    while (p != NULL) {
        switch (diag) {
            case memory_free:
                if (!p->alloc) accum += p->size;
                break;
            case total_payload:
                accum += p->size;
                break;
            case free_chunks:
                if (!p->alloc) accum++;
                break;
            case total_chunks:
                accum++;
                break;
            default:
                printf("Invalid diagnostic selected\n");
                return -1;
        }
        p = p->next;
    }
    return accum;
}

//initialize first header and point it to the next block in memory
void initialize(){
    freeLL->size = MEMSIZE - sizeof(header_t);
    freeLL->alloc = 0;
    freeLL->next = NULL;
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
    chunk_count += 1;
}

void *mymalloc(size_t size_req, char *file, int line){
    header_t *curr, *prev, *best_fit;
    void *result; 
    //if byte size requested is less then 1 report error return NULL
    if (size_req < 1) {
        printf("mymalloc() can not allocate less than 1 byte\n");
        return NULL;
    }
    //if mymalloc() is not initilized, initilize it
    if(!(freeLL->size)){
        initialize();
        printf("Memory initalized\n");
        chunk_count = 1;
    }
    //point curr to first header in the heap then point best_fit pointer to curr
    curr = freeLL;
    best_fit = curr;
    //while there are more then one chunks in the heap conduct firt-fit alg to traverse heap
    while((curr->size < size_req || curr->alloc == 1) && (curr->next != NULL)){
        prev=curr;
        curr=curr->next;
    }
    //set best-fit pointer equal to the first chunk that is large enough to fit the requested byte size if curr chunk is free
    if (curr->alloc == 0) best_fit = curr;
    else best_fit = NULL;
    //while curr/best_fit are not the last chunks in the heap continue traversing the heap
    //use best-fit alg to find the minimum size chunk large enough to fit the requested byte size
    while (curr->next != NULL){
        prev = curr;
        curr = curr->next;
        if ((curr->size > size_req && curr->size < best_fit->size) && (curr->alloc == 0)){
            best_fit = curr;
        }
    }
    /*if size of chunk found in LL is equal to size of requested memory
    return pointer to the whole chunk*/
    if (best_fit != NULL && best_fit->size == size_req){
        best_fit->alloc = 1;
        result = (void*)(++best_fit);
        // printf("Exact fitting block allocated\n");
        return result;
    }
    /*else if size of chunk found in LL is larger then requested memory size 
    call split method and return pointer to allocated memory chunk*/
    else if(best_fit != NULL && best_fit->size > (size_req + sizeof(header_t))){
        split(best_fit, size_req);
        result = (void*)(++best_fit);
        // printf("Fitting block allocated with split\n");
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
    header_t *p_header = (header_t *)p - 1;
    header_t *next_header = p_header->next;

    if (next_header != NULL && !next_header->alloc) {
        p_header->next = next_header->next;
        p_header->size += next_header->size + sizeof(header_t);
        num_coal++;
        chunk_count -= 1;
        // printf("%d\n", chunk_count);
    }

    // try to coalesce the previous and current chunks
    // start at beginning of memory, traverse linked list until we reach the previous chunk
    header_t *h = (header_t*)memory;
    while (h < p_header) {
        if (h->next == p_header && !h->alloc) {
            h->next = p_header->next;
            h->size += p_header->size + sizeof(header_t);
            num_coal++;
            chunk_count -= 1;
            // printf("%d\n", chunk_count);
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
                // printf("Free successful!\n");
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