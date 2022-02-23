#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"
#include "basic_tests.h"

#define TASK_REPEAT 50
#define TASK_SIZE 120
#define RAND_SEED 8675309

// malloc() and immediately free() a 1-byte chunk, TASK_SIZE times
int task1() {
    int i;
    char *p;
    for (i = 0; i < TASK_SIZE; i++) {
        p = malloc(sizeof(char));
        free(p);
    }
    return EXIT_SUCCESS;
}

// use malloc() to get TASK_SIZE 1-byte chunks, storing the pointers
// in an array, then use free() to deallocate the chunks.
int task2() {
    int i;
    char *p[TASK_SIZE];
    for (i = 0; i < TASK_SIZE; i++) {
        p[i] = malloc(sizeof(char));
    }
    for (i = 0; i < TASK_SIZE; i++) {
        free(p[i]);
    }
    return EXIT_SUCCESS;
}

// Randomly choose between allocating and deallocating 1-byte chunks,
// repeat until malloc() has been called TASK_SIZE times, then free all remaining allocated chunks
// variant 3a frees chunks in the order they were allocated (FIFO)
int task3a() {
    int malloc_ind = 0, free_ind = 0;
    char *p[TASK_SIZE];
    while (malloc_ind < TASK_SIZE) {
        if (rand() < RAND_MAX / 2) {
            p[malloc_ind++] = malloc(sizeof(char));
        }
        else {
            if (free_ind < malloc_ind) {
                free(p[free_ind++]);
            }
        }
    }
    while(free_ind < TASK_SIZE) {
        free(p[free_ind++]);
    }
    return EXIT_SUCCESS;
}

// Randomly choose between allocating and deallocating 1-byte chunks,
// repeat until malloc() has been called TASK_SIZE times, then free all remaining allocated chunks
// variant 3b frees previously allocated chunks in random order
int task3b() {
    int malloc_ind = 0, alloc_ct = 0;
    int rand_free, i;
    char *p[TASK_SIZE];
    char allocated[TASK_SIZE]; //keep track of allocated indexes in p

    for (i = 0; i < TASK_SIZE; i++) {
        allocated[i] = 0;
    }

    while (malloc_ind < TASK_SIZE) {
        // may randomly choose to call malloc()
        if (rand() < RAND_MAX / 2) {
            p[malloc_ind] = malloc(sizeof(char));
            allocated[malloc_ind] = 1;
            malloc_ind++;
            alloc_ct++;
        }
        // otherwise, if at least one chunk is allocated, then randomly
        // choose an allocated chunk and free it
        else {
            if (alloc_ct > 0) {
                rand_free = (rand() * malloc_ind) / RAND_MAX;
                // search should wrap around to beginning of p
                // once i reaches the last malloc'd index
                i = (rand_free + 1) % malloc_ind;
                // sequentially search for an allocated chunk and free it
                while (i != rand_free) {
                    if (allocated[i]) {
                        free(p[i]);
                        allocated[i] = 0;
                        alloc_ct--;
                        break;
                    }
                    else {
                        i = (i + 1) % malloc_ind;
                    }
                }
            }
        }
    }
    // free any remaining allocated chunks
    for (i=0; i < TASK_SIZE; i++) {
        if (allocated[i]) {
            free(p[i]);
        }
    }
    return EXIT_SUCCESS;
}

// Randomly choose between allocating and deallocating randomly-sized chunks,
// repeat until malloc() has been called TASK_SIZE times, then free all remaining allocated chunks.
// Constrain chunk size to MEMSIZE/TASK_SIZE - sizeof(header_t) to ensure we don't request too much memory
int task4() {
    int malloc_ind = 0, alloc_ct = 0;
    int rand_free, i;
    size_t size_req;
    char *p[TASK_SIZE];
    char allocated[TASK_SIZE]; //keep track of allocated indexes in p

    for (i = 0; i < TASK_SIZE; i++) {
        allocated[i] = 0;
    }

    while (malloc_ind < TASK_SIZE) {
        // may randomly choose to call malloc()
        if (rand() < RAND_MAX / 2) {
            // add 1 to ensure no requests for zero bytes are made
            size_req = (rand() * (4096 /TASK_SIZE - sizeof(header_t))) / RAND_MAX + 1;
            //printf("Requesting %d bytes to store in index %d\n", size_req, malloc_ind);
            p[malloc_ind] = malloc(size_req);
            allocated[malloc_ind] = 1;
            malloc_ind++;
            alloc_ct++;
        }
        // otherwise, if at least one chunk is allocated, then randomly
        // choose an allocated chunk and free it
        else {
            if (alloc_ct > 0) {
                rand_free = (rand() * malloc_ind) / RAND_MAX;
                // search should wrap around to beginning of p
                // once i reaches the last malloc'd index
                i = (rand_free + 1) % malloc_ind;
                // sequentially search for an allocated chunk and free it
                while (i != rand_free) {
                    if (allocated[i]) {
                        //printf("Attempting to free index %d, which has alloc = %d\n", i, allocated[i]);
                        free(p[i]);
                        allocated[i] = 0;
                        alloc_ct--;
                        break;
                    }
                    else {
                        i = (i + 1) % malloc_ind;
                    }
                }
            }
        }
    }
    // free any remaining allocated chunks
    for (i=0; i < TASK_SIZE; i++) {
        if (allocated[i]) {
            free(p[i]);
        }
    }
    return EXIT_SUCCESS;
}

int grind_task(char* task_name, int (*task)()) {
    int i;
    struct timeval start, end;
    long elapsed_time = 0;

    for (i=0; i < TASK_REPEAT; i++) {
        gettimeofday(&start, NULL);
        (*task)();
        gettimeofday(&end, NULL);
        elapsed_time += (end.tv_sec - start.tv_sec) * 1000000 +
            end.tv_usec - start.tv_usec;
    }
    printf("%s required an average of %ld usec per run\n", task_name, elapsed_time / TASK_REPEAT);
    return EXIT_SUCCESS;
}

int main(int argc, char**argv)
{   
    /*
    int req1 = 4096 - sizeof(header_t), req2 = 500, req3 = 100, req4 = 200;

    printf("Attempting to allocate %d bytes\n", req1);
    int *p = malloc(req1);

    printf("Attempting to allocate %d bytes\n", req2);
    int *q = malloc(req2);

    free(p);
    free(q);

    printf("Attempting to allocate %d bytes\n", req3);
    int *r = malloc(req3);

    printf("Attempting to allocate %d bytes\n", req4);
    int *s = malloc(req4);

    free(r);
    free(s);
    */

    //required tests:
    srand(RAND_SEED);
    //grind_task("Task 1", &task1);
    //grind_task("Task 2", &task2);
    //grind_task("Task 3a", &task3a);
    //grind_task("Task 3b", &task3b);
    //grind_task("Task 4", &task4);

    //basic tests:

    //basic_test1();
    //basic_test2();
    //basic_test3();
    basic_test4();

    return EXIT_SUCCESS;
}

