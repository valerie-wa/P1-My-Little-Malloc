Project 1 for CS 214 Systems Programming

Made by Valerie Wang (vjw33) & 

Some requirements
- everything must be in chunks of 8 bytes
- headers will only have size and is it allocated, no pointers to before/after
- myfree() will coalesce adjacent chunks by checking the prev and next chunk every time it runs
- 