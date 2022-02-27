CS 214 Systems Programming
Prof. David Menendez
Project 1: My Little malloc()
Due: 2/28/2022
by Scott Skibin, Scott Burke

-------------------------------
DESCRIPTIONS OF INCLUDED FILES:
-------------------------------

1) mymalloc.h

    -> Defines two macros(malloc(s) and free(p)) to replace explicitly calling mymalloc() and myfree() when a client wants to allocate or deallocate memory.

    -> Defines a type header_t as a struct.  This will hold the metadata for each chunk within memory[]
        -> On a 64-bit Linux system the byte size of the header is 24 bytes
        -> metadata include:
            1) size of payload
            2) allocated flag (1 = allocated, 0 = free)
            3) pointer to next header in linked list

    -> declares an enum for mem_diagnostics(); see the description of mem_diagnostics() in item 2 below for details

    -> Contains function prototypes from mymalloc.c

2) mymalloc.c

    -> headers: stdio.h, stdlib.h, mymalloc.h

    -> void print_err(char *file, int line, char *msg)

        -> Takes as parameters a filename, a line number, and an error message

        -> Prints an error message in the following format:
        ERROR: in <file>, line <line>: <msg>

        -> Used by mymalloc() and myfree() to provide more informative error messages than the C standard library's malloc() and free()
    
    -> int print_LL_table()

        -> The library maintains metadata about the simulated heap in a linked list, the nodes of which are of type header_t. (For the remainder of this document, these nodes are referred to as "headers".) print_LL_table() prints a table with a row for each header, showing:
            -> the size in bytes of the payload associated with the header;
            -> the allocated flag for the header; and
            -> the number of bytes between the start of the header and the start of the next header. If there is no next header, NULL is printed.
        The function returns EXIT_SUCCESS if it runs successfully.

    -> int mem_diagnostics(enum diagnostic diag)

        -> Traverses the linked list and returns one of the following diagnostics, depending on the argument provided:
            -> memory_free: total free memory in bytes
            -> total_payload: total payload in memory (whether free or allocated), in bytes
            -> free_chunks: number of free chunks of memory available
            -> total_chunks: total number of chunks of memory
            -> largest_free: size in bytes of the largest free chunk of memory

    -> void split(header_t * alloc_split, size_t size_req)

        ->parameters include: 
            1) header_t * alloc_split: a pointer to the header of a free chunk large enough for split to function. The header type "best_fit" pointer which is obtained through a best fit algorithm within mymalloc() is passed as the argument of this parameter.
            2) size_t size_req: an unisigned int equal to the number of bytes requested when a client calls malloc()

        ->First, split() defines a header type pointer called "free_split" that is positioned within the heap at the end of alloc_split + size_req + byte size of another header.  This is so that it points to the payload rather than the header itself.

        ->The free_split header is then initialized with the size of the remaining free memory, it's alloc flag is set to 0, and it is set to point to the next chunk in memory which is what alloc_split was pointing to originally.

        ->alloc_split's metadata is then revised. It's size is set to the byte size requested by the client, it's alloc flag is set to 1, and it is set to point to the now subsequent header type free_split.

    -> void *mymalloc(size_t size_req, char * file, int line)

        ->

    -> int coalesce(void *p)

        -> Takes one parameter p, a generic pointer. It presumes that p points to the beginning of a payload allocated by mymalloc(). coalesce() does not attempt to verify this because coalesce() is only called by myfree(). myfree(), in turn, calls coalesce() only after it has verified that the pointer being freed and passed to coalesce() does, in fact, point to the beginning of a payload allocated by mymalloc().

        -> Implements our library's strategy for minimizing memory fragmentation, which is to coalesce the chunk of memory associated with p (call it M_p) with the next and previous chunks of memory[] if they are free. coalesce() first attempts this with the next chunk of memory, a pointer to which is availble from M_p's header. If the next chunk is free, then M_p's pointer-to-next is set equal to the next chunk's pointer-to-next.

        -> coalesce() then traverses the linked list from the beginning until it reaches the header immediately prior to M_p's header, if it exists. If this previous header indicates that its associated chunk of memory is free, its pointer-to-next is set equal to M_p's pointer-to-next.

        -> coalesce() returns the number of memory chunks coalesced: 0, 1, or 2.

    -> void myfree(void *p, char *file, int line)

        -> Takes as a parameter a generic pointer p, which is intended to be a pointer to a chunk of memory allocated by mymalloc(). Also takes as parameters a filename and line number, which are supplied through the macro statements malloc(s) and free(p) and are used to generate informative error messages.

        -> First checks to make sure memory is initialized, exits with the following error message if not:
        ERROR: in <file>, line <line>: attempt to free when memory is not initialized

        -> Then traverses the linked list, checking if the payload associated with each header starts at the same address referenced by p
            -> If yes and the header indicates that the chunk is allocated, then set the chunk's allocated flag to 0 and call coalesce().
            -> If yes but the header indicates that the chunk is already free, then this is a usage error of trying to free a chunk that was freed previously. Display the following error message:
            ERROR: in <file>, line <line>: attempt to free memory that is already free
            -> If no but start of the header happens to equal the address referenced by p, then this is a special case of trying to free memory not allocated by mymalloc() and the following error message is displayed:
            ERROR: in <file>, line <line>: attempt to free a memory chunk that starts at a header

        -> If the traversal terminates and none of the conditions above was satisfied, then p references a chunk of memory that was not allocated by mymalloc() and the following error message is displayed:
        ERROR: in <file>, line <line>: attempt to free memory not allocated by malloc()

        -> With the exception of pointers to the start of a header, we do not distinguish between non-malloc() pointers within memory[] and outside of memory[]. Due to the undefined behavior of the > and < operators on pointers in non-contiguous blocks of memory, we would need to examine every byte of memory[] for equality to reliably make this distinction. We decided this was not worthwhile.            

