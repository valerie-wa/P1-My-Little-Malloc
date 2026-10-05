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
    unsigned short prev_size;
    int isAllocated;
} header;

/*
* initializes heap
* register leak detection func at exit
 */
void init(){
    //initialize the heap as a big free chunk
    header *first_header = (header *) (&heap.bytes[0]);
    first_header->isAllocated = 0;
    heap_initialized = 1;
    first_header->size = ((MEMLENGTH - sizeof(header)) + 7) & ~7;
    first_header->prev_size = NULL;

    //register leak detection function to be called at exit
    atexit(leak_detection);
}

void leak_detection(){
    //cannot call exit() itself
    //if anything still allocated print error msg
    int STD_ERR = 2;
    int obj_count = 0;
    int HEADER_SIZE = 8;
    int byte_count = 0;
    header *current = (header *)heap.bytes;
    while((char*) current < heap.bytes + MEMLENGTH ){
        if(current->isAllocated == 1){
            obj_count ++;
            byte_count += current->size;
        }
        current = (header *)((char *)current + HEADER_SIZE + current->size);
    }
    write(STD_ERR, "mymalloc: %zu bytes leaked in %d objects.\n", byte_count, obj_count);
    return;
}

/*
* always allocates memory in multiples of 8 bytes
* sort through heap starting from front linearly every time to search
* @size: number of bytes to allocate
* returns a pointer to the payload of memory (not the header)
*/
void * mymalloc (size_t size, char *file, int line){\
    int MIN_MEM_SIZE = 16; 
    int HEADER_SIZE = 8;
    int STD_ERR = 2;

    if(!heap_initialized){
        init();
    }

    size = (size + 7) & ~7; // round up to nearest multiple of 8

    //if cannot satisfy request
    if(size == 0 || size > MEMLENGTH - HEADER_SIZE){
        write(STD_ERR, "malloc:Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
        return NULL;
    }

    header *current = (header *) heap.bytes;
    while((char *)current < heap.bytes + MEMLENGTH){ 
        if(current->isAllocated == 0 && current->size >= size){
            if(current->size - HEADER_SIZE - size >= MIN_MEM_SIZE){
                //split chunk
                header *next_header = (header *)((char *)current + HEADER_SIZE + size);
                next_header->size = current->size - HEADER_SIZE - size;
                next_header->isAllocated = 0;
                next_header-> prev_size = size;
            }
            current->isAllocated = 1;
            current->size = size;

            //set prev size for next chunk (if exists)
            header *next_chunk = (header *)((char *)current + HEADER_SIZE + current->size);
            if ((char *)next_chunk < heap.bytes + MEMLENGTH) {//j checking if still in heap
                next_chunk->prev_size = current->size;
            }
            return (char *)current + HEADER_SIZE; //pointer to payload
        }
        current = (header *)((char *)current + HEADER_SIZE + current->size);
    }

    //no chunk found after going through all checks
    write(STD_ERR, "malloc:Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}

/*
* marks chunk free or not afterwards
* automatically coalesces adjacent free chunks
* @memory: pointer to start of payload of memory that's already allocated
* 
*/
void myfree (void *ptr, char *file, int line){
    int STD_ERR = 2;
    int HEADER_SIZE = 8;
    //frees the memory pointed to by payload
    if(!heap_initialized){
        init();
    }

    //if inappropriate calls to free()
    //3. middle of payload/head/not pointing to payload 4. not allocated 
    //ptr null
    if(ptr == NULL){
        write(STD_ERR, "free: Null pointer (%s:%d)", file, line);
        exit(2);
    } 
    //ptr before/after heap?
    if(ptr < (void *) heap.bytes || ptr >= (void *) (heap.bytes + MEMLENGTH)){ // ||middle of payload
        write(STD_ERR, "free: Inappropriate pointer (%s:%d)", file, line);
        exit(2);
    }
    header *head = (header *) ((char*)ptr - HEADER_SIZE);
    if(head -> isAllocated == 0){ //already free
        write(STD_ERR, "free: Double free (%s:%d)", file, line);
        exit(2);
    }

    //coalesce free chunks
    header* next_head = (header *) ((char*)head + head->size + HEADER_SIZE);
    if((char *)next_head < heap.bytes + MEMLENGTH && next_head->isAllocated == 0){
        head->size += (next_head -> size + HEADER_SIZE);
        next_head ->isAllocated = 0;
        next_head = NULL;
        //i do feel like this leaves dangling pointers
        //does prev_size need to updated??
    }
    next_head = NULL;
    header* prev_head = (header *) ((char*)head - head->prev_size - HEADER_SIZE);
    if(prev_head >= (header*)heap.bytes && prev_head->isAllocated == 0){
        prev_head->size += head->size + HEADER_SIZE;
        head->isAllocated = 0;
        head = NULL;
    }

    // freeing. do we need to change/0 anything in it like no??
    head->isAllocated = 0;
    head = NULL;
    return;
}