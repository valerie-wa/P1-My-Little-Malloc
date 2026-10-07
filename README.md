Project 1 for CS 214 Systems Programming

Made by Valerie Wang (vjw33) & 

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