#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define PATH_MAX 4096
#define FILE_SIZE 4096

int main(int argc, char** argv) {
//////////////////////////////////////////////////////////////////////parametrs
    int port;
    char* init_path;
///////////////////////////////////////////////////////////////////////////////
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
            init_path = malloc(strlen(argv[2]));
            if (init_path == NULL) {
                perror("malloc");
                exit(1);
            }
            init_path[0] = '\0';
            strcpy(init_path, argv[2]);
            if (chdir(init_path)) {
                perror("chdir");
                exit(1);
            }
        } else {
            init_path = malloc(PATH_MAX);
            if (init_path == NULL) {
                perror("malloc");
                exit(1);
            }
            if (getcwd(init_path, PATH_MAX) == NULL) {
                perror("getcwd");
                exit(1);
            }
        }
    }
    printf("\nSERVER\n port: %d\n current path: %s\n", port, init_path);
///////////////////////////////////////////////////////////////////////////////   
    int servsocket;
    struct sockaddr_in servaddr;
///////////////////////////////////////////////////////////////////////////////    
    if ((servsocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket");
        exit(1);
    }
    printf(" ...\n Socket created, ");

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if(bind(servsocket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("bind");
        exit(1);
    }
    printf("bind done.\n");

    if(listen(servsocket, 1)<0){
        perror("listen");
        exit(1);
    }
    printf(" Waiting for connections...\n");
///////////////////////////////////////////////////////////////////////////////
    int newsocket;
    char* buffer = malloc(FILE_SIZE);
    char* filename = malloc(FILENAME_MAX);
    char* path_name = malloc(PATH_MAX);
    char* curr_path = malloc(PATH_MAX);
    if (buffer == NULL || filename == NULL || path_name == NULL || curr_path == NULL) {
        perror("malloc");
        exit(1);
    }
///////////////////////////////////////////////////////////////////////////////
    while(1) {
        if ((newsocket = accept(servsocket, NULL, NULL)) < 0) {
            perror("accept");
            exit(1);
        }
        printf("\nConnection accepted.\n");

        bzero(buffer, FILE_SIZE);
        bzero(filename, FILENAME_MAX);
        bzero(path_name, PATH_MAX);

        int read_sz = recv(newsocket, buffer, FILE_SIZE, 0);


            if (!(strcmp(buffer, "quit"))) {
                free(buffer);
                free(filename);
                free(path_name);

                free(curr_path);

                shutdown(newsocket, SHUT_RDWR);
                exit(0);
            }

            if (!(strcmp(buffer, "put"))) {
                recv(newsocket, filename, FILE_SIZE, 0);
                printf("trying to get %s\n", filename);

                fflush(stdout); 

                int size;
                recv(newsocket, &size, sizeof(int), 0);

                char* recieved_file = malloc(size);

                recv(newsocket, recieved_file, size, 0);
                int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                write(fd, recieved_file, size);
                close(fd);
                printf("got %s\n", filename);
                fflush(stdout);
            }

            if (!(strcmp(buffer, "get"))) {
                printf("trying to send %s\n", filename);
                recv(newsocket, filename, FILE_SIZE, 0);

                int fd = open(filename, O_RDONLY, 0666);
                struct stat st;
                stat(filename, &st);
                send(newsocket, &(st.st_size), sizeof(int), 0);
                sendfile(newsocket, fd, NULL, st.st_size);
                close(fd);

                printf("sent %s", filename);
            }

            if (!(strcmp(buffer, "ls"))) {

                printf("sending ls\n");
                
                FILE *ls = popen("ls -l", "r");
                char line[MAX_SIZE];
                int num_of_lines = 0;
                while(fgets(line, FILE_SIZE, ls)) {
                    ++num_of_lines;
                }
                send(newsocket, &num_of_lines, sizeof(int), 0);
                pclose(ls);
                ls = popen("ls -l", "r");
                while(fgets(line, FILE_SIZE, ls)) {
                    send(newsocket, &line, sizeof(line), 0);
                }
                printf("ls sent\n");
            }
            if (!(strcmp(buffer, "cd"))) {

                printf("starting cd\n");
                recv(newsocket, path_name, FILE_SIZE, 0);
                printf("trying to cd to %s\n", path_name);
                if (!strcmp(path_name, "..") && !strcmp(curr_path, init_path)) {
                    continue;
                }
                chdir(path_name);
                getcwd(curr_path, MAX_SIZE);
            }

        //close(newsocket);
    }
///////////////////////////////////////////////////////////////////////////////    
    shutdown(servsocket, 2);
///////////////////////////////////////////////////////////////////////////////
    free(init_path);
    return 0;
}