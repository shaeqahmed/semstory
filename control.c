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

#define MAXSTORYSIZE 3000

int main (int argc, char *argv[]) 
{
	//argc is assumed to be 1
	//argv[0] assumed to be 'c' or 'r'
	if(strcmp(argv[1], "-c") == 0)
	{
		////////////make the text file////////////
		int fd = open("story.txt", O_CREAT, 0664); //contains only the last line
		//printf("made file\n");
		close(fd);

		//create shared memory segment
		int shmd = shmget(ftok("story.txt", 100), 4, IPC_CREAT | 0664);
		if(shmd == -1) printf("Shared Memory Error: %s\n", strerror(errno));

		//create semaphore
		int semd = semget(ftok("story.txt", 100), 1, IPC_CREAT | 0664);
		if(semd == -1) printf("Semaphore Creation Error: %s\n", strerror(errno));

		struct sembuf sb;
		sb.sem_num = 0;
		sb.sem_flg = 0;
		sb.sem_op = 1;
		int upRes = semop(semd, &sb, 1);
		if(upRes == -1) printf("Failure in semd++: %s\n", strerror(errno));

		//double check value
		int val = semctl(semd, 0, GETVAL);
		printf("Value of semaphore set to %d\n", val);
		if(val == -1) printf("Error in setting semaphore: %s\n", strerror(errno));
	}
	else if(strcmp(argv[1], "-v") == 0)
	{
		//printf("0\n");
		char* storyBuffer = (char *) calloc(1, MAXSTORYSIZE);
		int fd = open("story.txt", O_RDONLY);
		if(fd == -1) printf("Opening Error: %s\n", strerror(errno));
		int readRes = read(fd, storyBuffer, MAXSTORYSIZE);
		if(readRes == -1) printf("Reading Error: %s\n", strerror(errno));
		int closeRes = close(fd);
		if(closeRes == -1) printf("Closing Error: %s\n", strerror(errno));
		//printf("length of story: %d\n", strlen(storyBuffer));
		printf("%s", storyBuffer);
		free(storyBuffer);
	}
	else if(strcmp(argv[1], "-r") == 0)
	{
		//remove shared mem and semctl
		int shmd = shmget(ftok("story.txt", 100), 4, 0664); if(shmd == -1) printf("Shared Memory Access Error: %s\n", strerror(errno));
		int semd = semget(ftok("story.txt", 100), 1, 0664); if(semd == -1) printf("Semaphore Access Error: %s\n", strerror(errno));
		if(shmctl(shmd, IPC_RMID, 0) == -1) printf("Shared Memory Removal Error: %s\n", strerror(errno));
    	if(semctl(semd, 0, IPC_RMID) == -1) printf("Semaphore Removal Error: %s\n", strerror(errno)); 

		char* storyBuffer = (char *) calloc(1, MAXSTORYSIZE);
		int fd = open("story.txt", O_RDONLY);
		if(fd == -1) printf("Opening Error: %s\n", strerror(errno));
		int readRes = read(fd, storyBuffer, MAXSTORYSIZE);
		if(readRes == -1) printf("Reading Error: %s\n", strerror(errno));
		int closeRes = close(fd);
		if(closeRes == -1) printf("Closing Error: %s\n", strerror(errno));
		//printf("length of story: %d\n", strlen(storyBuffer));
    	printf("%s", storyBuffer);
    	free(storyBuffer);
	}
}