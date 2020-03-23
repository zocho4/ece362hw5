/*
Zoe Schmertzler
ilanaz@pdx.edu
ECE 362 HW5 Q1
3/4/20

This is the first program for HW5.
It is a program that creates four threads.
Each prints its thread ID.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

////////THREAD START ROUTINE////////
void *printThrID(void *arg) {
    printf("Thread ID: %d\n", pthread_self());
    return NULL;
}

////////////////MAIN////////////////
int main(void) {
    pthread_t thr1, thr2, thr3, thr4;
    printf("This is main.\n");

    //create thread 1
    int rc1C = pthread_create(&thr1, NULL, printThrID, NULL);
    //check for error
    if (rc1C != 0) {
        printf("Error creating thread 1.\n");
        exit(1);
    }

    //create thread 2
    int rc2C = pthread_create(&thr2, NULL, printThrID, NULL);
    //check for error
    if (rc2C != 0) {
        printf("Error creating thread 2.\n");
        exit(1);
    }

    //create thread 3
    int rc3C = pthread_create(&thr3, NULL, printThrID, NULL);
    //check for error
    if (rc3C != 0) {
        printf("Error creating thread 3.\n");
        exit(1);
    }

    //create thread 4
    int rc4C = pthread_create(&thr4, NULL, printThrID, NULL);
    //check for error
    if (rc4C != 0) {
        printf("Error creating thread 4.\n");
        exit(1);
    }

    //wait for thread 1 to terminate
    int rc1R = pthread_join(thr1, NULL);
    //check for error
    if (rc1R != 0) {
        printf("Error waiting for thread 1.\n");
        exit(1);
    }

    //wait for thread 2 to terminate
    int rc2R = pthread_join(thr2, NULL);
    //check for error
    if (rc2R != 0) {
        printf("Error waiting for thread 2.\n");
        exit(1);
    }

    //wait for thread 3 to terminate
    int rc3R = pthread_join(thr3, NULL);
    //check for error
    if (rc3R != 0) {
        printf("Error waiting for thread 3.\n");
        exit(1);
    }

    //wait for thread 4 to terminate
    int rc4R = pthread_join(thr4, NULL);
    //check for error
    if (rc4R != 0) {
        printf("Error waiting for thread 4.\n");
        exit(1);
    }

    printf("Main is ending.\n");
    exit(0);
}
