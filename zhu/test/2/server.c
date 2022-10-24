#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <openssl/rc4.h>
#include "common.h"
#include "debug.h"

int main(int argc, const char *argv[])
{
    int sockfd, acceptfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed.");
        exit(-1);
    }
    printf("socket ok.\n");

    struct sockaddr_in serveraddr, clientaddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi("8887"));
    serveraddr.sin_addr.s_addr = inet_addr("10.0.0.87");
    socklen_t serverlen = sizeof(serveraddr);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt,
               sizeof(opt));
    if (bind(sockfd, (struct sockaddr *)&serveraddr, serverlen) < 0) {
        perror("bind failed.");
        exit(-1);
    }
    printf("bind ok.\n");

    if (listen(sockfd, 5) < 0) {
        perror("listen failed.");
        exit(-1);
    }
    printf("listen ok.\n");

    bzero(&clientaddr, sizeof(clientaddr));
    socklen_t clientlen = sizeof(clientaddr);
    if ((acceptfd =
             accept(sockfd, (struct sockaddr *)&clientaddr, &clientlen)) < 0) {
        perror("accept error.");
        exit(-1);
    }
    printf("accept client ok.\n");

    close(sockfd);

    chans ctx;
    chans_add_channel(&ctx, acceptfd, CHANS_MASTER);
    chans_process(&ctx);

    return 0;
}
