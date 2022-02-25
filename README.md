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
