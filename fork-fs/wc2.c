//
// Created by sikalovnikita on 26.04.18.
//
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
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>

void exec_wc(const char* file_name, int number_of_process, int fd) {
  pid_t id = fork();
  if (!id) {
    dup2(fd, 1);
    execlp("/usr/bin/wc", "wc", file_name, NULL);
    perror("wc");
    exit(1);
  }
}

int main(int argc, char **argv) {
  const int number_of_sons = atoi(argv[1]);
  FILE *f;
  f = fopen(argv[2], "r");
  if(f == NULL) {
    perror(NULL);
    exit(1);
  }

  char **files = (char**)malloc(sizeof(char*));
  int c = fgetc(f);
  long long int count_of_files = 0, j = 0, new_word = 1;
  while(c != EOF) {
    if (!isspace(c) && new_word) {
      files = realloc(files, sizeof(char*) * (count_of_files + 1));
      if(!files) {
        perror(NULL);
        exit(1);
      }

      files[count_of_files] = (char*)malloc(100);
      j = 0;
      files[count_of_files][j] = c;
      count_of_files++;
      new_word = 0;
      ++j;
    } else {
      if (!isspace(c) && !new_word) {
        files[count_of_files - 1][j] = c;
        files[count_of_files - 1][j + 1] = '\0';
        ++j;
      } else {
        if(isspace(c)){
          new_word = 1;
        }
      }
    }
    c = fgetc(f);
  }
  fclose(f);

  char** temp_files;
  temp_files = malloc(sizeof(char*) * count_of_files);
  int fd[count_of_files];

  for (int i = 0; i < count_of_files; i++) {
    if (i >= number_of_sons) {
      wait(NULL);
    }
    temp_files[i] = malloc(sizeof(char) * 7);
    strcpy(temp_files[i], "XXXXXX");
    fd[i] = mkstemp(temp_files[i]);
    exec_wc(files[i], i, fd[i]);
  }
  for(int i = 0; i < number_of_sons; ++i) {
    wait(NULL);
  }

  long long int all_strings = 0, all_words = 0, all_charesters = 0;
  for (int i = 0; i < count_of_files; ++i) {
    long long int a, b, c;
    lseek(fd[i], 0, SEEK_SET);
    FILE *f = fdopen(fd[i], "r");
    if(!f) {
      perror(NULL);
      exit(1);
    }

    fscanf(f, "%lld %lld %lld", &a, &b, &c);
    all_strings += a;
    all_words += b;
    all_charesters += c;
    fclose(f);
    unlink(temp_files[i]);
  }
  printf("%lld %lld %lld\n", all_strings, all_words, all_charesters);

  for (int i = 0; i < count_of_files; ++i) {
    free(files[i]);
  }
  free(files);
  return 0;
}