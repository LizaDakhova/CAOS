#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#define MAX_SIZE 4096

int main(int argc,char **argv) {
  int port;
  printf("%s\n", "hello");
  if (argc < 1) {
    perror("argc");
    exit(1);
  }
  if (argc == 1) {
    printf("Error: need agrs \n");
    exit(1);
  }
  if (argc >= 2) {          
    port = atoi(argv[1]); 
  }
  char init_path[MAX_SIZE];
  char path[MAX_SIZE];
  getcwd(init_path, MAX_SIZE);
  strcpy(path, init_path);

  struct sockaddr_in server;
  struct stat obj;
  int sock;
  int choice;
  char buf[MAX_SIZE], command[5], filename[20], *f;
  int k, size, status;
  int filehandle;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1)
    {
      printf("socket creation failed");
      exit(1);
    }
  server.sin_family = AF_INET;
  server.sin_port = port;
  server.sin_addr.s_addr = 0;
  k = connect(sock,(struct sockaddr*)&server, sizeof(server));
  if(k == -1)
    {
      printf("Connect Error");
      exit(1);
    }
  int i = 1;

  while(1) {
      
    printf("Enter a choice:\n get filename\n put filename\n ls\n !ls\n cd newpath\n !cd newpath\n quit\n");
    scanf("%s", command);

    if (strncmp(command, "get", 3) == 0) {
      scanf("%s", filename);
      strcpy(buf, "get ");
      strcat(buf, filename);
      send(sock, buf, MAX_SIZE, 0);
      recv(sock, &size, sizeof(int), 0);
      if(size == 0) {
          printf("No such file on the remote directory\n\n");
        continue;
      }
      f = malloc(size);
      recv(sock, f, size, 0);
      while(1) {
        filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
        if(filehandle == -1) {
          sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
        }
        else continue;
      }
      write(filehandle, f, size, 0);
      close(filehandle);
      printf("File got and stored successfully\n");
    }

    if (strncmp(command, "put", 3) == 0) {
      scanf("%s", filename);
      filehandle = open(filename, O_RDONLY);
      if(filehandle == -1) {
        printf("No such file on the local directory\n\n");
        continue;
      }
      strcpy(buf, "put ");
      strcat(buf, filename);
      send(sock, buf, MAX_SIZE, 0);
      stat(filename, &obj);
      size = obj.st_size;
      send(sock, &size, sizeof(int), 0);
      sendfile(sock, filehandle, NULL, size);

      recv(sock, &status, sizeof(int), 0);
      if(status)
        printf("File put and stored successfully\n");
      else
        printf("File failed to be stored to remote machine\n");
    }

    if (strncmp(command, "ls", 2) == 0) {
      strcpy(buf, "ls");
      send(sock, buf, MAX_SIZE, 0);
      int num_of_lines;
      char line[MAX_SIZE];
      recv(sock, &num_of_lines, sizeof(int), 0);
      printf("The remote directory listing is as follows:\n");
      for (int i = 0; i < num_of_lines; ++i) {
        recv(sock, &line, MAX_SIZE, 0);
        puts(line);
      }
    }

    if (strncmp(command, "!ls", 3) == 0) {
      printf("The local directory listing is as follows:\n");
      FILE *ls = popen("ls -l", "r");
      char line[MAX_SIZE];
      while(fgets(line, MAX_SIZE, ls)) {
        puts(line);
      }
    }

    if (strncmp(command, "cd", 2) == 0) {
      strcpy(buf, "cd ");
      scanf("%s", buf + 3);
          send(sock, buf, MAX_SIZE, 0);
      recv(sock, &status, sizeof(int), 0);
          if(status)
            printf("Remote directory successfully changed\n");
          else
            printf("Remote directory failed to change\n");
    }

    if (strncmp(command, "!cd", 3) == 0) {
      scanf("%s", buf + 3);
      if (!strcmp(buf + 3, "..") || !strcmp(path, init_path)) {
        printf("Local directory failed to change\n");
        continue;
      }
      chdir(buf + 3);
      char path[MAX_SIZE];
      getcwd(path, MAX_SIZE);
      printf("Local directory successfully changed to %s\n", path);
    }

    if (strncmp(command, "quit", 4) == 0) {
      strcpy(buf, "quit");
          send(sock, buf, MAX_SIZE, 0);
          recv(sock, &status, MAX_SIZE, 0);
      if(status) {
          printf("Server closed\nQuitting..\n");
          shutdown(sock, SHUT_RDWR);
          exit(0);
        }
        printf("Server failed to close connection\n");
    }
  }
}