#include "mymalloc.h"
//this is for grinding multiple times
/*
* (1) specify
the requirements your library must satisfy, (2) describe how you could determine whether the
requirements have been violated, and (3) write programs to check those conditions
*/

/*
* 4. allocate 2 consecutively, free prev (8 byte pointers)
* 5. allocate alternating 8, 16 bytes, middle -> end, beginning -> middle
*/