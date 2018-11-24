#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>

#define MAX_SIZE 4096

int main(int argc, char** argv) {
  int port;
  char init_path[MAX_SIZE];
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
    /*init port*/           
    port = atoi(argv[1]); 
    /*init path*/                      
    if (argc >= 3) {
      init_path[0] = '\0';
      strcpy(init_path, argv[2]);
      if (chdir(init_path)) {
         perror("chdir");
         exit(1);
      }
    } else {
      if (getcwd(init_path, MAX_SIZE) == NULL) {
        perror("getcwd");
        exit(1);
      }
    }
  }
  char path[MAX_SIZE];
  strcpy(path, init_path);
  printf("\nSERVER\n port: %d\n current path: %s\n", port, init_path);

  struct sockaddr_in server, client;
  struct stat obj;
  int servsock, clientsock;
  char buf[MAX_SIZE], command[5], filename[20];
  int i, size, len, c;
  int filehandle;
  servsock = socket(AF_INET, SOCK_STREAM, 0);
  if(servsock == -1) {
      printf("socket creation failed");
      exit(1);
  }
  server.sin_port = port;
  server.sin_addr.s_addr = 0;

  if(bind(servsock,(struct sockaddr*)&server, sizeof(server)) == -1) {
      perror("binding failed");
      exit(1);
  }

  if(listen(servsock, 1) == -1) {
      perror("listen failed");
      exit(1);
  }
  printf(" waiting for connections...\n");

  len = sizeof(client);
  clientsock = accept(servsock,(struct sockaddr*)&client, &len);

  printf(" connection accepted\n");
  
  i = 1;
  while(1)
    {
      recv(clientsock, buf, MAX_SIZE, 0); 
      sscanf(buf, "%s", command);

      if(strcmp(command, "ls") == 0) {
        i = 0;
        FILE *ls = popen("ls -l", "r");
        char line[MAX_SIZE];
        int num_of_lines = 0;
        while(fgets(line, MAX_SIZE, ls)) {
          ++num_of_lines;
        }
        send(clientsock, &num_of_lines, sizeof(int), 0);
        pclose(ls);
        ls = popen("ls -l", "r");
        while(fgets(line, MAX_SIZE, ls)) {
          send(clientsock, &line, sizeof(line), 0);
        }
        pclose(ls);
      }
      else if(!strcmp(command,"get")) {
          sscanf(buf, "%s%s", filename, filename);
          stat(filename, &obj);
          filehandle = open(filename, O_RDONLY);
          size = obj.st_size;
          if(filehandle == -1) {
              size = 0;
          }
          send(clientsock, &size, sizeof(int), 0);
          if(size) {
            sendfile(clientsock, filehandle, NULL, size);
          }
    }
      else if(!strcmp(command, "put")) {
        int c = 0, len;
        char *f;
        sscanf(buf + strlen(command), "%s", filename);
        recv(clientsock, &size, sizeof(int), 0);
        i = 1;
        while(1) {
          filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
          if(filehandle == -1) {
            sprintf(filename + strlen(filename), "%d", i);
          }
          else break;
        }
        f = malloc(size);
        recv(clientsock, f, size, 0);
        c = write(filehandle, f, size);
        close(filehandle);

        send(clientsock, &c, sizeof(int), 0);
      }
      else if(!strcmp(command, "cd")) {
        if(strcmp(buf + 3, "..") && strcmp(path, init_path) && chdir(buf + 3) == 0) {
          getcwd(path, MAX_SIZE);
          c = 1;
          printf(" Path changed to %s\n", path);
        }
        else
          c = 0;
        send(clientsock, &c, sizeof(int), 0);
      } 
      else if(!strcmp(command, "quit")) {
        printf("FTP server quitting..\n");
        i = 1;
        send(clientsock, &i, sizeof(int), 0);
        shutdown(clientsock, SHUT_RDWR);
        shutdown(servsock, SHUT_RDWR);
        exit(0);
      }
    }
  return 0;
}