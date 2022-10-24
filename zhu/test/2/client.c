#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <termios.h>
#include <openssl/rc4.h>
#include "common.h"
#include "debug.h"

int Creat_Listenfd()
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed.");
        exit(-1);
    }
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi("2222"));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t serverlen = sizeof(serveraddr);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt,
               sizeof(opt));
    if (bind(sockfd, (struct sockaddr *)&serveraddr, serverlen) < 0) {
        perror("bind failed.");
        exit(-1);
    }
    if (listen(sockfd, 5) < 0) {
        perror("listen failed.");
        exit(-1);
    }
    printf("listen 2222 ok.\n");
    return sockfd;
}

int main(int argc, const char *argv[])
{
    int connectfd = Creat_Listenfd();

    chans ctx;
    chans_add_channel(&ctx, connectfd, CHANS_LISTEN);

    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed.");
        exit(-1);
    }
    printf("socket ok.\n");

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi("8887"));
    serveraddr.sin_addr.s_addr = inet_addr("10.0.0.87");
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <
        0) {
        perror("connect failed.");
        exit(-1);
    }
    printf("connect server ok.\n");

    chans_add_channel(&ctx, sockfd, CHANS_MASTER);
    chans_process(&ctx);

    close(sockfd);
    return 0;
}
