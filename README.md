Project 1 for CS 214 Systems Programming

Made by Valerie Wang (vjw33) & David Goncalves (drg144)

Some requirements
- everything must be in chunks of 8 bytes
- headers will have 1) size 2) isAllocated 3)prev_size and always be 8 bytes
- myfree() will coalesce adjacent chunks by checking the prev and next chunk every time it runs


MemTest Testing Plan
1. malloc 5 chunks, free first chunk
    this is testing previous coalescing edge case
2. malloc 5 chunks, free 3 middle chunks
    this is testing forwards and backward coalescing
3. malloc 1 chunk
4. free pointer to middle of payload
(#5-10 are testing errors. to test them we deleted the exit(2) method after each error in mymalloc and then replaced it after testing. )
5. free a chunk that's already free
6. allocate 0 bytes, 4088 bytes
7. test splitting a chunk when mallo
8. malloc everything, then malloc smth again
9. free null pointer
10. free pointer pointing to middle of empty memory

MyMalloc Design
mymalloc.c uses a 4096-byte heap divided into chunks. Each chunks has an 8 byte header storing the payload size and whether the chunk is allocated. Payload sizes
are rounded up to mulitples of 8 to account for allignment. 

mymalloc uses first fit allocation (meaning the first available space gets used) by scanning from the beginning of the heap for the first free chink large enough for
the request. If enough sapce remains for another valid chunk, the free chunk is split. Otherwise, te whole chunk is allocated.

myfree scans the heap to make sure the pointed matches the start of a valid payload. It detects invalid pointers and free requests for already freed memory. After freeing a 
chunk, it coalesces with adjacent free chunks to reduce fragmentation.

The heap is initialized as one large free chunk. A leak detector registered with atexit() scans the heap when the program ends and reports any allocated chunks that were not
freed

MemGrind Testing Plan
1. Allocate sizes 8,16,32,64,128,512,1024, then free them in reverse order. This also test different size allocations and coalescing
2. Allocate 120 one-byte objects, then free them in allocation order. Tests many small allocations and frees
3. Randomly allocate one-byte objects or free and active object until 120 allocations have occured. Tests random allocation/free patterns and fragmentation
4. Allocate 50 objects of different sizes, free every other object, then free the rest. Tests fragmentation and forward/backward coalescing
5. Allocate 60 16 byte objects, free half, reuse the freed space with 8 byte allocations, then free everything. This tests memeory use and splitting.
6. The workload is ran 50 times and gettimeofday() used to time it takes for the total workload to be completed. Time is reported in microseconds. 