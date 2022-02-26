CS 214 Systems Programming
Prof. David Menendez
Project 1: My Little malloc()
Due: 2/28/2022
by Scott Skibin, Scott Burke

-------------------------------
DESCRIPTIONS OF INCLUDED FILES:
-------------------------------

1) mymalloc.c
    -> headers: stdio.h, stdlib.h, mymalloc.h
    -> int print_LL_table()
    -> int mem_diagnostics(enum diagnostic diag)
    -> void initialize()
    -> void split(header_t * alloc_split, size_t size_req)
    -> void *mymalloc(size_t size_req, char * file, int line)
    -> int coalesce(void *p)
    -> void myfree(void *p, char *file, int line)

2) mymalloc.h
    -> Defines two macros(malloc(s) and free(p)) to replace explicitly calling mymalloc() and myfree() when a client wants to allocate or deallocate memory.
    -> Defines a type header_t as a struct.  This will hold the metadata for each chunk within memory[]
    -> declares an enum for mem_diagnostics()
    -> Contains function prototypes from mymalloc.c

3) memgrind.c
    -> headers: stdio.h, stdlib.h, sys/time.h, mymalloc.h, basic_tests.h
    -> Defines 3 macros(TASK_REPEAT, TASK_SIZE, RAND_SEED)
        -> TASK_REPEAT set to int value of 50 for number of times each task is collected for data collection purposes
        -> TASK_SIZE set to int value of 120 for repeating processes within tasks
        -> RAND_SEED set to integer value of 8675309 for randomization within tasks
    -> int task1()
    -> int task2()
    -> int task3a()
    -> int task3b()
    -> int task4()
    -> int task5()
    -> int grind_task(char* task_name, int (*task)())
    -> int main(int argc, char**argv)

4) basic_tests.c
    -> headers: stdio.h, stdlib.h, mymalloc.h
    -> int set_diff_value_types()
        ->Gets n = user input as an integer. Calls malloc(n * sizeof(type)) with 3 different pointer types (int, char, float) using type casting.  Populate each allocated memory array with it's respective type value, then prints the contents of the arrays. free() all memory to ensure allocation and deallocation of memory works as it should.
    -> int normal_ops()
    -> int break_things()
    -> int test_range_case()

5) basic_tests.h
    -> Contains function prototypes from basic_tests.c

6) utils.c
    -> headers: stdio.h, stdlib.h, mymalloc.h
    -> void print_err(char *file, int line, char *msg)

7) MakeFile
    -> Compiles and links all .c files within project folder.
    -> Creates executable file called "test" to run all tests/tasks within main() of memgrind.c

----------
TEST PLAN:
----------

The properties of our library can be divided into two broad categories: those that the library exhibits when it is used as intended ("normal operations") and those it exhibits when the user attempts actions outside of the design intentions ("attempted breaks"). This test plan states each property, followed by a description of how we verify that each property holds. Note that all test programs described below are called from the main() function of memgrind.c.

1) Normal operations

    a) The library correctly initializes the memory array: it sets up the memory management data structure if and only if malloc() is called on a memory array that has just been initialized

        -> Make sure data structure is correctly set up on a newly initialized memory array; make sure the code does not attempt to set up a new data structure on an array that has just one header (have to think this through some more)

    b) malloc(k) correctly allocates k bytes of memory from the (simulated) heap
        
        -> When the user requests allocations of various sizes and writes data to the allocated memory locations, the user should be able to read back the correct data from those locations. The set_diff_value_types() function in basic_tests.c performs this test. The user is prompted for a number of elements n. malloc() is then called to request three allocations: one of size n ints, one of size n chars, and one of size n floats. The chunks referenced by the pointers returned by malloc() are populated with increasing sequences of the respective datatypes (1, 2, 3, ...; A, B, C, ...; 0.5, 1.5, 2.5, ...). The contents of each chunk are then printed to the screen. A successful result is a printout of the entirety of each of the entered sequences. An unsuccessful result is any other output.

        -> A secondary test is to ensure that when a user requests an allocation of k bytes, the number of bytes between the pointer to that memory chunk and a pointer to the next chunk in memory is at least k plus the size of the header. Test 2 of the normal_ops() function in basic_tests.c performs a specific instance of this test. It takes pointers p and q returned from two malloc() requests that were made in the previous test (Test 1). These requests were made from a fully free heap, with p requested first, so a successful test will have the number of bytes between p and q equal to the size of the request for p plus the size of the header. Test 2 prints to screen a message that concludes with PASS if this condition is met, FAIL otherwise.

    c) malloc() returns a pointer to the start of the data portion of each chunk, not to the start of the metadata portion, nor to any other address in the memory array, nor to an address outside the memory array. 

        -> Given a pointer p returned from malloc(), the bytes immediately preceding p should compose a valid header. Test 1 of the normal_ops() function in basic_tests.c checks this by using malloc() to request two pointers (p and q) to chunks of memory. We cast p and q to be pointers to headers (more precisely, pointers to header_t). Applying pointer artihmetic, we assume that (p - 1) and (q - 1) are headers and reference their size and alloc members, which respectively store the size requested (in bytes) and a 0/1 indicator of whether the memory chunk is allocated. A successful test finds the correct values in these locations. Test 1 prints to screen a message that concludes with PASS if this condition is met, FAIL otherwise.

    d) malloc() should return first available exact fitting chunk (if one exists) and not split the fitted chunk, but rather return a pointer to the exact fitting chunk and set its allocated flag to true(1)

        -> call malloc(n) 2 times, free a pointer from the first call to malloc(n), call a new malloc(n). Use print_LL_table() to view nodes in memory and ensure the new call to malloc(n) was fitted into the first chunk with a payload of size n.

    e) malloc() should handle situations when a client requests a number of bytes within memory, and memory[] contains free chunks of size within the range byte size requested < free chunk available <= (bite size requested + byte size of a header(24 bytes on linux)). In this case malloc() should continue to search for a free chunk large enough to call split() or if no larger chunk is available malloc() should fit the requested amount of bytes into the first available free chunk without calling split to avoid initializing headers with 0 bytes of payload or overwriting memory with a header.

        -> Case 1: Fill heap with chunks of equal size n. free() first and last chunk. Attempt to allocate malloc(n).

        -> Case 2: Fill heap with chunks of equal size n. free() first chunk and last two chunks so they coalesce into 1 chunk of size n*2. Attempt to allocate malloc(n).


2) Attempted breaks

    a) malloc() should not allocate more memory than is available

        -> call malloc(n), where n is larger than the available memory, and return a null pointer along with an informative error message

    b) malloc() should not allocate memory for requests of size 0 or less

        -> call malloc(n) where n is zero or negative, return a null pointer along with an informative error message (maybe only need the error message for negative numbers)
    
    c) malloc() should exhibit memory safety, meaning that it does not allocate a chunk that extends beyond the boundaries of our memory array

        ->Test: allocate four chunks with chunk 0 taking 91% of array capacity and chunks 1 through 3 taking up 3% each. Free chunks 1 and 3. Then try to allocate 5% of array capacity. There is a total of 6% of memory free, nevertheless malloc() should not return a pointer to chunk 3. It should return a null pointer and an informative error message

    d) free() should detect and report usage errors, including 1) trying to free() addresses not obtained with malloc(); 2) trying to free() addresses within memory[] but not at the start of a payload; 3) calling free() more than once on the same pointer

        -> Try doing each of these, ensure a correct, informative error message is sent to the terminal

    e) The library should minimize memory fragmentation

        -> see task5() in memgrind.c
