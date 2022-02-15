#define malloc(s) mymalloc(s, __FILE__, __LINE__)
#define free(p) myfree(p, __FILE__, __LINE__)

typedef struct header header_t;

struct header {
    int allocated;
    int size;
    header_t *next;
};

void *mymalloc(size_t size, char *file, int line);
void myfree(void *p, char *file, int line);
int coalesce(void *p);
header_t *get_header(void *p);
int is_initialized();