3) memgrind.c

    -> headers: stdio.h, stdlib.h, sys/time.h, mymalloc.h, basic_tests.h

    -> Defines 3 macros(TASK_REPEAT, TASK_SIZE, RAND_SEED)    
        -> TASK_REPEAT set to int value of 50 for number of times each task is performed for the purpose of generating a sample of computation times
        -> TASK_SIZE set to int value of 120 for repeating processes within tasks
        -> RAND_SEED set to integer value of 8675309 for randomization within tasks

    -> The following six functions perform a series of actions (a "task") some number of times based on TASK_SIZE. Each task is designed to run to completion with no error messages or other feedback if successful, and return the int EXIT_SUCCESS upon completion.

        -> int task1() - performs item 4.1 of the assignment: malloc() and immediately free() a 1-byte chunk, 120 times.

        -> int task2() - performs item 4.2 of the assignment: use malloc() to get 120 1-byte chunks, storing the pointers in an array, then use free() to deallocate the chunks.

        -> int task3a() - performs item 4.3 of the assignment: randomly choose between allocating a 1-byte chunk and storing the pointer in an array, and deallocating one of the chunks in the array (if any). Repeat until malloc() has been called 120 times, then free all remaining allocated chunks. Memory chunks are freed in the order in which they were allocated.

        -> int task3b() - also performs item 4.3 of the assignment, but chooses the chunk to be deallocated randomly from among those chunks already allocated.

        -> int task4() - randomly chooses between allocating and deallocating randomly-sized chunks, repeats until malloc() has been called TASK_SIZE times, then frees all remaining allocated chunks. Constrains chunk size to MEMSIZE/TASK_SIZE - sizeof(header_t) to ensure memory requests aren't too large. The intent of this test is to simulate extensive, fully random usage of the heap. Returns EXIT_SUCCESS upon completion.

        -> int task5() - tests that the fragmentation minimization strategy works after memory has been filled by chunks of a randomly chosen size. Calculates a request size between 2 and (MEMSIZE / 8) - sizeof(header_t); requests allocations of this size through malloc() until memory is fully allocated; frees the even-numbered chunks; frees the odd-numbered chunks; then requests one big chunk that takes up all of memory and ensures the request is successful. 

    -> int grind_task(char* task_name, int (*task)())

        -> Takes as parameters a string containing a task name and a pointer to the corresponding task function.

        -> grind_task() runs the task function TASK_REPEAT times and accumulates the time required for each run. It then prints the average time (in microseconds) required to run the task. It returns EXIT_SUCCESS upon successful completion.

    -> void testing_menu(int *p)

    -> int main(int argc, char**argv)

