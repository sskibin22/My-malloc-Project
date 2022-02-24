#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int basic_test1() {
    // DELETE: these are our earliest tests for functionality
    int req1 = 4096 - sizeof(header_t), req2 = 500, req3 = 100, req4 = 200;

    printf("Attempting to allocate %d bytes\n", req1);
    int *p = malloc(req1);
    print_LL_table();

    printf("Attempting to allocate %d bytes\n", req2);
    int *q = malloc(req2);

    free(p);
    print_LL_table();

    free(q);

    printf("Attempting to allocate %d bytes\n", req3);
    int *r = malloc(req3);
    print_LL_table();

    printf("Attempting to allocate %d bytes\n", req4);
    int *s = malloc(req4);
    print_LL_table();

    printf("Attempting to free %d-byte chunk\n", req4);
    free(s);
    print_LL_table();

    printf("Attempting to free %d-byte chunk\n", req3);
    free(r);
    print_LL_table();

    return EXIT_SUCCESS;
}

int basic_test2() {
    // make sure code works when it is supposed to    
    int p_size_req = 10 * sizeof(int);
    int q_size_req = 5 * sizeof(int);
    
    int *p = malloc(p_size_req);
    int *q = malloc(q_size_req);
    
    printf("\nTest 1: make sure malloc is returning a pointer to the payload, "
        "not the header\n");
    printf("request %d bytes, return pointer p\n", p_size_req);
    printf("request %d bytes, return pointer q\n", q_size_req);
    int p_alloc = ((header_t *)p - 1)->alloc;
    int p_size_size = ((header_t *)p - 1)->size;
    int q_alloc = ((header_t *)q - 1)->alloc;
    int q_size_size = ((header_t *)q - 1)->size;
    printf("header imputed from p reports allocated == %d\n", p_alloc);
    printf("header imputed from p reports size == %d\n", p_size_size);
    printf("header imputed from q reports allocated == %d\n", q_alloc);
    printf("header imputed from q reports allocated == %d\n", q_size_size);
    if (p_size_req == p_size_size && p_alloc == 1 
        && q_size_req == q_size_size && q_alloc == 1) {
        printf("PASS\n");
    }
    else {
        printf("FAIL\n");
    }    
    
    printf("\nTest 2: make sure malloc(n) actually allocates n bytes\n");
    int p_size_next = (q - p) * sizeof(int) - sizeof(header_t);
    printf("p points to %d bytes: computed from pointer to next header\n", p_size_next);
    printf("p points to %d bytes: stored as size in header\n", p_size_size);
    if (p_size_req == p_size_next && p_size_req == p_size_size) {
        printf("PASS\n");
    }
    else {
        printf("FAIL\n");
    }

    free(p);
    free(q);

    return EXIT_SUCCESS;
}

int basic_test3() {
    // see what happens when we try doing bad stuff
    
    
    

    // request larger chunk than available, part 1
    int req3 = 4094;
    printf("\nTest 3: request too large a chunk of memory, part 1\n");
    printf("request allocation of %d bytes\n", req3);
    int *p = malloc(req3);
    //print_LL_table();

    // request larger chunk than available, part 2
    printf("\nTest 4: request too large a chunk of memory, part 2\n");
    int req4big = 3700, req4small = 400;
    printf("request %d bytes, return pointer p\n", req4big);
    p = malloc(req4big);
    printf("request %d bytes, return pointer q\n", req4small);
    int *q = malloc(req4small);
    free(p);
    
    // request chunk of 0 bytes
    printf("\nTest 5: request allocation of 0 bytes\n");
    p = malloc(0);

    // test memory safety
    // allocate 4 chunks: chunk 0 takes 91% of array capacity, chunks 1 through 3
    // take 3% each. Free chunks 1 and 3. Then try to allocate 5% of array capacity.
    // There is a total of 6% of memory free, nevertheless malloc() should return a
    // null pointer and an informative message.
    printf("\nTest 6: memory safety\n");
    printf("request four allocations, free two of them\n\n");
    int req6big = 3700, req6small = 110, req6fail = 192;
    p = malloc(req6big);
    q = malloc(req6small);
    int *r  = malloc(req6small);
    int *s  = malloc(req6small);
    free(q);
    free(s);
    print_LL_table();
    printf("%d bytes of memory free\n", mem_diagnostics(memory_free));
    printf("request %d bytes of memory\n", req6fail);
    int *t = malloc(req6fail);
    printf("null pointer returned: %d\n\n", t == NULL);

    // test free() usage errors
    printf("\nTest 7: free() usage errors\n");
    printf("\nfree a local variable:\n");
    free(&req6big);
    printf("\nfree a ptr offset by +1 byte from a ptr returned by malloc:\n");
    free(p + 1);
    printf("\nfree a ptr that's already been freed:\n");
    free(p);
    free(p);

    return EXIT_SUCCESS;
}

// replicate error discovered in memgrind test 4
int basic_test4()
{
    char *p[8];
    p[0] = malloc(20);
    print_LL_table();
    p[1] = malloc(12);
    print_LL_table();
    p[2] = malloc(17);
    print_LL_table();
    free(p[2]);
    print_LL_table();
    p[3] = malloc(20);
    print_LL_table();
    p[4] = malloc(22);
    print_LL_table();
    p[5] = malloc(9);
    print_LL_table();
    p[6] = malloc(11);
    print_LL_table();
    free(p[5]);
    print_LL_table();
    free(p[3]);
    print_LL_table();
    p[7] = malloc(10);
    print_LL_table();

    return EXIT_SUCCESS;
}

int basic_test5(){
    int *ptr; 
    char *ptr2; 
    float *ptr3;
    int n, i;
 
    //get the number of elements for the array from user
    printf("Enter number of elements:");
    scanf("%d",&n);
    printf("Entered number of elements: %d\n", n);
 
    //Dynamically allocate memory to each pointer using malloc()
    ptr = (int*)malloc(n * sizeof(int));
    ptr2 = (char*)malloc(n * sizeof(char));
    ptr3 = (float*)malloc(n * sizeof(float));
    
    //set elements of array as integers
    for (i = 0; i < n; ++i) {
        ptr[i] = i + 1;
    }
    //set elements of array as characters
    for (i = 0; i < n; ++i) {
        ptr2[i] = 'A';
    }
    //set elements of array as floats
    for (i = 0; i < n; ++i) {
        ptr3[i] = i + 0.5;
    }

    //print the elements of each array
    printf("The elements of the integer array are: ");
    for (i = 0; i < n; ++i) {
        printf("%d, ", ptr[i]);
    }
    printf("\n");
    printf("The elements of the character array are: ");
    for (i = 0; i < n; ++i) {
        printf("%c, ", ptr2[i]);
    }
    printf("\n");
    printf("The elements of the float array are: ");
    for (i = 0; i < n; ++i) {
        printf("%.2f, ", ptr3[i]);
    }
    printf("\n");

    //deallocate memory from heap
    free(ptr);
    free(ptr2);
    free(ptr3);
    
    return EXIT_SUCCESS;
}
int basic_test6(){
    int n = 118, *q, *r, *s, *t;
    char *p[n];
    for (int i = 0; i < n; i++){
        p[i] = malloc(22);
    }
    q = malloc(38);
    r = malloc(10);
    print_LL_table();
    free(p[50]);
    free(p[80]);
    //free(p[81]);
    print_LL_table();
    s = malloc(12);
    print_LL_table();
    t = malloc(10);
    print_LL_table();
    free(q);
    print_LL_table();
    free(s);
    print_LL_table();

    return EXIT_SUCCESS;
}
