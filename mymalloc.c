/*
 * things to remember:
 * assume all memory is divisible by 8
 * need line # and name of src file for printing msgs (standard error)
 * memory needs 1. size 2. is it allocated
 * 
 * 
 */

#include "mymalloc.h"

#define MEMLENGTH 4096
static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;
static int heap_initialized = 0;

typedef struct {
    size_t size;
    int allocated;
} header;

/*
* initializes heap
* register leak detection func at exit
 */
void init(){
    //initialize the heap as a big free chunk
    header *first_header = (header *) heap.bytes;
    first_header->size = MEMLENGTH - sizeof(header);
    first_header->allocated = 0;
    heap_initialized = 1;

    //register leak detection function to be called at exit
    atexit(leak_detection);
}

void leak_detection(){
    //cannot call exit() itself
    //if anything still allocated print error msg
}

/*
* always allocates memory in multiples of 8 bytes
* sort through heap starting from front linearly every time to search
* @size: number of bytes to allocate
* returns a pointer to the payload of memory (not the header)
*/
void * mymalloc (size_t size, char *file, int line){
    int STD_ERR = 2;
    if(!heap_initialized){
        init();
    }
    //if cannot satisfy request
    if(size == 0 || size > MEMLENGTH - sizeof(header)){
        write(STD_ERR, "malloc:Unable to allocate %zu bytes (%s:%d)", size, file, line);
        return NULL;
    }

    header *current = (header *) heap.bytes;
    void *payload = NULL;

    size = (size + 7) & ~7; // round up to nearest multiple of 8
    while(){
        // #actually mallocing booo
    }
}

/*
* marks chunk free or not afterwards
* automatically coalesces adjacent free chunks
* @memory: pointer to start of payload of memory that's already allocated
* 
*/
void myfree (void *ptr, char *file, int line){
    int STD_ERR = 2;
    //frees the memory pointed to by payload
    if(!heap_initialized){
        init();
    }
    //if inappropriate calls to free(): 1. null 2. before/after heap 3. middle of payload 4. not allocated  & not obtained fro mmalloc??
    if(ptr == NULL || ptr < (void *) heap.bytes || ptr >= (void *) (heap.bytes + MEMLENGTH)){ // ||middle of payload
        write(STD_ERR, "free: Inappropriate pointer (%s:%d)", file, line);
        exit(2);
    }
    // # actually freeing boo
    return;
}