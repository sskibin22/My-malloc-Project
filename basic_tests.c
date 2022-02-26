#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int normal_ops() {
    // make sure code works when it is supposed to    
    
    printf("\nTest 1: make sure malloc is returning a pointer to the payload, "
        "not the header\n");
    int p_size_req = 10 * sizeof(int);
    int q_size_req = 5 * sizeof(int);    
    printf("request %d bytes, return pointer p\n", p_size_req);
    int *p = malloc(p_size_req);    
    printf("request %d bytes, return pointer q\n", q_size_req);
    int *q = malloc(q_size_req);
    int p_alloc = ((header_t *)p - 1)->alloc;
    int p_size_size = ((header_t *)p - 1)->size;
    int q_alloc = ((header_t *)q - 1)->alloc;
    int q_size_size = ((header_t *)q - 1)->size;
    printf("header imputed from p reports allocated == %d\n", p_alloc);
    printf("header imputed from p reports size == %d\n", p_size_size);
    printf("header imputed from q reports allocated == %d\n", q_alloc);
    printf("header imputed from q reports size == %d\n", q_size_size);
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

int break_things() {
    
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
    printf("request four allocations, free two of them, then request an allocation\n "
        "that fits within total free memory but would extend beyond the end of memory\n\n");
    int req6big = 3700, req6small = 110, req6fail = 192;
    printf("request %d bytes, return pointer p\n", req6big);
    p = malloc(req6big);
    printf("request %d bytes, return pointer q\n", req6small);
    q = malloc(req6small);
    printf("request %d bytes, return pointer r\n", req6small);
    int *r  = malloc(req6small);
    printf("request %d bytes, return pointer s\n", req6small);
    int *s  = malloc(req6small);
    printf("free q and s\n");
    free(q);
    free(s);
    print_LL_table();
    printf("%d bytes of memory free\n", mem_diagnostics(memory_free));
    printf("request %d bytes\n", req6fail);
    int *t = malloc(req6fail);
    printf("null pointer returned: %d\n\n", t == NULL);
    // clean-up
    free(r);

    // test free() usage errors
    printf("\nTest 7: free() usage errors\n");
    printf("\ntry freeing a local variable:\n");
    free(&req6big);
    printf("\ntry freeing a ptr offset by +1 byte from a ptr returned by malloc:\n");
    free(p + 1);
    printf("\ntry freeing a ptr that's already been freed:\n");
    free(p);
    free(p);

    return EXIT_SUCCESS;
}
//simple test to ensure average program level use of mymalloc() is attainable
int set_diff_value_types(){
    int *ptr1; 
    char *ptr2; 
    float *ptr3;
    int n, i;
 
    //get the number of elements for the array from user
    printf("Enter number of elements:");
    scanf("%d",&n);
    printf("Entered number of elements: %d\n", n);
 
    //Dynamically allocate memory to each pointer using malloc()
    ptr1 = (int*)malloc(n * sizeof(int));
    ptr2 = (char*)malloc(n * sizeof(char));
    ptr3 = (float*)malloc(n * sizeof(float));
    
    //set elements of array as integers
    for (i = 0; i < n; ++i) {
        ptr1[i] = i + 1;
    }
    //set elements of array as characters
    for (i = 0; i < n; ++i) {
        ptr2[i] = 'A' + i;
    }
    //set elements of array as floats
    for (i = 0; i < n; ++i) {
        ptr3[i] = i + 0.5;
    }

    //print the elements of each array
    printf("The elements of the integer array are: ");
    for (i = 0; i < n; ++i) {
        printf("%d, ", ptr1[i]);
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
    free(ptr1);
    free(ptr2);
    free(ptr3);
    
    return EXIT_SUCCESS;
}

int test_range_case(){
    int size = 4,
        br = (MEMSIZE/size) - sizeof(header_t);

    char *p[size];

    //Fill heap
    for (int i = 0; i < size; i++){
        p[i] = malloc(br);
    }
    print_LL_table();
    //Free memory so that there is 1 free chunk of size br at node 0 and 1 free chunk of size br at node 3(last node in LL)
    free(p[0]);
    free(p[3]);
    print_LL_table();
    //attempt to allocate memory of size br
    p[0] = malloc(br);
    print_LL_table();
    free(p[0]);
    print_LL_table();
    //attempt to allocate memory of size br - header byte size with only free chunks of size br available
    p[0] = malloc(br-sizeof(header_t));
    print_LL_table();
    free(p[0]);
    free(p[2]);
    print_LL_table();
    //attempt to allocate memory of size br - header byte size with 1 free chunk of size br and 1 free chunk of size 2*br available
    p[0] = malloc(br-sizeof(header_t));
    print_LL_table();
    
    return EXIT_SUCCESS;
}
