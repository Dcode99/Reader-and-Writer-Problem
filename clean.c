
/* File clean.c */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main()
{

    int id, id2;
    int* readptr;
    struct StudentInfo* infoptr;
    int sema_set;
    char filename[] = "save.txt";
    FILE* save_file;

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

    id = shmget(KEY, SEGSIZE, IPC_CREAT | 0666);/* get shared memory to store data*/
    if (id < 0) {
        perror("create: shmget failed");
        exit(1);
    }

    infoptr = (struct StudentInfo*)shmat(id, 0, 0);/*attach the shared memory segment
                         to the process's address space */
    if (infoptr <= (struct StudentInfo*)(0)) {
        perror("create: shmat failed");
        exit(2);
    }

    sema_set = GetSemaphs(SEMA_KEY, NUM_SEMAPHS); /* get a set of NUM_SEMAPHS
                             semaphores*/
    if ((sema_set < 0)) {
        perror("create: semget failed");
        exit(2);
    }

    //get filename
    printf("Writing to file\n");
    save_file = fopen(filename, "w");
    if (save_file == NULL)
    {
        fprintf(stderr, "Couldn't open file\n");
        exit(1);
    }

    // wait for write semaphore permission before saving and deleting
    Wait(sema_set, 1);

    // loop through users
    int count = 0;
    //check if next name and if count is greater than allocated space
    while (strlen(infoptr[count].name) > 1 && count <= 50) {
        fprintf(save_file, "%s %s %s %s",
            infoptr[count].name, infoptr[count].stuId, infoptr[count].address, infoptr[count].telNumber);
        count += 1;
    }

    printf("File written\n");

    // remove reader count shared memory
    shmdt((char*)readptr); /* detach the shared memory segment */
    shmctl(id2, IPC_RMID, (struct shmid_ds*)0); /* destroy the shared memory segment*/
    // remove main shared memory
	shmdt((char*)infoptr); /* detach the shared memory segment */
	shmctl(id, IPC_RMID, (struct shmid_ds*)0); /* destroy the shared memory segment*/

	semctl(sema_set, 0, IPC_RMID); /*Remove the semaphore set */
	exit(0);
}