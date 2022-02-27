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
            size_req = (rand() * (MEMSIZE / TASK_SIZE - sizeof(header_t))) / RAND_MAX + 1;
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

// defragmentation test:
// 1. calculate a request size between 2 and floor(MEMSIZE / 8) - sizeof(header_t)
// 2. malloc as many requests as possible
// 3. free the even-numbered chunks
// 4. free the odd-numbered chunks
// 5. request one big chunk that take up all of memory, ensure the request is successful
int task5() {
    int i;
    int req_size = (rand() * (MEMSIZE / 8 - sizeof(header_t) - 2)) / RAND_MAX + 2;
    int num_chunks = MEMSIZE / (req_size + sizeof(header_t));
    char *p[MEMSIZE / sizeof(header_t)];
    // request memory
    for (i = 0; i < num_chunks; i++) {
        p[i] = malloc(req_size);
    }
    // free even-numbered chunks
    for (i = 0; i < num_chunks; i += 2) {
        free(p[i]);
    }    
    // free odd-numbered chunks
    for (i = 1; i < num_chunks; i += 2) {
        free(p[i]);
    }
    // request one big chunk
    req_size = MEMSIZE - sizeof(header_t);
    p[0] = malloc(req_size);
    // clean up
    free(p[0]);
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

void testing_menu(int *p){
    int choice1, choice2;
    srand(RAND_SEED);
    printf("TESTING MENU\n");
    printf("------------\n");
    printf("1) MEMGRIND TEST OPTIONS\n");
    printf("2) BASIC TEST OPTIONS\n");
    printf("3) EXIT\n");
    printf("\nEnter number corresponding to menu choice:");
    scanf("%d",&choice1);
    printf("\n");

    switch(choice1){
        case 1:
            printf("MEMGRIND TEST OPTIONS:\n");
            printf("----------------------\n");
            printf("1) Grind Task 1\n");
            printf("2) Grind Task 2\n");
            printf("3) Grind Task 3a\n");
            printf("4) Grind Task 3b\n");
            printf("5) Grind Task 4\n");
            printf("6) Grind Task 5\n");
            printf("7) Run All Tasks Sequentially\n");
            printf("\nEnter number corresponding to menu choice:");
            scanf("%d",&choice2);
            printf("\n");
            switch (choice2) {
                case 1:
                    grind_task("Task 1", &task1);
                    printf("\n");
                    break;
                case 2:
                    grind_task("Task 2", &task2);
                    printf("\n");
                    break;
                case 3:
                    grind_task("Task 3a", &task3a);
                    printf("\n");
                    break;
                case 4:
                    grind_task("Task 3b", &task3b);
                    printf("\n");
                    break;
                case 5:
                    grind_task("Task 4", &task4);
                    printf("\n");
                    break;
                case 6:
                    grind_task("Task 5", &task5);
                    printf("\n");
                    break;
                case 7:
                    printf("Memgrind Task 1:\n");
                    printf("----------------\n");
                    grind_task("Task 1", &task1);
                    printf("\n");
                    printf("Memgrind Task 2:\n");
                    printf("----------------\n");
                    grind_task("Task 2", &task2);
                    printf("\n");
                    printf("Memgrind Task 3a:\n");
                    printf("-----------------\n");
                    grind_task("Task 3a", &task3a);
                    printf("\n");
                    printf("Memgrind Task 3b:\n");
                    printf("-----------------\n");
                    grind_task("Task 3b", &task3b);
                    printf("\n");
                    printf("Memgrind Task 4:\n");
                    printf("----------------\n");
                    grind_task("Task 4", &task4);
                    printf("\n");
                    printf("Memgrind Task 5:\n");
                    printf("----------------\n");
                    grind_task("Task 5", &task5);
                    printf("\n");
                    break;
            
                default:
                    printf("ERROR: Invalid Input.\n");
                    printf("\n");
                    break;
            }
            break;
        
        case 2:
            printf("BASIC TEST OPTIONS:\n");
            printf("-------------------\n");
            printf("1) Test 1: set_diff_value_types()\n");
            printf("2) Test 2: normal_ops() \n");
            printf("3) Test 3: break_things()\n");
            printf("4) Test 4: test_range_case()\n");
            printf("5) Run All Tests Sequentially\n");
            printf("\nEnter number corresponding to menu choice:");
            scanf("%d",&choice2);
            printf("\n");
            switch (choice2) {
                case 1:
                    set_diff_value_types();
                    printf("\n");
                    break;
                case 2:
                    normal_ops();
                    printf("\n");
                    break;
                case 3:
                    break_things();
                    printf("\n");
                    break;
                case 4:
                    test_range_case();
                    printf("\n");
                    break;
                case 5:
                    printf("-------------\n");
                    printf("Basic Test 1:\n");
                    printf("-------------\n");
                    set_diff_value_types();
                    printf("\n");
                    printf("-------------\n");
                    printf("Basic Test 2:\n");
                    printf("-------------\n");
                    normal_ops();
                    printf("\n");
                    printf("-------------\n");
                    printf("Basic Test 3:\n");
                    printf("-------------\n");
                    break_things(); 
                    printf("\n"); 
                    printf("-------------\n");
                    printf("Basic Test 4:\n");
                    printf("-------------\n");
                    test_range_case();
                    printf("\n");
                    break;
            
                default:
                    printf("ERROR: Invalid Input.\n");
                    printf("\n");
                    break;
            }
            break;
        case 3:
            *p = 1;
            break;
        
        default:
            printf("ERROR: Invalid Input.\n");
            printf("\n");
            break;

    }
    
}

int main(int argc, char**argv)
{   
    int flag = 0;
    
    while (flag == 0){
        testing_menu(&flag);
    }
   
    return EXIT_SUCCESS;
}

