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
#include <stdint.h>

#define MEMLENGTH 4096
static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;
static int heap_initialized = 0;

//rn the header is 8 bytes and the payload can be a min of 8 bytes, making a min 16 byte chunk 
typedef struct {
    uint32_t size;
    uint32_t isAllocated;
} header;

void leak_detection(){
    //cannot call exit() itself
    //if anything still allocated print error msg
    int obj_count = 0;
    int HEADER_SIZE = 8;
    size_t byte_count = 0; //changed to size_t since we are using memory sizes
    header *current = (header *)heap.bytes;
    while((char*) current < heap.bytes + MEMLENGTH ){
        if(current->isAllocated == 1){
            obj_count ++;
            byte_count += current->size + HEADER_SIZE;
        }
        current = (header *)((char *)current + HEADER_SIZE + current->size);
    }
    if (obj_count>0){
         //only counting memory in payloads
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", byte_count, obj_count);
        exit(2);
    }
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
    atexit(leak_detection); //doesnt this need to be called after running leak_detection
}

/*
* always allocates memory in multiples of 8 bytes
* sort through heap starting from front linearly every time to search
* @size: number of bytes to allocate
* returns a pointer to the payload of memory (not the header)
*/
void * mymalloc (size_t size, char *file, int line){
   //REMOVED MIN_MEM_SIZE

    size_t HEADER_SIZE = 8;
    size_t requested_size = size; //we'll use this to display the original requested size before it being rounded. helps with processing invalid sizes

    //initialize the heap the first time malloc/free is used
    if(!heap_initialized){
        init();
    }
    
    // round up to nearest multiple of 8 for allignment 
    size = (size + 7) & ~7; 

    //if cannot satisfy request, send error
    if(size == 0 || size > MEMLENGTH - HEADER_SIZE){
        fprintf(stderr, "malloc:Unable to allocate %zu bytes (%s:%d)\n", requested_size, file, line);
        exit(2);
    }

    //start searching for free memeory at the first chunk of heap
    header *current = (header *) heap.bytes;

    //walk through each chunk in theheap. 
    //current always points to the header of the chunk
    while((char *)current < heap.bytes + MEMLENGTH){ 
        //check if chunk is currently free and has enought payload space for request

        if(current->isAllocated == 0 && current->size >= size){
            //determine whether the chunk should be split, otherwise allocate entire current chunk
            if(current->size >= size + HEADER_SIZE + 8){ //+8 since the min payload should be 16 bytes? 
                //split chunk
                header *next_header = (header *)((char *)current + HEADER_SIZE + size);
                next_header->size = current->size - HEADER_SIZE - size;
                next_header->isAllocated = 0;
                current->size = size; // moved this here so we can shrink size when we actually split
            }
            current->isAllocated = 1;
            
            return (char *)current + HEADER_SIZE; //pointer to payload
        }
        //chunk could not satisfy request, move to next chunk
        current = (header *)((char *)current + HEADER_SIZE + current->size);
    }

    //no chunk found after going through all checks
    fprintf(stderr,"malloc:Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    exit(2);
}

/*
* marks chunk free or not afterwards
* automatically coalesces adjacent free chunks
* @ptr: pointer to start of payload of memory that's already allocated
* 
*/
void myfree (void *ptr, char *file, int line){
    int HEADER_SIZE = 8;
    //frees the memory pointed to by payload
    if(!heap_initialized){
        init();
    }

   //Check for null pointer
    if(ptr == NULL){
        fprintf(stderr, "free: Null pointer (%s:%d)\n", file, line);//fprintf
        exit(2);
    } 
    //make sure ptr points somehwere in the heap
    /*
    Change from (void *) heap.bytes to (char *)ptr < heap.bytes since it would be easier
    to iterate byte by byte
    */
    if((char*) ptr < heap.bytes || (char *)ptr >= heap.bytes + MEMLENGTH){ // ||middle of payload
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }
   
    //Start the first chunk
    //current points to the chunk currently being examined
    //prev remembers the chunk immedietly before current so we can coalesce
    header *current = (header *)heap.bytes;
    header* previous = NULL; //let's keep this null as it avoid corrupting the heap since the size of prev would be 0 if set to current

    //iterate through each chunk until we find the chunk that contains ptr. 
    //Have to start the beginning of the heap each pass 
    while((char *)current < heap.bytes + MEMLENGTH){
        void *payload = (char *)current + HEADER_SIZE;
        if (payload == ptr){
            break; //payload found
        }
        previous = current;//save current chunk before moving to the next one
        current = (header *)((char *) current + HEADER_SIZE + current->size);

        //if we reach the end of heap, ptr was inside the heap but had no payload, thereofre not a valid ptr
        if((char *)current >=heap.bytes + MEMLENGTH){
            fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
            exit(2);
        }
    }
    //asks to be freed, but is the chunk was already freed
    if(current -> isAllocated == 0){ //already free
        fprintf(stderr, "free: Double free (%s:%d)\n", file, line);
        exit(2);
    }

    //freeing the chunk
    current->isAllocated = 0;

     //find the chunk immedietly after current and we move past the current chunk's header and payload
    header *next = (header *)((char *)current + HEADER_SIZE + current->size); 

   //if the chunk exists and is also free,combine it 
    if((char *)next <heap.bytes + MEMLENGTH && next->isAllocated==0){
        current->size += HEADER_SIZE + next->size;
    }

    //merge with prev chunk is possible

    if(previous != NULL && previous->isAllocated==0){
        previous->size += HEADER_SIZE+current->size;
    }
    return;
}