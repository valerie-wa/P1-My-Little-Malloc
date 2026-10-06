/*
 * things to remember:
 * assume all memory is divisible by 8
 * need line # and name of src file for printing msgs (standard error)
 * memory needs 1. size 2. is it allocated
 * 
 * snprintf
 */

#include "mymalloc.h"
#include <stdio.h>

#define MEMLENGTH 4096
static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;
static int heap_initialized = 0;

//rn the header is 16 bytes
typedef struct {
    size_t size;
    int isAllocated;
} header;

void leak_detection(){
    //cannot call exit() itself
    //if anything still allocated print error msg
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
    //not sure is write can be used like that? maybe use fprintf
    fprintf(stderr, "mymalloc: %d bytes leaked in %d objects.\n", byte_count, obj_count);
    return; 
}

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
    //register leak detection function to be called at exit
    atexit(leak_detection()); //doesnt this need to be called after running leak_detection
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
        fprintf(stderr, "malloc:Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
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
            }
            current->isAllocated = 1;
            current->size = size;
            return (char *)current + HEADER_SIZE; //pointer to payload
        }
        current = (header *)((char *)current + HEADER_SIZE + current->size);
    }

    //no chunk found after going through all checks
    fprintf(stderr,"malloc:Unable to allocate %d bytes (%s:%d)\n", size, file, line);
    return NULL;
}

/*
* marks chunk free or not afterwards
* automatically coalesces adjacent free chunks
* @memory: pointer to start of payload of memory that's already allocated
* 
*/
void myfree (void *ptr, char *file, int line){
    int HEADER_SIZE = 8;
    //frees the memory pointed to by payload
    if(!heap_initialized){
        init();
    }

    //if inappropriate calls to free()
    //3. middle of payload/head/not pointing to payload 4. not allocated 
    //ptr null, does nothing 
    if(ptr == NULL){
        fprintf(stderr, "free: Null pointer (%s:%d)", file, line);//fprintf
        exit(2);
    } 
    //ptr before/after heap?
    /*
    Change from (void *) heap.bytes to (char *)ptr < heap.bytes since it would be easier
    to iterate byte by byte
    */
    if((char*) ptr < heap.bytes || (char *)ptr >= heap.bytes + MEMLENGTH){ // ||middle of payload
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)", file, line);
        exit(2);
    }
   
    header *current = (header *)heap.bytes;
    header* previous = NULL; //let's keep this null as it avoid corrupting the heap since the size of prev would be 0 if set to current

    //iterate through each chunk until we find the chunk that contains ptr
    while((char *)current < heap.bytes + MEMLENGTH){
        void *payload = (char *)current + HEADER_SIZE;
        if (payload == ptr){
            break; //payload found
        }
        previous = current;
        current = (header *)((char *) current + HEADER_SIZE + current->size);

        //if we reach the end of heap, ptr was inside the heap but had no payload
        if((char *)current >=heap.bytes + MEMLENGTH){
            fprintf(stderr, "free: Inappropriate pointer (%s:%d)", file, line);
            exit(2);
        }
    }
    //chunk was already freed
    if(current -> isAllocated == 0){ //already free
        fprintf(stderr, "free: Double free (%s:%d)", file, line);
        exit(2);
    }

    //freeing the chunk
    current->isAllocated = 0;

    //merge with next chunk if possible
    header *next = (header *)((char *)current + HEADER_SIZE + current->size); 

    if((char *)next <heap.bytes + MEMLENGTH && next->isAllocated==0){
        current->size += HEADER_SIZE + next->size;
    }

    //merge with prev chunk is possible

    if(previous != NULL && previous->isAllocated==0){
        previous->size += HEADER_SIZE+current->size;
    }

    /*
    //coalesce free chunks
    header* next_head = (header *) ((char*)current + current->size + HEADER_SIZE);
    if((char *)next_head < heap.bytes + MEMLENGTH && next_head->isAllocated == 0){
        current->size += (next_head -> size + HEADER_SIZE);
        next_head ->isAllocated = 0;
        next_head = NULL;
        //i do feel like this leaves dangling pointers
        //does prev_size need to updated??
    }
    next_head = NULL;
    if((char*)current != heap.bytes){
        previous->size += current->size + HEADER_SIZE;
        current->isAllocated = 0;
        current = NULL;
    }

    // freeing. do we need to change/0 anything in it like no??
    current->isAllocated = 0;
    current = NULL;
    return;
    */
}