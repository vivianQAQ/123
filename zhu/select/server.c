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
#define N 1024

static int fdA[64];

void Send_Data(fd_set tempfds)
{
    if (FD_ISSET(0, &tempfds)) {
        char buf[N];
        memset(buf, 0, sizeof(buf));
        int ret = read(0, buf, sizeof(buf));
        if (ret <= 0) {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                printf("error : %s\n", strerror(errno));
            }
            exit(-1);
        }
        int i;
        for (i = 0; i < 64; i++) {
            if (fdA[i] > 0) {
                send(fdA[i], buf, sizeof(buf), 0);
            }
        }
    }
}

void Recv_Data(fd_set tempfds)
{
    int i;
    for (i = 0; i < 64; i++) {
        if (FD_ISSET(fdA[i], &tempfds)) {
            char buf[N];
            memset(buf, 0, sizeof(buf));
            int ret = recv(fdA[i], buf, sizeof(buf), 0);
            if (ret <= 0) {
                if ((errno == EINTR || errno == EWOULDBLOCK ||
                     errno == EAGAIN)) {
                    continue;
                }
                close(fdA[i]);
                printf("client[%d] %d close\n", i, fdA[i]);
                fdA[i] = -1;
            } else {
                memset(&buf[ret - 1], '\0', 1);
                printf("client[%d] send:%s\n", i, buf);
            }
        }
    }
}

void Socket_Accept(int sockfd, fd_set tempfds)
{
    int i, acceptfd, num = 0;
    for (i = 0; i < 64; i++) {
        if (fdA[i] >= 0) {
            num++;
        }
    }
    if (num < 64) {
        if (FD_ISSET(sockfd, &tempfds)) {
            struct sockaddr_in clientaddr;
            bzero(&clientaddr, sizeof(clientaddr));
            socklen_t clientlen = sizeof(clientaddr);
            if ((acceptfd = accept(sockfd, (struct sockaddr *)&clientaddr,
                                   &clientlen)) < 0) {
                perror("accept error.");
                exit(-1);
            }
            printf("accept ok.\n");
            for (i = 0; i < 64; i++) {
                if (fdA[i] < 0) {
                    fdA[i] = acceptfd;
                    break;
                }
            }
        }
    }
}

void Socket_Select(int sockfd)
{
    int i, maxfd;
    fd_set tempfds;
    for (i = 0; i < 64; i++) {
        fdA[i] = -1;
    }
    while (1) {
        maxfd = sockfd;
        FD_ZERO(&tempfds);
        FD_SET(0, &tempfds);
        FD_SET(sockfd, &tempfds);
        for (i = 0; i < 64; i++) {
            if (fdA[i] > 0) {
                if (fdA[i] > maxfd) {
                    maxfd = fdA[i];
                }
                FD_SET(fdA[i], &tempfds);
            }
        }

        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        int val = select(maxfd + 1, &tempfds, NULL, NULL, &tv);
        if (val < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select failed.");
            exit(-1);
        } else if (val == 0) {
            printf("timeout.\n");
            continue;
        }
        Socket_Accept(sockfd, tempfds);
        Recv_Data(tempfds);
        Send_Data(tempfds);
    }

    for (i = 0; i < 64; i++) {
        if (fdA[i] > 0) {
            close(fdA[i]);
        }
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 2) {
        printf("input port!\n");
        exit(-1);
    }

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
    serveraddr.sin_port = htons(atoi(argv[1]));
    serveraddr.sin_addr.s_addr = inet_addr("10.0.0.87");
    socklen_t serverlen = sizeof(serveraddr);
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

    Socket_Select(sockfd);
    close(sockfd);
}
