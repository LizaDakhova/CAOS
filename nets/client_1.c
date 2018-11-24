/*FTP Client*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/*for getting file size using stat()*/
#include<sys/stat.h>
/*for sendfile()*/
#include<sys/sendfile.h>
/*for O_RDONLY*/
#include<fcntl.h>
#define MAX_SIZE 4096
int main(int argc,char *argv[])
{
  struct sockaddr_in server;
  struct stat obj;
  int sock;
  char buffer[MAX_SIZE];
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
  server.sin_port = atoi(argv[1]);
  server.sin_addr.s_addr = 0;
  k = connect(sock,(struct sockaddr*)&server, sizeof(server));
  if(k == -1)
    {
      printf("Connect Error");
      exit(1);
    }
  int i = 1;

  while(1)
    {
      printf("get\n put\n pwd\n ls\n cd\n quit\n");
      scanf("%s", buffer);

	if (strncmp(buffer, "get", 3) == 0) {
	  printf("Enter filename to get: ");
	  scanf("%s", filename);
	  strcpy(buf, "get ");
	  strcat(buf, filename);
	  send(sock, buf, MAX_SIZE, 0);
	  recv(sock, &size, sizeof(int), 0);
	  if(!size)
	    {
	      printf("No such file on the remote directory\n\n");
	    break;
	    }
	  f = malloc(size);
	  recv(sock, f, size, 0);
	  while(1)
	    {
	      filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
	      if(filehandle == -1)
		{
		  sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
		}
	      else break;
	    }
	  write(filehandle, f, size, 0);
	  close(filehandle);
	  strcpy(buf, "cat ");
	  strcat(buf, filename);
	  system(buf);
	}
	if (strncmp(buffer, "put", 3) == 0) {
	  printf("Enter filename to put to server: ");
          scanf("%s", filename);
	  filehandle = open(filename, O_RDONLY);
          if(filehandle == -1)
            {
              printf("No such file on the local directory\n\n");
              break;
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
	    printf("File stored successfully\n");
	  else
	    printf("File failed to be stored to remote machine\n");
	}
	if (strncmp(buffer, "pwd", 3) == 0) {
	  strcpy(buf, "pwd");
	  send(sock, buf, MAX_SIZE, 0);
	  recv(sock, buf, MAX_SIZE, 0);
	  printf("The path of the remote directory is: %s\n", buf);
	}
	if (strncmp(buffer, "ls", 2) == 0) {
	  strcpy(buf, "ls");
      send(sock, buf, MAX_SIZE, 0);
	  recv(sock, &size, sizeof(int), 0);
      f = malloc(size);
      recv(sock, f, size, 0);
	  filehandle = creat("temp.txt", O_WRONLY);
	  write(filehandle, f, size, 0);
	  close(filehandle);
      printf("The remote directory listing is as follows:\n");
	  system("cat temp.txt");
	}
	if (strncmp(buffer, "cd", 2) == 0) {
	  strcpy(buf, "cd ");
	  printf("Enter the path to change the remote directory: ");
	  scanf("%s", buf + 3);
          send(sock, buf, MAX_SIZE, 0);
	  recv(sock, &status, sizeof(int), 0);
          if(status)
            printf("Remote directory successfully changed\n");
          else
            printf("Remote directory failed to change\n");
    }
	if (strncmp(buffer, "quit", 4) == 0) {
	  strcpy(buf, "quit");
          send(sock, buf, MAX_SIZE, 0);
          recv(sock, &status, MAX_SIZE, 0);
	  if(status)
	    {
	      printf("Server closed\nQuitting..\n");
	      exit(0);
	    }
	    printf("Server failed to close connection\n");
	}
  }
}