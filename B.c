/*
Zoe Schmertzler
ilanaz@pdx.edu
ECE 362 HW5 Q2
3/4/20

This is the second program for HW5.

*/

////////Preprocessor////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define NUM_ELEMENTS 64000

////////Globals////////
pthread_mutex_t lock;

//////////Typedef//////////
typedef struct { //struct that holds arguments for each thread
    int threadsIndex;
    int numThreads;
    int subTotal;
    int *intArray;
    int *threadSum;
} threadArgs_t, *threadArgsPtr_t;

//////////Prototypes//////////
void *calcSubTotal(void *arg);
int getRand(int min, int max);
void initThreadArgs();

///////////////////////////////MAIN///////////////////////////////
int main(int argc, char *argv[]) {

    //initialize input variables
    int randSeed = 1; //I have made the default 1
    int numThreads = 2; //I have made the default 2
    int intMin = 0; //min value of random number, chose default as 0
    int intMax = 100; //max value of random number, chose default as 100
    int currentArg = 1; //we'll start with checking argv[1]

    int intArray[NUM_ELEMENTS];
    int deterministicSum = 0;
    //declare the threads/thread args here because it's ok for these to be local only
    pthread_t threads[8]; //can have up to 8 threads, may not use them all
    threadArgs_t threadArgs[8]; //can ahve up to 8 of these too, may not use them all
    //could have allocated dynamically but that would involve using more pointers
    int threadSum = 0;

    //check and parse the command line arguments
    printf("argc is %d\n", argc);
    //better to use a while because we aren't exactly iterating through each argv
    while (currentArg < argc) { //don't include argc because array starts at 0
        printf("argv[%d] is %s\n", currentArg, argv[currentArg]);
        //check for each flag case
        if (strcmp(argv[currentArg], "-t") == 0) {
            //check that next argv is a number and print as string
            //ok to assume that the next argv is an ascii number
            int reqThrNum = atoi(argv[++currentArg]);
            printf("The requested number of threads is %d.\n", reqThrNum);
            //check that the number of threads requested is valid
            if ( reqThrNum==1 | reqThrNum==2 | reqThrNum==4 | reqThrNum==8 ) {
                numThreads = reqThrNum;
            }
            else {
                printf("Requested number of threads not valid.\n");
                printf("Number of threads set to default value, 2.\n");
            }
            //increment currentArg again to check next flag
            ++currentArg;
        } //end if for checking first flag
        else if (strcmp(argv[currentArg], "-s") == 0) {
            //same deal for the srand() seed
            printf("The seed is %s as a string.\n", argv[currentArg+1]);
            randSeed = atoi(argv[++currentArg]);
            printf("The seed is %d as an integer.\n", randSeed);
            ++currentArg;
        } //end else for checking second flag
        else {
            printf("Not a valid command line argument!\n");
            //skip to checking the next argv
            ++currentArg;
        }
    } //end while (currentArg < argc)

    printf("This is main, numThreads=%d, randSeed=%d\n", numThreads, randSeed);

    //Initialize the seed for the rand() function
    srand(randSeed);

    for (int i=0; i<NUM_ELEMENTS; ++i) {
        intArray[i] = getRand(intMin, intMax);
        deterministicSum += intArray[i];
    }

    printf("First 5 integers are:\n");
    for (int i=0; i<5; ++i) {
        printf("%d\n", intArray[i]);
    }
    printf("The expected sum is %d.\n", deterministicSum);

    //create the threads
    for (int i=0; i<numThreads; ++i) {
        //the thread and arg structs are already declared
        //use the function to initialize all the arguments in the struct
        initThreadArgs(&threadArgs[i], i, numThreads, intArray, &threadSum);
        //create thread for this thread and its args
        int rc = pthread_create(&threads[i], NULL, calcSubTotal, &threadArgs[i]);
        //minimally check for error
        if (rc != 0) {
            printf("Error creating thread %d.\n", i);
            exit(1);
        }
    }

    //wait for threads to finish
    //this for loop must be separate so the threads can run concurrently
    for (int i=0; i<numThreads; ++i) {
        //no need to pass in a pointer to return args, subtotal is in threadArgs
        int rc = pthread_join(threads[i], NULL);
        //minimally check for error
        if (rc != 0) {
            printf("Error joining thread %d.\n", i);
            exit(1);
        }
        //threadSum += threadArgs[i].subTotal;
        printf("Thread %d is done, threadSum is now %d.\n", threadSum);
    }

    //all threads have completed, now compare sums and finish
    printf("Back in main.\n");
    if (deterministicSum == threadSum) {
        printf("SUCCESS!!\n");
        printf("The multi-threaded sum is also %d.\n", threadSum);
    }
    else {
        printf("ERROR: Multi-threaded sum is not correct.\n");
        printf("Thread Sum = %d\n", threadSum);
    }

    return 0;
}
/////////////////////////////END MAIN//////////////////////////////



