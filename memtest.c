#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#include "mymalloc.h"
#endif

// Compile with -DLEAK to leak memory
#ifndef LEAK
#define LEAK 0
#endif

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

int 
main (int argc, char **argv)
{
	char *obj[OBJECTS];
	int i, j, errors = 0;
	
	// fill memory with objects
	for (i = 0; i < OBJECTS; i++) {
		obj[i] = malloc (OBJSIZE);
		if (obj[i] == NULL) {
		    printf ("Unable to allocate object %d\n", i);
		    exit (EXIT_FAILURE);
		}
	}
	
	// fill each object with distinct bytes
	for (i = 0; i < OBJECTS; i++) {
		memset (obj[i], i, OBJSIZE);
	}
	
	// check that all objects contain the correct bytes
	for (i = 0; i < OBJECTS; i++) {
		for (j = 0; j < OBJSIZE; j++) {
			if (obj[i][j] != i) {
				errors++;
				printf ("Object %d byte %d incorrect: %d\n", i, j, obj[i][j]);
			}
		}
	}

	// free all objects
	if (!LEAK) {
	    for (i = 0; i < OBJECTS; i++) {
		free (obj[i]);
	    }
	}
	
	printf("%d incorrect bytes\n", errors);
	


	// our tests----------------------------------------
	//1. malloc 5 chunks free 1st chunk
	char *arr1[5];
	for (int i = 0; i < 5; i++){
		arr1[i] = malloc(9);
	}

	free(arr1[0]);
	printf("(1) There should be 96 leaked bytes in 4 objects.\n");

	//2. Testing backward and forward coalescing. freeing middle 3/5 chunks
	arr1[0] = malloc(9);
	free(arr1[2]);
	free(arr1[1]);
	free(arr1[3]);
	printf("(2) Middle of heap should be coalesced, now allocating alloted space in middle: 64 bytes\n");
	char* middle = malloc(64);
	printf("(2) If i get here there was no error allocating 64 bytes.\n");

	//3 malloc 1 block
	free(arr1[0]);
	free(middle);
	free(arr1[4]);
	printf("(3) Heap is now empty again.\n");
	printf("(3) Mallocing one block.\n");
	char *one_block = malloc(3234);
	
	//4 free pointer pointing to middle of payload
	char *payload_middle = one_block + 123; 
	free(payload_middle);
	printf("(4) Should return inappropriate pointer error\n");

	//5 free chunk that's already free
	char *one_block2 = one_block;
	free(one_block);
	free(one_block2);
	printf("(5) Should return double free error\n");
	
	//6 allocating 0 and full heap
	//nothing allocated right ow
	int *zero_bytes = malloc(0);
	int *full_heap = malloc(4089); //4088 (4096-8) is limit 
	printf("(6) Should return unable to allocate error twice for 0 and 4096.\n");

	//7 testing if malloc can split free chunks when allocating
	char* arr2[7];
	arr2[0] = malloc(8);
	arr2[1] = malloc(48); //56 bytes in middle including header
	arr2[2] = malloc(4016);
	free(arr2[1]);
	arr2[3] = malloc(16);//24
	arr2[4] = malloc(8); //16
	printf("(7) If no errors than successfully splitting into chunks when allocating.\n");

	//8 malloc everything then try mallocing when heap full
	//mallocing remaining portion
	arr2[5] = malloc(8); //whole heap allocated
	//allocating something else
	arr2[6] = malloc(8);
	printf("(8) Should throw unable to allocate 8 bytes.\n");

	//9 free null pointer
	char* nullptr = NULL;
	free(nullptr);
	printf("(9) Should throw null pointer.\n");

	//10 free ptr pointing to middle of empty memory
	free(arr2[1]);
	free(arr2[1] + 8);
	printf("(10) Should throw inappropriate pointer. \n");

	//11 Freeing wiht address not from malloc
	int x;
	free(&x);
	printf("(11) Should throw inappropriate pointer error.\n");
		
	
	//freeing everything
	free(arr2[0]);
	free(arr2[2]);
	free(arr2[4]);
	free(arr2[5]);
	
	//put exit(2) back after ea error message
	return(EXIT_SUCCESS);
}