4) basic_tests.h

    -> Contains function prototypes from basic_tests.c

5) basic_tests.c

    -> headers: stdio.h, stdlib.h, mymalloc.h

    -> int print_pass_fail(int condition)

        -> Prints PASS if condition != 0, FAIL otherwise

    -> int set_diff_value_types()

        -> Gets n = user input as an integer. Calls malloc(n * sizeof(type)) with 3 different pointer types (int, char, float) using type casting.  Populate each allocated memory array with it's respective type value, then prints the contents of the arrays. free() all memory to ensure allocation and deallocation of memory works as it should.

    -> int normal_ops()

        -> Runs two tests associated with "normal operations"--see the Test Plan below for background.

            -> Test 1 requests two pointers (p and q) to chunks of memory using malloc(). It casts p and q to be pointers to headers--more precisely, pointers to header_t. Applying pointer artihmetic, it assumes that (p - 1) and (q - 1) are headers and references their size and alloc members, which respectively store the size requested (in bytes) and a 0/1 indicator of whether the memory chunk is allocated. A successful test finds the correct values in these locations. Test 1 prints to screen a message that concludes with PASS if this condition is met, FAIL otherwise.

            -> Test 2 uses the pointers p and q from the two malloc() requests that were made in Test 1. These requests were made from a fully free heap, with p requested first, so a successful test will have the number of bytes between p and q equal to the size of the request for p plus the size of the header. Test 2 prints to screen a message that concludes with PASS if this condition is met, FAIL otherwise.

        -> Returns EXIT_SUCCESS upon successful completion.

    -> int break_things()

        -> Runs five tests for appropriate return values and error messages when users attempt actions outside of the design intentions. See the "Attempted breaks" section of the Test Plan for background.

            -> Test 3 requests a chunk of memory of size MEMSIZE - 2, which is larger than mymalloc() can ever allocate because the header is larger than 2 bytes. A successful test concludes by displaying two messages. The first is generated by the call to mymalloc():
            ERROR: in <file>, line <line>: No sufficient memory to allocate
            The second message is PASS, which is displayed if mymalloc returns a null pointer.

            -> Test 4 requests a chunk of memory of size MEMSIZE - 32 from a fully free heap. Since the header is only 24 bytes, mymalloc() successfully returns a pointer to the requested chunk. Test 4 then requests a chunk of size 64, which mymalloc() should not be able to allocate. A successful test concludes by displaying two messages. The first is generated by the second call to mymalloc():
            ERROR: in <file>, line <line>: No sufficient memory to allocate
            The second message is PASS, which is displayed if the second call to mymalloc returns a null pointer.

            -> Test 5 requests a chunk of memory of size 0. A successful test concludes by displaying two messages. The first is generated by the call to mymalloc():
            ERROR: in <file>, line <line>: malloc cannot allocate less then 1 byte of memory
            The second message is PASS, which is displayed if mymalloc returns a null pointer.

            -> Test 6 requests four chunks of memory from a fully free heap, with chunk 0 taking a large proportion of total capacity (~91%) and chunks 1 through 3 each taking an equal proportion of the remaining capacity (~3% of total capacity each). It frees chunks 1 and 3, then requests an allocation of ~5% of total capacity, which is larger than the size of chunks 1 through 3 but smaller than the total free capacity, which is ~6%. malloc() should nevertheless fail to allocate memory, rather, it should return a null pointer and an informative error message. A successful test concludes by displaying two messages. The first is generated by the second call to mymalloc():
            ERROR: in <file>, line <line>: No sufficient memory to allocate
            The second message is PASS, which is displayed if the second call to mymalloc returns a null pointer.

        -> Returns EXIT_SUCCESS upon successful completion.

    -> int test_range_case()

6) Makefile

    -> Compiles and links all .c files within project folder.
    
    -> Creates executable file called "test" to run testing routine within main() of memgrind.c

----------
TEST PLAN:
----------

