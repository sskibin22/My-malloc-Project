#define malloc(s) mymalloc(s, __FILE__, __LINE__)
#define free(p) myfree(p, __FILE__, __LINE__)

typedef struct header header_t;

struct header {
    size_t size;
    int alloc;
    header_t *next;
};

header_t *get_header(void *p);
int is_initialized();
void initialize();
void split(header_t *alloc_fit, size_t size);
void *mymalloc(size_t size, char *file, int line);
int coalesce(void *p);
void myfree(void *p, char *file, int line);