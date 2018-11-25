#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char ** argv){
  	int len;
        if (argc<2){
		fprintf(stderr,"Usage: %s string\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	len=strlen(argv[1])+1;
	int fd = shm_open("mem", O_RDWR | O_CREAT, 0600);
	if (fd == -1){
		perror("shm_open");
		exit(EXIT_FAILURE);
	} 
	ftruncate(fd, len);
	char* mem = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);	
	if (mem == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(EXIT_FAILURE);
	} 
	strcpy(mem,argv[1]);
	if (munmap(mem, len) == -1) {
		perror("unmmap");
		close(fd);
		exit(EXIT_FAILURE);
	} 
}