The properties of our library can be divided into two broad categories: those that the library exhibits when it is used as intended ("normal operations") and those it exhibits when the user attempts actions outside of the design intentions ("attempted breaks"). This test plan states each property, followed by a description of how we verify that each property holds. Note that all test programs described below are called from the main() function of memgrind.c.

1) Normal operations

    a) The library correctly initializes the memory array: it sets up the memory management data structure if and only if malloc() is called on a memory array that has just been initialized

        -> Make sure data structure is correctly set up on a newly initialized memory array; make sure the code does not attempt to set up a new data structure on an array that has just one header

    b) malloc(k) correctly allocates k bytes of memory from the (simulated) heap
        
        -> When the user requests allocations of various sizes and writes data to the allocated memory locations, the user should be able to read back the correct data from those locations. The set_diff_value_types() function in basic_tests.c performs this test. The user is prompted for a number of elements n. malloc() is then called to request three allocations: one of size n ints, one of size n chars, and one of size n floats. The chunks referenced by the pointers returned by malloc() are populated with increasing sequences of the respective datatypes (1, 2, 3, ...; A, B, C, ...; 0.5, 1.5, 2.5, ...). The contents of each chunk are then printed to the screen. A successful result is a printout of the entirety of each of the entered sequences. An unsuccessful result is any other output.

        -> A secondary test is to ensure that when a user requests an allocation of k bytes, the number of bytes between the pointer to that memory chunk and a pointer to the next chunk in memory is at least k plus the size of the header. Test 2 of the normal_ops() function in basic_tests.c performs a specific instance of this test.

    c) malloc() returns a pointer to the start of the data portion of each chunk, not to the start of the metadata portion, nor to any other address in the memory array, nor to an address outside the memory array. 

        -> Given a pointer p returned from malloc(), the bytes immediately preceding p should compose a valid header. Test 1 of the normal_ops() function in basic_tests.c verifies that this property holds.

    d) malloc() should return first available exact fitting chunk (if one exists) and not split the fitted chunk, but rather return a pointer to the exact fitting chunk and set its allocated flag to true(1)

        -> call malloc(n) 2 times, free a pointer from the first call to malloc(n), call a new malloc(n). Use print_LL_table() to view headers in memory and ensure the new call to malloc(n) was fitted into the first chunk with a payload of size n.

    e) malloc() should handle situations when a client requests a number of bytes within memory, and memory[] contains free chunks of size within the range byte size requested < free chunk available <= (bite size requested + byte size of a header(24 bytes on linux)). In this case malloc() should continue to search for a free chunk large enough to call split() or if no larger chunk is available malloc() should fit the requested amount of bytes into the first available free chunk without calling split to avoid initializing headers with 0 bytes of payload or overwriting memory with a header.

        -> Case 1: Fill heap with chunks of equal size n. free() first and last chunk. Attempt to allocate malloc(n).

        -> Case 2: Fill heap with chunks of equal size n. free() first chunk and last two chunks so they coalesce into 1 chunk of size n*2. Attempt to allocate malloc(n).


2) Attempted breaks

    a) malloc() should not allocate more memory than is available

        -> If a user calls malloc() to request a larger chunk of memory than is available, then malloc() should return a null pointer and an informative error message should be displayed. Tests 3 and 4 of the break_things() function in basic_tests.c are run to verify that this property holds.

    b) malloc() should not allocate memory for requests of size 0 or less

        -> If a user calls malloc() to request 0 or negative bytes, then malloc() should return a null pointer and an informative error message should be displayed. Test 5 of break_things() is run to verify that this property holds.
    
    c) malloc() should exhibit memory safety, meaning that it does not allocate a chunk that extends beyond the boundaries of our memory array

        -> This property is partially tested through item 2a above. Another essential test is to ensure that malloc() does not allocate a chunk that extends beyond the boundaries of memory[], even when the request is smaller than total free memory. Test 5 of break_things() tests an example of this scenario.

    d) free() should detect and report usage errors, including 1) trying to free() addresses not obtained with malloc(); 2) trying to free() addresses within memory[] but not at the start of a payload; 3) calling free() more than once on the same pointer

        -> Try doing each of these, ensure a correct, informative error message is sent to the terminal

    e) The library should minimize memory fragmentation

        -> see task5() in memgrind.c
