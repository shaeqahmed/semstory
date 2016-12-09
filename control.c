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

int main (int argc, char *argv[]) 
{
	//argc is assumed to be 1
	//argv[0] assumed to be 'c' or 'r'

	if(strcmp(argv[1], "c") == 0)
	{
		//create shared memory segment
		int shmd = shmget(ftok("story.txt", 100), 4, IPC_CREAT | 0664);
		if(shmd == -1) printf("Shared Memory Error: %s\n", strerror(errno));

		//create semaphore
		int semd = semget(ftok("story.txt", 100), 1, IPC_CREAT | 0664);
		if(semd == -1) printf("Semaphore Creation Error: %s\n", strerror(errno));

		union semun valSetter;
		valSetter.val = 1;
		int res = semctl(semd, 0, SETVAL, valSetter);
		if(res == -1) printf("Semaphore Value Setting Error: %s\n", strerror(errno));

		//double check value
		int newVal = semctl(semd, 0, GETVAL);
		printf("value of semaphore set to %d\n", newVal);
		if(newVal == -1) printf("Semaphore Value Getting Error: %s\n", strerror(errno));

		////////////make the text files////////////
		int totalFd = open("totalStory.txt", O_CREAT, 0664); //contains the entire story so far
		int fd = open("story.txt", O_CREAT, 0664); //contains only the last line

		printf("made files\n");

		close(totalFd);
		close(fd);
	}
	else //argv[0] is 'r'
	{

	}


}