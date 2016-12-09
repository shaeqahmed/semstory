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
	3. Open story.txt, read the line into a char* which has the size from the shm segment. Print it 
	4. Read line from input into a char*.
	5. Set story.txt to truncate already, so write the new line to story.txt. Append the new line to totalStory.txt
	6. Close files, set semaphore to 1, detach from shared memory
	*/

	////////////////1////////////////
	int storyFD = open("story.txt", O_TRUNC | O_RDONLY);
	int totalFD = open("totalStory.txt", O_APPEND | O_RDONLY);

	int semd = semget(ftok("story.txt", 100), 1, 0664);
	if(semd == -1) printf("Semaphore Access Error: %s\n", strerror(errno));
	union semun valSetter;
	valSetter.val = 0;
	int res = semctl(semd, 0, SETVAL, valSetter);
	if(res == -1) printf("Semaphore Value Setting Error: %s\n", strerror(errno));

	////////////////2////////////////
	int shmd = shmget(ftok("story.txt", 100), 4, 0664);
	if(shmd == -1) printf("Shared Memory Access Error: %s\n", strerror(errno));
	void* p = NULL;
	void* shmatResult = shmat(shmd, &p, 0664);
	if((long) shmatResult == -1) printf("Attaching to Shared Memory Error: %s\n", strerror(errno));
	int* loc = (int *) p;

	int lineSize = *loc;
	
	////////////////3////////////////
	char* lineBuffer = (char*) calloc(1, lineSize + 1);
	read(storyFD, lineBuffer, lineSize);
	*(lineBuffer + lineSize) = 0; //lineBuffer[lineSize] = NULL;
	printf("Previously Added Line: %s\n", lineBuffer);
	free(lineBuffer);

	////////////////4////////////////
	char* nextLine = (char*) calloc(1, 50);
	printf("Enter next line of story (at most 49 characters) > ");
	fgets(nextLine, 50, stdin);

	////////////////5////////////////
	write(storyFD, nextLine, strlen(nextLine));
	write(totalFD, nextLine, strlen(nextLine));
	//append nextLine to totalStory.txt and story.txt (including the new line)
	free(nextLine);

	////////////////6////////////////
	close(storyFD);
	close(totalFD);
	shmdt(p);
	//set value of semaphore
	valSetter.val = 1;
	res = semctl(semd, 0, SETVAL, valSetter);
	if(res == -1) printf("Semaphore Value Setting Error: %s\n", strerror(errno));


	return 0;

}