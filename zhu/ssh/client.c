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

static int ufd[64];

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

int Socket_Accept(int connectfd, fd_set tempfds, int sockfd, RC4_KEY *key)
{
    int i, acceptfd, num = 0;
    for (i = 0; i < 64; i++) {
        if (ufd[i] >= 0) {
            num++;
        }
    }

    if (num >= 64) {
        return 0;
    }

    if (!FD_ISSET(connectfd, &tempfds)) {
        return 0;
    }

    struct sockaddr_in clientaddr;
    bzero(&clientaddr, sizeof(clientaddr));
    socklen_t clientlen = sizeof(clientaddr);
    if ((acceptfd = accept(connectfd, (struct sockaddr *)&clientaddr,
                           &clientlen)) < 0) {
        perror("accept error.");
        exit(-1);
    }

    for (i = 0; i < 64; i++) {
        if (ufd[i] < 0) {
            ufd[i] = acceptfd;
            break;
        }
    }

    int len = sizeof(UserInfo);
    UserInfo uinfo, uinfo2;
    bzero(&uinfo, len);
    bzero(&uinfo2, len);
    int flag = 1;
    uinfo.package_len = htonl(0);
    uinfo.fd = htonl(acceptfd);
    uinfo.flag = htonl(flag);
    RC4(key, len, (unsigned char *)&uinfo, (unsigned char *)&uinfo2);

    int val = send_all(sockfd, &uinfo2, len);
    if (val < 0) {
        perror("send data error");
        exit(0);
    }

    return 0;
}

void Send_Ser_Data(fd_set tempfds, int sockfd, RC4_KEY *key)
{
    int i;
    for (i = 0; i < 64; i++) {
        if (ufd[i] < 0) {
            continue;
        }

        if (!FD_ISSET(ufd[i], &tempfds)) {
            continue;
        }

        char recvbuf[1024];
        memset(recvbuf, 0, sizeof(recvbuf));
        int ret = recv_data(ufd[i], recvbuf, sizeof(recvbuf));
        if (ret == 0) {
            continue;
        }

        if (ret < 0) {
            close(ufd[i]);
            ufd[i] = -1;
            printf("(%s) port[%d] close\n", strerror(errno), i);
            continue;
        }

        int len = sizeof(UserInfo);
        int flag = 0;
        UserInfo *uinfo = malloc(len + ret);
        bzero(uinfo, len + ret);
        uinfo->package_len = htonl(ret);
        uinfo->fd = htonl(ufd[i]);
        uinfo->flag = htonl(flag);
        memcpy(uinfo->buf, recvbuf, ret);
        //hexdump(stdout, "send data", (void *)uinfo, len + ret);

        unsigned char *inbuf = (unsigned char *)malloc(ret + len);
        memset(inbuf, 0, ret + len);
        RC4(key, len + ret, (unsigned char *)uinfo, inbuf);

        int val = send_all(sockfd, inbuf, len + ret);
        if (val < 0) {
            perror("send data error");
            exit(0);
        }

        free(uinfo);
        free(inbuf);
    }
}

int Handle_Ser_Data(int sockfd, RC4_KEY *key)
{
    int N = sizeof(UserInfo) + 1024;
    char recvbuf[N];
    memset(recvbuf, 0, sizeof(recvbuf));
    int ret = recv_all(sockfd, recvbuf, sizeof(UserInfo));
    if (ret < 0) {
        perror("read error");
        exit(0);
    }

    UserInfo uinfo;
    bzero(&uinfo, sizeof(UserInfo));
    RC4(key, ret, (unsigned char *)recvbuf, (unsigned char *)&uinfo);
    uinfo.fd = ntohl(uinfo.fd);
    uinfo.package_len = ntohl(uinfo.package_len);

    memset(recvbuf, 0, sizeof(recvbuf));
    ret = recv_all(sockfd, recvbuf, uinfo.package_len);
    if (ret < 0) {
        perror("read error");
        exit(0);
    }

    unsigned char *outbuf = (unsigned char *)malloc(ret);
    memset(outbuf, 0, ret);
    RC4(key, ret, (unsigned char *)recvbuf, outbuf);

    //hexdump(stdout, "recv buf", outbuf, ret);
    int val = send_all(uinfo.fd, outbuf, ret);
    if (val < 0) {
        int i;
        for(i = 0;i < 64;i++) {
            if(ufd[i] == uinfo.fd);
            close(ufd[i]);
            ufd[i] = -1;
            perror("send data error");
            break;
        }
    }

    free(outbuf);
    return 0;
}

int main(int argc, const char *argv[])
{
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
    serveraddr.sin_port = htons(atoi(argv[2]));
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <
        0) {
        perror("connect failed.");
        exit(-1);
    }
    printf("connect server ok.\n");

    int i, maxfd;
    fd_set tempfds;
    for (i = 0; i < 64; i++) {
        ufd[i] = -1;
    }

    RC4_KEY key, key2;
    unsigned char *data = (unsigned char *)("hello");
    int length = strlen((char *)data);
    RC4_set_key(&key, length, data);
    RC4_set_key(&key2, length, data);

    int connectfd = Creat_Listenfd();
    while (1) {
        maxfd = connectfd > sockfd ? connectfd : sockfd;
        FD_ZERO(&tempfds);
        FD_SET(sockfd, &tempfds);
        FD_SET(connectfd, &tempfds);
        for (i = 0; i < 64; i++) {
            if (ufd[i] > 0) {
                if (ufd[i] > maxfd) {
                    maxfd = ufd[i];
                }
                FD_SET(ufd[i], &tempfds);
            }
        }
        int val = select(maxfd + 1, &tempfds, NULL, NULL, NULL);
        if (val < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select failed");
            exit(-1);
        }
        Socket_Accept(connectfd, tempfds, sockfd, &key);
        if (FD_ISSET(sockfd, &tempfds)) {
            Handle_Ser_Data(sockfd, &key2);
        }
        Send_Ser_Data(tempfds, sockfd, &key);
    }

    for (i = 0; i < 64; i++) {
        if (ufd[i] >= 0) {
            close(ufd[i]);
        }
    }
    close(sockfd);
    return 0;
}
