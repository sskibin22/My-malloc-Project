CS 214 Systems Programming
Prof. David Menendez
Project 1: My Little malloc()
Due: 2/28/2022
by Scott Skibin, Scott Burke

-------------------------------
DESCRIPTIONS OF INCLUDED FILES:
-------------------------------
1) mymalloc.c
    ->headers: stdio.h, stdlib.h, mymalloc.h
    ->int print_LL_table()
    ->int mem_diagnostics(enum diagnostic diag)
    ->void initialize()
    ->void split(header_t * alloc_split, size_t size_req)
    ->void *mymalloc(size_t size_req, char * file, int line)
    ->int coalesce(void *p)
    ->void myfree(void *p, char *file, int line)

2) mymalloc.h
    ->Defines two macros(malloc(s) and free(p)) to replace explicitly calling mymalloc() and myfree() when a client wants to allocate or deallocate memory.
    ->Defines a type header_t as a struct.  This will hold the metadata for each chunk within memory[]
    ->declares an enum for mem_diagnostics()
    ->Contains function prototypes from mymalloc.c

3) memgrind.c
    ->headers: stdio.h, stdlib.h, sys/time.h, mymalloc.h, basic_tests.h
    ->Defines 3 macros(TASK_REPEAT, TASK_SIZE, RAND_SEED)
        ->TASK_REPEAT set to int value of 50 for number of times each task is collected for data collection purposes
        ->TASK_SIZE set to int value of 120 for repeating processes within tasks
        ->RAND_SEED set to integer value of 8675309 for randomization within tasks
    ->int task1()
    ->int task2()
    ->int task3a()
    ->int task3b()
    ->int task4()
    ->int task5()
    ->int grind_task(char* task_name, int (*task)())
    ->int main(int argc, char**argv)

4) basic_tests.c
    ->headers: stdio.h, stdlib.h, mymalloc.h
    ->int normal_ops()
    ->int break_things()
    ->int set_diff_value_types()

5) basic_tests.h
    ->Contains function prototypes from basic_tests.c

6) utils.c
    ->headers: stdio.h, stdlib.h, mymalloc.h
    ->void print_err(char *file, int line, char *msg)

6) MakeFile
    ->Compiles and links all .c files within project folder.
    ->Creates executable file called "test" to run all tests/tasks within main() of memgrind.c

----------
TEST PLAN:
----------
a) malloc(n) correctly allocates n bytes of memory
    ->Test: make sure the address of the metadata of the next chunk (or the end of the memory array) is at least n bytes higher than the pointer returned from malloc(n)

b) (closely related to the previous property) malloc() returns a pointer to the start of the data portion of each chunk, not to the start of the metadata portion, or to any other address in the memory array (or outside the memory array)
    ->Test: can we test that a pointer returned from malloc() is n bytes larger than the start of some chunk in the memory array, where n is the size of any metadata portion?

c) malloc() does not allocate more memory than is available
    ->Test: call malloc(n), where n is larger than the available memory, and return a null pointer along with an informative error message

d) malloc() sensibly handles requests for a chunk of size 0 or less
    ->Test: call malloc(n) where n is zero or negative, return a null pointer along with an informative error message (maybe only need the error message for negative numbers)

e) malloc() returns first available exact fitting chunk (if one exists) and does not split the fitted chunk but rather returns a pointer to the exact fitting chunk and sets it's allocated flag to true(1)
    ->Test: call malloc(n) 2 times, free a pointer from the first call to malloc(n), call a new malloc(n). Use print_LL_table() to view nodes in memory and ensure the new call to malloc(n) was fitted into the first chunk with a payload of size n.

f) malloc() handles situations when a client requests a number of bytes within memory, and memory[] contains free chunks of size within the range bite size requested < free chunk available <= (bite size requested + byte size of a header(24 bytes on linux)).  In this case malloc() should continue to search for a free chunk large enough to call split() or if no larger chunk is available malloc() should fit the requested amount of bytes into the first available free chunk without calling split to avoid initializing headers with 0 bytes of payload or overwriting memory with a header.
    ->Test: 
        Case 1:
        ->Fill heap with chunks of equal size n. free() first and last chunk. Attempt to allocate malloc(n).

        Case 2: 
        ->Fill heap with chunks of equal size n. free() first chunk and last two chunks so they coalesce into 1 chunk of size n*2. Attempt to allocate malloc(n).

g) Memory Safety: malloc() cannot allocate a chunk that extends beyond the boundaries of our memory array
    ->Test: allocate four chunks
        ->chunk 0 takes 91% of array capacity 
        ->chunks 1 through 3 take up 3% each. 
        ->Free chunks 1 and 3. 
        Then try to allocate 5% of array capacity. There is a total of 6% of memory free, nevertheless malloc() should not return a pointer to chunk 3. It should return a null pointer and perhaps an informative error message

h) Detection of usage errors with free(): including... 
    1)trying to free() addresses not obtained with malloc() 
    2)trying to free() addresses not at the start of a chunk 
    3)calling free() more than once on the same pointer
    ->Test: Try doing each of these, ensure a correct, informative error message is sent to the terminal

i) Correct initialization of memory array: need to set up initial memory management data structure(s) if and only if malloc() (and free()?) are called on a memory array that has just been initialized
    ->Test: make sure data structure is correctly set up on a newly initialized memory array; make sure the code does not attempt to set up a new data structure on an array that has just one header (have to think this through some more)

j) Management of memory fragmentation: we need to define a fragmentation management policy and make sure it’s being followed
    ->Test: see task5() in memgrind.c

