#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char * argv[]) {
    int d  = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    int c;
    ssize_t s;
    char * buffer = malloc(1024*sizeof(char));
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8080);
    if(bind(d, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
        perror("bind");
        exit(1);
    }
    if(listen(d, 10)<0){
        perror("listen");
        exit(1);
    }
    int size = 1-1;
    int k = 10;
    while(k>0){
        c = accept(d, NULL, NULL);
        while(1){
            recv(c, buffer, size, 0);
            while(strcmp(buffer, "quit\n")){
            }
        }
        close(c);
        k--;
    }
    free(buffer);
    return 0;
}
