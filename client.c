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
	1. Open files, set semaphore to 0
	2. Open shared memory segment, get line size
	3. Use lseek on story.txt, read the line into a char* which has the size from the shm segment. Print it 
	4. Read line from input into a char*.
	5. Set story.txt to truncate already, so write the new line to story.txt. Append the new line to totalStory.txt
	6. Close files, set semaphore to 1, detach from shared memory
	*/

	////////////////1////////////////
	int storyFD = open("story.txt", O_RDWR | O_APPEND);

	int semd = semget(ftok("story.txt", 100), 1, 0664);
	if(semd == -1) printf("Semaphore Access Error: %s\n", strerror(errno));

	union semun valSetter;
	valSetter.val = 0;
	int res = semctl(semd, 0, SETVAL, valSetter);
	if(res == -1) printf("Semaphore Value Setting Error: %s\n", strerror(errno));

	////////////////2////////////////
	int shmd = shmget(ftok("story.txt", 100), 4, 0664);
	if(shmd == -1) printf("Shared Memory Access Error: %s\n", strerror(errno));

	int *p;
	p = (int*) shmat(shmd, 0, 0);
	if((int) p == -1) printf("Attaching to Shared Memory Error: %s\n", strerror(errno));

	int lineSize = *p;

	////////////////3////////////////
	int currentPos = lseek(storyFD, -1 * lineSize, SEEK_END);
	if(currentPos == -1) printf("Error setting position in story.txt: %s\n", strerror(errno));

	char* lineBuffer = (char*) calloc(1, lineSize + 1);
	int readRes = read(storyFD, lineBuffer, lineSize); if(readRes == -1) printf("Reading Error: %s\n", strerror(errno));
	*(lineBuffer + lineSize) = 0;
	if(strlen(lineBuffer)) printf("Previously Added Line: %s", lineBuffer);
	else printf("Story Being Created\n");
	//printf("Previously Added Line Size: %d\n", lineSize);
	free(lineBuffer);
	currentPos = lseek(storyFD, 0, SEEK_END); if(currentPos == -1) printf("Error setting position in story.txt: %s\n", strerror(errno));

	////////////////4////////////////
	char* nextLine = (char*) calloc(1, 52);
	printf("Enter next line of story (at most 49 characters) > ");
	fgets(nextLine, 51, stdin);
	//printf("adding the line: %s", nextLine);

	////////////////5////////////////
	int wrResult = write(storyFD, nextLine, strlen(nextLine)); if(wrResult == -1) printf("Writing to story.txt error: %s\n", strerror(errno));
	*p = strlen(nextLine);
	free(nextLine);

	////////////////6////////////////
	close(storyFD);
	shmdt(p);

	//set value of semaphore
	valSetter.val = 1;
	res = semctl(semd, 0, SETVAL, valSetter);
	if(res == -1) printf("Semaphore Value Setting Error: %s\n", strerror(errno));

	return 0;

}