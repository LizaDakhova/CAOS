#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>

#define MAX_FILES 256

int min(int a, int b) {
	return a < b ? a : b;
}

void read_files(char* file, char*** files, int *number_of_files) {
FILE *f;
  f = fopen(file, "r");
  if(f == NULL) {
      perror(NULL);
      exit(1);
  }

  *files = (char**)malloc(sizeof(char*));
  if (!(*files)) {
  	  perror("malloc");
	  exit(1);
  }

  int c, number_of_files_ = 0, index_j = 0, new_word = 1;
  while((c = fgetc(f)) != EOF) {
  	if (!isspace(c) && !new_word) {
        (*files)[number_of_files_ - 1][index_j++] = c;
        (*files)[number_of_files_ - 1][index_j] = '\0';
    } else if (!isspace(c) && new_word) {
        *files = realloc(*files, sizeof(char*) * (number_of_files_ + 1));
         if(!(*files)) {
            perror(NULL);
            exit(1);
      	}

        (*files)[number_of_files_] = (char*)malloc(256);
        if (!(*files)[number_of_files_]) {
        	perror("malloc");
	    	exit(1);
        }
        
        index_j = 0;
        new_word = 0;
        (*files)[number_of_files_++][index_j++] = c;
    } else if(isspace(c)){
        new_word = 1;
    }
  }

  *number_of_files = number_of_files_;
  fclose(f);
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
	int number_of_files;

	read_files(argv[1], &files, &number_of_files);

    long long int strings = 0, words = 0, characters = 0;

    for (int k = 0; k < number_of_files; k += MAX_FILES) {
    	int size = min(number_of_files - k, MAX_FILES);
    	char** temp_files = (char**)malloc(sizeof(char*) * size);
    	int* fd = (int*)malloc(sizeof(int) * size);

    	for (int i = 0; i < size; ++i) {
	        if (i >= number_of_sons) {
	           wait(NULL);
	        }
		    temp_files[i] = malloc(7);
		    strcpy(temp_files[i], "XXXXXX");
		    fd[i] = mkstemp(temp_files[i]);
		    exec_wc(files[k + i], i, fd[i]);
	    }
	    for(int i = 0; i < number_of_sons; ++i) {
	        wait(NULL);
	    }
		for (int i = 0; i < size; ++i) {
		    long long int s_f = 0, w_f = 0, c_f = 0;
		    lseek(fd[i], 0, SEEK_SET);

		    FILE *f = fdopen(fd[i], "r");
		    if(!f) {
		        perror(NULL);
		        exit(1);
		    }

		    fscanf(f, "%lld %lld %lld", &s_f, &w_f, &c_f);
		    //printf("%lld %lld %lld\n", s_f, w_f, c_f);
		    strings += s_f;
		    words += w_f;
		    characters += c_f;

		    fclose(f);
		    unlink(temp_files[i]);
		}

		free(fd);
		for (int i = 0; i < size; ++i) {
			free(temp_files[i]);
		}
		free(temp_files);
    }

    printf("%lld %lld %lld total\n", strings, words, characters);

	for (int i = 0; i < number_of_files; ++i) {
		free(files[i]);
	}
	free(files);
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