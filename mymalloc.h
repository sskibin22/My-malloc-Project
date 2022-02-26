#define malloc(s) mymalloc(s, __FILE__, __LINE__)
#define free(p) myfree(p, __FILE__, __LINE__)

//define total byte size of heap
#define MEMSIZE 4096

typedef struct header header_t;

struct header {
    size_t size;
    int alloc;
    header_t *next;
};

// enum for memory diagnostics
enum diagnostic {memory_free, total_payload, free_chunks, total_chunks,
                largest_free};

int print_LL_table();
int mem_diagnostics(enum diagnostic);
void print_err(char *, int, char *);
void initialize();
void split(header_t *alloc_fit, size_t size);
void *mymalloc(size_t size, char *file, int line);
int coalesce(void *);
void myfree(void *, char *, int);