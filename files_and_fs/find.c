#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>

void FindFile(char* path, char* object_name, char* user_name, unsigned int size) {

  DIR* directory = opendir(path);
  if (directory == NULL) {
    perror(NULL);
  } else {

    struct dirent* data_of_file;
    struct stat buffer;

    while ((data_of_file = readdir(directory)) != NULL) {
      //make new path
      char* new_path = malloc(2 + strlen(path) + strlen(data_of_file->d_name));
      if (new_path == NULL) {
        perror(NULL);
        exit(-1);
      }
      new_path[0] = '\0';
      strcat(strcat(strcat(new_path, path), "/"), data_of_file->d_name);

      lstat(new_path, &buffer);
      struct passwd* password = getpwuid(buffer.st_uid);
      if (password == NULL) {
      	perror(NULL);
      }
      if (!strcmp(object_name, data_of_file->d_name) && 
      	!strcmp(user_name, password->pw_name) && 
      	size >= buffer.st_size) {
          printf("%s %u\n", new_path, (unsigned int)buffer.st_size);
      }	
      if (S_ISDIR(buffer.st_mode) && strcmp(data_of_file->d_name, ".") &&
      	strcmp(data_of_file->d_name, "..")) {
        FindFile(new_path, object_name, user_name, size);
      }

      free(new_path);
    }
    free(data_of_file);

    closedir(directory);
  }
}

int main(int argc, char** argv) {
  if (argc == 5) {
	  unsigned int num = strlen(argv[4]);
	  unsigned int size = 0;
	  unsigned int power10 = 1;
	  for (int i = num - 1; i >= 0; --i) {
	  	size += (argv[4][i] - '0') * power10;
	  	power10 *= 10;
	  }
	  FindFile(argv[1], argv[2], argv[3], size);
  }
  return 0;
}
