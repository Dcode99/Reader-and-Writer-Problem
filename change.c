/*File change.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sem.h>
#include <pwd.h>
#include "header.h"
#define SIZE 150

int main(int argc, char* argv[])
{
  int id, found = -1, count = 0;
  struct StudentInfo *infoptr;
  int sema_set;
  char input[10];
  char name[40];
  char stuId[12];
  char address[150];
  char telNumber[15];
  if (argc != 1){
    fprintf(stderr, "usage: change\n");
    exit(3);
  } 

//Check for correct password
 char pwd[32];
 printf("Enter the password: ");
 fgets(pwd, SIZE, stdin);
 pwd[strcspn(pwd, "\n")] = '\0';
 if (strcmp(pwd, "000")){
   printf("error: Incorrect Password\n");
   exit(1);
 }


/* get the id of the shared memory segment with key "KEY" 
 note that this is the segment, created in the program "create",
 where the data is stored */
  id = shmget(KEY,SEGSIZE, 0);
  if (id <0){
    perror("change: shmget failed 1");
    exit(1);
  }

/* attach the already created shared memory segment to the prcesse's
   address space and make infoptr point to the begining of the 
   shared memory segment so the shared memory segment can be
   accessed through 'inforptr'
   */
  infoptr=(struct StudentInfo *)shmat(id,0,0);
  if (infoptr <= (struct StudentInfo *) (0)) {
    perror("change: shmat failed");
    exit(2);
  }
  //ask writer what they want to do (edit, add, or delete)
  printf("Do you want to (1) edit, (2) add, or (3) delete a record: ");
  fgets(input, SIZE, stdin);
  input[strcspn(input, "\n")] = '\0';

  // check for needed information from user based on input
  if (strcmp(input,"1") == 0) {
      printf("Enter the Student ID:");
      fgets(stuId, SIZE, stdin);
      // ask for student information
      printf("Enter the Name: ");   
      fgets(name, SIZE, stdin);
      printf("Enter the Address: ");
      fgets(address, SIZE, stdin);
      printf("Enter the Telephone number: ");
      fgets(telNumber, SIZE, stdin);
  }
  else if (strcmp(input, "2") == 0) {
      // ask for student information
      printf("Enter the Name: ");
      fgets(name, SIZE, stdin);
      printf("Enter the Student ID: ");
      fgets(stuId, SIZE, stdin);
      printf("Enter the Address: ");
      fgets(address, SIZE, stdin);
      printf("Enter the Telephone number: ");
      fgets(telNumber, SIZE, stdin);
  }
  else if (strcmp(input, "3") == 0) {
      // ask for student ID
      printf("Enter the Student ID: ");
      fgets(stuId, SIZE, stdin);
  }
  // check if input was invalid
  else {
      printf("Invalid Input\n");
      exit(1);
  }
/* get the id of the semaphore set associated with SEMA_KEY, created by the
"create" process */ 
  sema_set=semget(SEMA_KEY, 0,0);
  /* Load new data (obtained as commandline arguments)
     into the shared memory segment */

  // writer wait
  Wait(sema_set,1);
  //printf("the value of sema_set=%d\n", sema_set); 

  // use loop to search for ID in data
  // once found, ask for new data to overwrite;
  if (strcmp(input, "1") == 0){
    // edit data with given student ID
    while (strlen(infoptr[count].name) > 1 && count <= 50) {
        char str[12];
        strcpy(str,infoptr[count].stuId);
        str[strcspn(str, "\n")] = '\0';
        stuId[strcspn(stuId, "\n")] = '\0';
        if (strcmp(infoptr[count].stuId, stuId) == 13) {
            strcpy(infoptr[count].name, name);
            strcpy(infoptr[count].stuId, stuId);
            strcpy(infoptr[count].address, address);
            strcpy(infoptr[count].telNumber, telNumber);
            printf("Data overwritten\n");
        }
        count += 1;
    }
  }
  else if (strcmp(input, "2") == 0){
    // add data as new node
    while (strlen(infoptr[count].name) > 1 && count <= 50) {
        count += 1;
    }
    found = count;
    strcpy(infoptr[found].name, name);
    strcpy(infoptr[found].stuId, stuId);
    strcpy(infoptr[found].address, address);
    strcpy(infoptr[found].telNumber, telNumber);
  }
  else if (strcmp(input, "3") == 0){
    //delete associated record
    while (strlen(infoptr[count].name) > 1 && count <= 50) {
        char str[12];
        strcpy(str, infoptr[count].stuId);
        str[strcspn(str, "\n")] = '\0';
        stuId[strcspn(stuId, "\n")] = '\0';
        if (strcmp(str, stuId) == 13) {
            printf("Name: %s Student ID: %s Address: %s Phone Number: %s\n",
                infoptr[count].name, infoptr[count].stuId, infoptr[count].address, infoptr[count].telNumber);
            found = count;
        }
        count += 1;
    }
    // check if matching entry was found
    if (found != -1) {
        while (found < count) {
            strcpy(infoptr[found].name, infoptr[found + 1].name);
            strcpy(infoptr[found].stuId, infoptr[found + 1].stuId);
            strcpy(infoptr[found].address, infoptr[found + 1].address);
            strcpy(infoptr[found].telNumber, infoptr[found + 1].telNumber);
            found += 1;
        }
    }
    else {
        printf("Record not found\n");
    }
  }
  else{
    printf("Invalid Input");
    exit(1);
  }

  //sleep before signalling for testing
  sleep(2);

  // writer signal and exit
  Signal(sema_set,1); 
  exit(0);

}
