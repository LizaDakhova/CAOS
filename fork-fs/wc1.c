#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void read_files(char* file, char*** files, int *size_of_files, int *number_of_files) {

}

void exec_wc(const char* file_name, int number_of_process, int fd) {
    pid_t pid = fork();
    if (pid < 0) {
    	perror("fork");
    	exit(1);
    }
  	if (pid == 0) {
	    int newfd = dup2(fd, 1);
	    if (newfd == -1) {
	    	perror("dup2");
	    	exit(1);
	    }
	    int err = execlp("/usr/bin/wc", "wc", file_name, NULL);
	    if (err) {
	    	perror("execlp");
	    	exit(1);
	    }
    }
}

void wc_forks(int argc, char** argv) {
	const int number_of_sons = atoi(argv[2]);
	char** files;
	int size_of_files, number_of_files;

	//read_files(argv[1], &files, &size_of_files, &number_of_files);

	FILE *f;
	f = fopen(argv[1], "r");
	if (!f) {
		perror("fopen");
		exit(1);
	}
	size_of_files = 200;
	files = (char**)malloc(sizeof(char*) * (size_of_files));
	if (!(files)){
		perror("malloc");
		exit(1);
	}
	int c, last_i = 0, last_j = 0, is_new_word = 1;
	char str[108];
	while ((c = fgetc(f)) != EOF) {
		if (isspace(c) && !is_new_word) {
			str[last_j] = '\0';
			(files)[last_i] = (char*)malloc(strlen(str));
			if (!(files)[last_i]) {
				perror("malloc");
				exit(1);
			}
			if (last_i >= (size_of_files) - 1) {
				//printf("%s\n", "kapec!");
				size_of_files *= 2;
				files = realloc(files, sizeof(char*) * (size_of_files));
				if (!(files)) {
					perror("realloc");
					exit(1);
				}
			}
			strcpy((files)[last_i++], str);
			last_j = 0;
			is_new_word = 1;
		} else if (!isspace(c)) {
			str[last_j++] = c;
			is_new_word = 0;
		}
	}
/*	for (int i = 0; i < *size_of_files; ++i) {
		printf("%s\n", *files[i]);
	}*/
	fclose(f);
	number_of_files = last_i;







	char** temp_files = (char**)malloc(sizeof(char*) * number_of_files);
    int fd[number_of_files];

    for (int i = 0; i < number_of_files; ++i) {
        if (i >= number_of_sons) {
           wait(NULL);
        }
	    temp_files[i] = malloc(7);
	    strcpy(temp_files[i], "XXXXXX");
	    fd[i] = mkstemp(temp_files[i]);
	    exec_wc(files[i], i, fd[i]);
    }
    for(int i = 0; i < number_of_sons; ++i) {
        wait(NULL);
    }

    long long int strings = 0, words = 0, characters = 0;
    for (int i = 0; i < number_of_files; ++i) {
        long long int s_f, w_f, c_f;
        lseek(fd[i], 0, SEEK_SET);

        FILE *f = fdopen(fd[i], "r");
        if(!f) {
            perror(NULL);
            exit(1);
        }

	    fscanf(f, "%lld %lld %lld", &s_f, &w_f, &c_f);
	    strings += s_f;
	    words += w_f;
	    characters += c_f;
	    fclose(f);
	    unlink(temp_files[i]);
  	}
  	printf("%lld %lld %lld\n", strings, words, characters);

	for (int i = 0; i < size_of_files; ++i) {
		free(files[i]);
	}
	free(files);

	for (int i = 0; i < number_of_files; ++i) {
		free(temp_files[i]);
	}
	free(temp_files);
}

int main(int argc, char** argv) {
	if (argc < 1) {
		perror("argc");
		exit(1);
	}
	if (argc < 3) {
		printf("Not enough arguments: need 2\n");
		exit(1);
	}
	wc_forks(argc, argv);
	return 0;
}