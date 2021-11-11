
/* header.h */
#define KEY  ((key_t)(41777)) /*change it to last five digits of your SID*/
#define KEY2 ((key_t)(141777))
#define SEGSIZE  sizeof(struct StudentInfo)*50

#define NUM_SEMAPHS 2
#define SEMA_KEY   ((key_t)(1777)) /* change this to last four digits of SID */

struct StudentInfo {
	char name[40];
	char stuId[12];
	char address[50];
	char telNumber[15];
};

void Wait(int semaph, int n);
void Signal(int semaph, int n);
int GetSemaphs(key_t k, int n);