//////////Function Definitions//////////

//returns random number between min and max (originally written for hw1)
int getRand(int min, int max) {
    int modVal = (max-min) + 1;
    int randNum = (rand() % modVal) + min;
    return randNum;
}

/* Thread start routine.
Inputs: the void* is actually a  threadArgsPtr_t
Outputs: no return args struct
Actions: add up this threads portion of elements into subTotal
         print thread ID and subTotal
         obtain lock
         update threadSum
         return lock
Note: the algorithm for the addition is to have each thread start on its index number
      then increment to the next array index that is numThreads indexes away
      in this method sequential elements are added by a sequence of threads
      rather than one thread adding a sequential block of elements
*/
void *calcSubTotal(void *arg) {
    //initialize local version of input args struct pointer
    threadArgsPtr_t args = (threadArgsPtr_t)arg;
    //do the addition
    //start at the index of this thread
    //increment by the number of threads
    //stop on the last element index
    for (int i= args->threadsIndex; i<NUM_ELEMENTS; i+= args->numThreads) {
        //add this element to the subTotal of this thread
        args->subTotal += args->intArray[i];
    }
    printf("This is thread %d, subtotal is %d\n", pthread_self(), args->subTotal);
    pthread_mutex_lock(&lock);
        printf("threadSum is %d before update\n", *(args->threadSum) );
        //add this thread's subTotal to the current threadSum
        *(args->threadSum) += args->subTotal;
        printf("threadSum is %d after update\n", *(args->threadSum) );
    pthread_mutex_unlock(&lock);
    return NULL;
}

/* Function for initializing the struct of input arguments for each thread.
Inputs: pointer to struct, index number of this thread, pointer to array, pointer to global sum
Outputs: does not explicitly return anything
Actions: Initialize each member of the arg struct
*/
void initThreadArgs(threadArgsPtr_t argStruct, int threadsIndex, int numThreads, int *intArray, int *threadSum) {
    argStruct->threadsIndex = threadsIndex;
    printf("Initializing thread %d\n", argStruct->threadsIndex);
    argStruct->numThreads = numThreads;
    printf("There are %d threads total\n", argStruct->numThreads);
    argStruct->subTotal = 0;
    printf("The subtotal is initialized to %d\n", argStruct->subTotal);
    argStruct->intArray = intArray;
    printf("Second element of array is %d\n", argStruct->intArray[1]);
    argStruct->threadSum = threadSum;
    printf("Global sum is %d\n", *(argStruct->threadSum) );
    printf("\n");
    return;
}

/*
        initThreadArgs(&threadArgs[i], i, numThreads, intArray, &threadSum);

typedef struct { //struct that holds arguments for each thread
    int threadsIndex;
    int numThreads;
    int subTotal;
    int *intArray;
    int *threadSum;
} threadArgs_t, *threadArgsPtr_t;
*/
