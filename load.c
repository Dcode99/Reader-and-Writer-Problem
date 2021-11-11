/* File load.c */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"
#define LINE_SIZE 100


int main()
{
    int id1, id2;
    struct StudentInfo* infoptr;
    struct StudentInfo arr[50];
    int* readptr;
    int sema_set;
    const char* fileName = "student.txt";
    FILE* studentFile;
    int count;
    // counter for keeping track of what data to look at (name, stuId, address, telNumber)
    int data = 0;

    // Shared memory 2
    id2 = shmget(KEY2, SEGSIZE, IPC_CREAT | 0666);
    if (id2 < 0) {
        perror("create: shmget failed for read_count");
        exit(1);
    }

    // attach read_count memory
    readptr = (int*)shmat(id2, NULL, 0);
    if (readptr <= (int*)(0)) {
        perror("create: shmat failed for read_count");
        exit(2);
    }

    *readptr = 0;

    // Shared memory 1
    id1 = shmget(KEY, SEGSIZE, IPC_CREAT | 0666);/* get shared memory to store data*/
    if (id1 < 0) {
        perror("create: shmget failed");
        exit(1);
    }

    /*attach the shared memory segment to the process's address space */
    infoptr = (struct StudentInfo*) shmat(id1, 0, 0);
    if (infoptr <= (struct StudentInfo*)(0)) {
        perror("create: shmat failed");
        exit(2);
    }

    sema_set = GetSemaphs(SEMA_KEY, NUM_SEMAPHS); /* get a set of NUM_SEMAPHS semaphores*/
    if ((sema_set < 0)) {
        perror("create: semget failed");
        exit(2);
    }

    //get filename
    printf("Reading from file\n");
    studentFile = fopen(fileName, "r");
    if (studentFile == NULL)
    {
        fprintf(stderr, "Couldn't open file\n");
        exit(1);
    }

    char line[LINE_SIZE] = { 0 };
    // loop through users
    while (fgets(line, LINE_SIZE, studentFile)) {
        // store example data in the shared memory segment
        if (data == 0) {
            strcpy(arr[count].name, line);
            data++;
        }
        else if (data == 1) {
            strcpy(arr[count].stuId, line);
            data++;
        }
        else if (data == 2) {
            strcpy(arr[count].address, line);
            data++;
        }
        else if (data == 3) {
            strcpy(arr[count].telNumber, line);
            data = 0;
            count++;
        }
    }
    //infoptr = malloc(sizeof(struct StudentInfo) * count);

    int i = 0;
    while (i < count) {
        strcpy(infoptr[i].name, arr[i].name);
        strcpy(infoptr[i].stuId, arr[i].stuId);
        strcpy(infoptr[i].address, arr[i].address);
        strcpy(infoptr[i].telNumber, arr[i].telNumber);
        // move to next student's info
        i++;
    }

    //close file and print done
    printf("file loaded\n");
    fclose(studentFile);

    // Exit load process
    exit(0);
}
