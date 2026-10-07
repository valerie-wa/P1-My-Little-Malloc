#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//this is for grinding multiple times


//Allocate objects of sizes 8, 16, 32, 64, 128, 512, and 1024 and free them in reverse order.
void task1(){
    void* objects[7];
    int sizes[7] = {8,16,32,64,128,512,1024};
    //allocating each object
    for(int i=0; i<7;i++){
        objects[i] = malloc(sizes[i]);
    }
    //now free in reverse order
    for(int i=6;i>=0;i--){
        free(objects[i]);
    }
}

//Allocate 120 small objects, store their pointers in an array, then free them in the same order they were allocated.
void task2(){
    void *objects[120];
    //allocate the 120 objects in memory
    for(int i=0; i<120; i++){
        objects[i] = malloc(1);
    }
    //free them in allocation order
    for(int i=0; i<120;i++){
        free(objects[i]);
    }
}

void task3(){
    //create an array of 120 pointers and repeatedly randomly allocating between
    //allocating 1 byte objects or freeing a randomly selected allocated object
    void *objects[120];
    int active = 0;
    int allocations = 0;

    while(allocations<120){
        //randomly choose between allocating and freeing
        //where 0 = allocate and 1 = free
        int randOp = rand() % 2;

        //if nothing is allocated than we must allocate, otherwise allocate/free
        if(randOp == 0 || active == 0){
            objects[active] = malloc(1);

            active++;
            allocations++;
        }
        else{
            //pick a random currently allocated block object and free it
            int index = rand()% active;
            free(objects[index]);

            //move the last active pointer into the freed spot, so we can keep all
            //active pointers together at the beginning of the array

            objects[index] = objects[active-1];
            active--;
        }
    }
    //free everything that is still alloated
      while(active>0){
        active--;
        free(objects[active]);
    }
}

void task4(){
    //stress testing fragmentation and coalescing of chunks
    void *objects[50];

    //allocate objects of different sizes
    for (int i=0; i<50; i++){
        objects[i] = malloc((i % 5 +1 )*8);
    }

    //free every other object
    for(int i=0; i<50; i+=2){
        free(objects[i]);
        objects[i] = NULL;
    }
    //free the remaining objects
    for(int i=1;i<50;i+=2){
        free(objects[i]);
    }
}

void task5(){
    void *objects[60];
    
    //first round of allocations
    for(int i=0;i<60; i++){
        objects[i] = malloc(16);
    }

    //free the first half
    for(int i=0; i<30; i++){
        free(objects[i]);
    }

    //Reuse the first half of the freed memeory with different sized allocations
    for(int i=0; i<30; i++){
        objects[i] = malloc(8);
    }

    //free everything
    for(int i=0; i<60; i++){
        free(objects[i]);
    }
}

void workload(){
    //this function will call to all the task functions
    task1();
    task2();
    task3();
    task4();
    task5();
}

int main(void){
    //this will call to workload 50 times and record the performance time
    struct timeval start;
    struct timeval end;
    
    //num of times we'll be runnig the tasks
    const int EXECUTIONS = 50;
    gettimeofday(&start, NULL);

    for(int i=0; i<EXECUTIONS; i++){
        workload();
    }

    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;

    long microsecs = end.tv_usec - start.tv_usec;

    long total_time = seconds * 1000000 + microsecs;

    double average_time = (double)total_time / EXECUTIONS;

    printf("Average workload time: %.2f microseconds\n", average_time);

    return 0;
}