#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void pipes(int num, char** processes) {

	if (num < 1) {
    	fprintf(stderr, "Usage: %s <string>\n", processes[0]);
    	exit(1);
    }

	int fd[2]; // file descriptors

	for (int i = 1; i < num - 1; ++i) {
		pipe(fd);
		//fd[0] is set up for reading, fd[1] is set up for writing

		pid_t pid = fork();

		if (pid < 0) { //creation of a child process was unsuccessful
			perror("fork");
			exit(1);
		} else if (pid == 0) { //returned to the newly created child process
			close(fd[1]);
			int res = dup2(fd[0], 0);
			if (res == -1) {
				perror("dup2");
				exit(1);
			}
		} else { //returned to parent or caller, the value contains process ID of newly created child process.
			close(fd[0]);
			int res = dup2(fd[1], 1);
			if (res == -1) {
				perror("dup2");
				exit(1);
			}
			execlp(processes[i], processes[i], NULL);
		}
	}
	execlp(processes[num - 1], processes[num - 1], NULL); 

	for (int i = 1; i < num; ++i) {
		wait(NULL);
	}

	close(1);
	close(0);
}

int main(int argc, char** argv) {
	pipes(argc, argv);
	return 0;
}