#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>

int main () {

	/*
	1. Open files, set semaphores to 0
	2. Open shared memory segment, get line size
	3. Open story.txt, read the line into a char* which has the size from the shm segment. Print it 
	4. Read line from input into a char*.
	5. Set story.txt to truncate already, so write the new line to story.txt. Append the new line to totalStory.txt
	6. Close shared memory segment
	7. Close files, set semaphores to 1
	*/

	////1////
	int storyFD = open("story.txt", O_TRUNC | O_RDONLY);
	int totalFD = open("totalStory.txt", O_APPEND | O_RDONLY);

	int semd = semget(ftok("story.txt", 100), 1, IPC_CREAT | 0644);
	union semun valSetter;
	valSetter.val = 0;
	int res = semctl(semd, 0, SETVAL, valSetter);
	if(res == -1) printf("Semaphore Value Setting Error: %s\n", strerror(errno));

	////2////
	int shmd = shmget(ftok("story.txt", 100), 4, IPC_CREAT | 0644);
	int* p = 0;
	shmat(shmd, &p, 0644);
	p = (int *) p;

	int lineSize = *p;
	

}