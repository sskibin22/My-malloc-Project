#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int basic_test1() {
    // see if code works when it's supposed to
    int req1 = 4096 - sizeof(header_t), req2 = 500, req3 = 100, req4 = 200;

    printf("Attempting to allocate %d bytes\n", req1);
    int *p = malloc(req1);
    print_LL_contents();

    printf("Attempting to allocate %d bytes\n", req2);
    int *q = malloc(req2);

    free(p);
    print_LL_contents();

    free(q);

    printf("Attempting to allocate %d bytes\n", req3);
    int *r = malloc(req3);
    print_LL_contents();

    printf("Attempting to allocate %d bytes\n", req4);
    int *s = malloc(req4);
    print_LL_contents();

    printf("Attempting to free %d-byte chunk\n", req4);
    free(s);
    print_LL_contents();

    printf("Attempting to free %d-byte chunk\n", req3);
    free(r);
    print_LL_contents();

    return EXIT_SUCCESS;
}

int basic_test2() {
    // 
    int req1 = 10, req2 = 5, req3 = 100, req4 = 200;
    
    int *p = malloc(req1 * sizeof(int));
    int *q = malloc(req2 * sizeof(int));
    
    printf("Test 1: make sure malloc(n) actually allocates n bytes\n");
    printf("Bytes requested: %d\n", req1 * sizeof(int));
    printf("Bytes allocated--computed from pointers: %d\n",
        (q - p) * sizeof(int) - sizeof(header_t));
    printf("Bytes allocated--according to linked list: %d\n\n",
        ((header_t *)p - 1)->size);

    printf("Test 2: make sure malloc is returning a pointer to the payload, "
        "not the header\n");
    printf("Size should be %d, header reports %d\n",
        req2 * sizeof(int), ((header_t *)q - 1)-> size);
    printf("Allocated should be 1, header reports %d\n\n",
        ((header_t *)q - 1)-> alloc);

    return EXIT_SUCCESS;
}

int basic_test3() {
    // see what happens when we try doing bad stuff
    int req_big = 4094, req2 = 4000, req3 = 1000;
    int req4big = 3700, req4small = 110, req4fail = 192;

    // request larger chunk than available, part 1
    printf("Requesting allocation of %d bytes\n", req_big);
    int *p = malloc(req_big);
    //print_LL_contents();

    // request larger chunk than available, part 2
    printf("\nRequesting allocation of %d bytes\n", req2);
    p = malloc(req2);
    printf("Requesting allocation of %d bytes\n", req3);
    int *q = malloc(req3);
    free(p);
    
    // request chunk of 0 bytes
    printf("\nRequesting allocation of 0 bytes\n");
    p = malloc(0);

    // test memory safety
    // allocate 4 chunks: chunk 0 takes 91% of array capacity, chunks 1 through 3
    // take 3% each. Free chunks 1 and 3. Then try to allocate 5% of array capacity.
    // There is a total of 6% of memory free, nevertheless malloc() should return a
    // null pointer and an informative message.
    printf("\nMemory safety test\n");
    print_LL_contents();
    p = malloc(req4big);
    q = malloc(req4small);
    int *r  = malloc(req4small);
    int *s  = malloc(req4small);
    free(q);
    free(s);
    print_LL_contents();
    printf("%d bytes of memory free\n", mem_diagnostics(memory_free));
    printf("request %d bytes of memory\n", req4fail);
    int *t = malloc(req4fail);
    printf("null pointer returned: %d\n\n", t == NULL);

    // test free() usage errors
    printf("\nTry freeing local variable:\n");
    free(&req_big);
    printf("\nTry freeing a ptr offset from a ptr returned by malloc:\n");
    free(p + 1);
    printf("\nTry freeing a ptr that's already been freed:\n");
    free(p);
    free(p);

    return EXIT_SUCCESS;
}

int main(int argc, char**argv)
{   
    basic_test1();
    basic_test2();
    basic_test3();

    return EXIT_SUCCESS;
}