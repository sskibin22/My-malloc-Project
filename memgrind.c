#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

#define TASK_REPEAT 50
#define RAND_SEED 8675309

// malloc() and immediately free() a 1-byte chunk, 120 times
int task1() {
    int i;
    char *p;
    for (i = 0; i < 120; i++) {
        p = malloc(sizeof(char));
        free(p);
    }
    return EXIT_SUCCESS;
}

// use malloc() to get 120 1-byte chunks, storing the pointers
// in an array, then use free() to deallocate the chunks.
int task2() {
    int i;
    char *p[120];
    for (i = 0; i < 120; i++) {
        p[i] = malloc(sizeof(char));
    }
    for (i = 0; i < 120; i++) {
        free(p[i]);
    }
    return EXIT_SUCCESS;
}

// Randomly choose between allocating and deallocating 1-byte chunks,
// repeat until malloc() has been called 120 times, then free all remaining allocated chunks
int task3() {
    int malloc_ind = 0, free_ind = 0;
    char *p[120];
    while (malloc_ind < 120) {
        if (rand() < RAND_MAX / 2) {
            p[malloc_ind++] = malloc(sizeof(char));
        }
        else {
            if (free_ind < malloc_ind) {
                free(p[free_ind++]);
            }
        }
    }
    for( ; free_ind < 120; free_ind++) {
        free(p[free_ind++]);
    }
    return EXIT_SUCCESS;
}

int grind_task(char* task_name, int (*task)()) {
    int i;
    struct timeval current_time;
    long start_time, elapsed_time = 0;

    for (i=0; i < TASK_REPEAT; i++) {
        gettimeofday(&current_time, NULL);
        start_time = current_time.tv_usec;
        (*task)();
        gettimeofday(&current_time, NULL);
        elapsed_time += current_time.tv_usec - start_time;
    }
    printf("%s required an average of %ld usec per run\n", task_name, elapsed_time / TASK_REPEAT);
    return EXIT_SUCCESS;
}

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

    // required tests
    srand(RAND_SEED);
    //grind_task("Task 1", &task1);
    //grind_task("Task 2", &task2);
    //grind_task("Task 3", &task3);

    return EXIT_SUCCESS;
}

