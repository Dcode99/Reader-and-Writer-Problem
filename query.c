/* File query.c */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"


int main() {
    int id, id2, found = 0;
    int* readptr;
    struct StudentInfo* infoptr;
    int sema_set;
    char stuId[12];

    // Shared memory 2
    id2 = shmget(KEY2, SEGSIZE, IPC_CREAT | 0666);
    if (id2 < 0) {
        perror("create: shmget failed for read_count");
        exit(1);
    }

    // attach read_count memory
    readptr = (int*)shmat(id2, 0, 0);
    if (readptr <= (int*)(0)) {
        perror("create: shmat failed for read_count");
        exit(2);
    }

    /* get the id of the shared memory segment with key "KEY"
     note that this is the segment, created in the program "create",
     where the data is stored */
    id = shmget(KEY, SEGSIZE, 0);
    if (id < 0) {
        perror("change: shmget failed 1");
        exit(1);
    }

    /* attach the already created shared memory segment to the prcesse's
       address space and make infoptr point to the begining of the
       shared memory segment so the shared memory segment can be
       accessed through 'inforptr'
       */
    infoptr = (struct StudentInfo*)shmat(id, 0, 0);
    if (infoptr <= (struct StudentInfo*)(0)) {
        perror("change: shmat failed");
        exit(2);
    }

    //Start querying
    // ask for student ID
    printf("Enter the Student ID: ");
    scanf("%s", stuId);

    /* get the id of the semaphore set associated with SEMA_KEY, created by the
"create" process */
    sema_set = semget(SEMA_KEY, 0, 0);

    printf("the value of sema_set=%d\n", sema_set);
    // semaphore for readers
    Wait(sema_set, 0);
    *readptr += 1;
    // semaphore for writers
    if (*readptr == 1) {
        Wait(sema_set, 1);
    }
    Signal(sema_set, 0);

    //check for student ID in the database
    int count = 0;
    while (strlen(infoptr[count].name) > 1 && count <= 50) {
        //printf("%d\n", strcmp(infoptr[count].stuId, stuId));
        if (strcmp(infoptr[count].stuId, stuId) == 13) {
            printf("Name: %s Student ID: %s Address: %s Phone Number: %s\n",
                infoptr[count].name, infoptr[count].stuId, infoptr[count].address, infoptr[count].telNumber);
            found = 1;
        }
        count += 1;
    }
    if (found == 0) {
        printf("No matching record found\n");
    }
    sleep(2);

    //Signal and exit after printing query results
    Wait(sema_set, 0);
    *readptr -= 1;
    if (*readptr == 0) {
        Signal(sema_set, 1);
    }
    Signal(sema_set, 0);

    exit(0);
}