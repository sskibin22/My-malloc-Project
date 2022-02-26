#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

//declare contiguous array that acts as the 'heap'. Set it's size equal to MEMSIZE
static char memory[MEMSIZE];

//Points to the main block of memory which is initially free
header_t *freeLL = (void*)memory;

// print errors from mymalloc() and myfree() calls
// caller must include \n in msg, if desired
void print_err(char *file, int line, char *msg) {
    printf("ERROR in %s, line %d: %s", file, line, msg);
}

// print table summarizing contents of linked list
// Shows node #, size of payload, allocated indicator,
// and # of bytes to header of next node (if any)
int print_LL_table() {
    header_t *p = freeLL;
    int node = 0;
    if(!freeLL->size) {
        printf("Memory not allocated\n\n");
        return EXIT_FAILURE;
    }
    printf("Header\tSize\tAlloc\tNext\n");
    printf("****************************\n");
    while (p != NULL) {        
        printf("%d\t%lu\t%d\t", node, p->size, p->alloc);
        if (p->next != NULL) {
            printf("%ld\n", (char *)p->next - (char *)p);
        }
        else {
            printf("NULL\n");
        }
        p = p->next;
        node++;
    }
    printf("\n");
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
            case largest_free:
                if (!p->alloc && p->size > accum) accum = p->size;
                break;
            default:
                printf("Invalid memory diagnostic selected\n");
                return -1;
        }
        p = p->next;
    }
    return accum;
}

//splits a free block that is larger then the size requested
void split(header_t *alloc_split, size_t size_req){
    header_t *free_split = ((void*)alloc_split + size_req + sizeof(header_t));  //+ sizeof(header_t) //points free_split pointer to end of alloc_split header + requested byte size + size of header(12 bytes)
    free_split->size = (alloc_split->size) - size_req - sizeof(header_t);   //set size of free_split to allocated size - size requested - size of header(12 bytes).
    free_split->alloc = 0;
    free_split->next = alloc_split->next;
    alloc_split->size = size_req;
    alloc_split->alloc = 1;
    alloc_split->next = free_split;
}

void *mymalloc(size_t size_req, char *file, int line){
    header_t *curr, *prev, *best_fit, *same_fit;
    void *mem_ptr; 
    //if byte size requested is less then 1 report error return NULL
    if (size_req < 1) {
        print_err(file, line, "malloc cannot allocate less then 1 byte of memory\n");
        return NULL;
    }
    //if mymalloc() is not initilized, initilize it
    if(!(freeLL->size)){
        freeLL->size = MEMSIZE - sizeof(header_t);
        freeLL->alloc = 0;
        freeLL->next = NULL;
    }
    
    //point curr to first header in the heap then point best_fit pointer to curr
    curr = freeLL;
    best_fit = curr;

    //while there are more then one chunks in the heap conduct firt-fit alg to traverse heap
    while ((curr->size < size_req || curr->alloc == 1) && (curr->next != NULL)){
        prev=curr;
        curr=curr->next;
    }
    //set best-fit and same_fit pointer equal to the first chunk that is large enough to fit the requested byte size if curr chunk is free
    if (curr->alloc == 0){
        best_fit = curr;
        same_fit = curr;
    }
    else best_fit = NULL;
    /*while best_fit is not NULL and best_fit->size is larger than size_req but also smaller than (size_req + size of a header)
    continue to repeat first-fit alg until the end of the LL is reached.  Do not update same_fit.*/
    while(best_fit != NULL && (best_fit->size > size_req && best_fit->size <= size_req + sizeof(header_t))){
            if(best_fit->next == NULL){
                break;
            }  
            prev=curr;
            curr=curr->next;
            while ((curr->size < size_req || curr->alloc == 1) && (curr->next != NULL)){
                prev=curr;
                curr=curr->next;
            }
            //set best-fit pointer equal to the first chunk that is large enough to fit the requested byte size if curr chunk is free
            if (curr->alloc == 0) best_fit = curr;
            else best_fit = NULL;
        }
    //use best-fit alg to find the minimum size chunk large enough to fit the requested byte size
    while (curr->next != NULL){
        prev = curr;
        curr = curr->next;
   
        if ((curr->size > size_req && curr->size < best_fit->size) && (curr->alloc == 0)){
            best_fit = curr;
        }
    }
    /*Covers the case when best_fit is set to NULL because the last node in the LL contained an allocated chunk and there 
    is no other free chunk larger than same_fit when same_fit is within these conditions: (size_req < same_fit <= size_req + sizeof(header_t)).
    This if conditional checks to see if, in this case, the same_fit pointer is still pointing to a free chunk of memory that
    is still large enough to fit size_req but falls within the conditions stated above. Therefore, we set best_fit equal to 
    the original same_fit pointer so that the first if conditional can catch it below.*/
    if (best_fit == NULL && (size_req + (2*sizeof(header_t)) >= same_fit->size)){
        best_fit = same_fit;
    }
    
    
    /*if size of chunk found in LL is equal to size of requested memory OR greater than size_req AND less than or equal to size_req + size of a header
    return pointer to the whole chunk*/
    if (best_fit != NULL && (best_fit->size == size_req || ((best_fit->size > size_req && best_fit->size <= size_req + sizeof(header_t)) ))){
        best_fit->alloc = 1;
        mem_ptr = (void*)(++best_fit);
        return mem_ptr;
    }
    /*else if size of chunk found in LL is larger then requested memory size + size of a header
    call split method and return pointer to allocated memory chunk*/
    else if(best_fit != NULL && best_fit->size > (size_req + sizeof(header_t))){
        split(best_fit, size_req);
        mem_ptr = (void*)(++best_fit);
        return mem_ptr;
    }
    //else if conditions are not met return NULL
    else{
        print_err(file, line, "No sufficient memory to allocate\n");
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
        print_err(file, line, "attempt to free when memory is not initialized\n");
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
                print_err(file, line, "attempt to free memory that is already free\n");
                return;
            }
        }
        // cannot free memory that was not allocated by malloc, part 1
        // for now, provide a specific error message if p points to a header
        else if (p == curr) {
            print_err(file, line, "attempt to free a memory chunk that starts at a header\n");
            return;
        }        
        curr = curr->next;
    }
    // cannot free memory that was not allocated by malloc, part 2
    // error message if loop terminates without freeing a chunk on the linked list
    // don't know a foolproof way to distinguish between pointers inside
    // and outside of memory[]
    print_err(file, line, "attempt to free memory not allocated by malloc()\n");
}