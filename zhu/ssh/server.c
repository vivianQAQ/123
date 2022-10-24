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

static userfd fd[64];

void Send_Cli_Data(fd_set tempfds, int acceptfd, RC4_KEY *key)
{
    int i;
    for (i = 0; i < 64; i++) {
        if (fd[i].fd2 < 0) {
            continue;
        }

        if (!FD_ISSET(fd[i].fd2, &tempfds)) {
            continue;
        }

        char recvbuf[1024];
        memset(recvbuf, 0, sizeof(recvbuf));

        int ret = recv_data(fd[i].fd2, recvbuf, sizeof(recvbuf));
        if (ret == 0) {
            continue;
        }

        if (ret < 0) {
            close(fd[i].fd2);
            fd[i].fd2 = -1;
            printf("(%s) port[%d] close\n", strerror(errno), i);
            continue;
        }

        int len = sizeof(UserInfo);
        int flag = 0;
        UserInfo *uinfo = malloc(len + ret);
        bzero(uinfo, len + ret);
        uinfo->package_len = htonl(ret);
        uinfo->fd = htonl(fd[i].fd1);
        uinfo->flag = htonl(flag);
        memcpy(uinfo->buf, recvbuf, ret);

        unsigned char *inbuf = (unsigned char *)malloc(ret + len);
        memset(inbuf, 0, ret + len);
        RC4(key, ret + len, (unsigned char *)uinfo, inbuf);

        int val = send_all(acceptfd, inbuf, len + ret);
        if (val < 0) {
            perror("send data error");
            exit(0);
        }
        //hexdump(stdout, "send buf", (void *)uinfo, len + ret);
        free(inbuf);
        free(uinfo);
    }
}

void Creat_Connectfd(int usefd)
{
    int i, num = 0;
    for (i = 0; i < 64; i++) {
        if (fd[i].fd2 >= 0) {
            num++;
        }
    }
    if (num < 64) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("socket failed.");
            exit(-1);
        }
        struct sockaddr_in serveraddr;
        bzero(&serveraddr, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(atoi("22"));
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (connect(sockfd, (struct sockaddr *)&serveraddr,
                    sizeof(serveraddr)) < 0) {
            perror("connect failed.");
            exit(-1);
        }
        for (i = 0; i < 64; i++) {
            if (fd[i].fd2 < 0) {
                fd[i].fd2 = sockfd;
                fd[i].fd1 = usefd;
                break;
            }
        }
    }
}

int Handle_Cli_Data(int acceptfd, RC4_KEY *key)
{
    int N = sizeof(UserInfo) + 1024;
    char recvbuf[N];
    memset(recvbuf, 0, sizeof(recvbuf));
    int ret = recv_all(acceptfd, recvbuf, sizeof(UserInfo));
    if (ret < 0) {
        perror("read error");
        exit(0);
    }

    UserInfo uinfo;
    bzero(&uinfo, sizeof(UserInfo));
    RC4(key, ret, (unsigned char *)recvbuf, (unsigned char *)&uinfo);

    uinfo.fd = ntohl(uinfo.fd);
    uinfo.flag = ntohl(uinfo.flag);
    uinfo.package_len = ntohl(uinfo.package_len);
    if (uinfo.flag == 1) {
        Creat_Connectfd(uinfo.fd);
        return 0;
    }

    memset(recvbuf, 0, sizeof(recvbuf));
    ret = recv_all(acceptfd, recvbuf, uinfo.package_len);
    if (ret < 0) {
        perror("read error");
        exit(0);
    }

    unsigned char *outbuf = (unsigned char *)malloc(ret);
    memset(outbuf, 0, ret);
    RC4(key, ret, (unsigned char *)recvbuf, outbuf);
    //hexdump(stdout, "recv data", outbuf, ret);

    int i, val;
    for (i = 0; i < 64; i++) {
        if (fd[i].fd1 == uinfo.fd) {
            val = send_all(fd[i].fd2, outbuf, ret);
            if (val < 0) {
                close(fd[i].fd2);
                fd[i].fd2 = -1;
                perror("send failed.");
            }
            break;
        }
    }
    free(outbuf);
    return 0;
}

void Socket_Select(int acceptfd)
{
    RC4_KEY key, key2;
    unsigned char *data = (unsigned char *)("hello");
    int length = strlen((char *)data);
    RC4_set_key(&key, length, data);
    RC4_set_key(&key2, length, data);

    int i, maxfd;
    fd_set tempfds;
    for (i = 0; i < 64; i++) {
        fd[i].fd1 = -1;
        fd[i].fd2 = -1;
    }
    while (1) {
        maxfd = acceptfd;
        FD_ZERO(&tempfds);
        FD_SET(acceptfd, &tempfds);
        for (i = 0; i < 64; i++) {
            if (fd[i].fd2 > 0) {
                if (fd[i].fd2 > maxfd) {
                    maxfd = fd[i].fd2;
                }
                FD_SET(fd[i].fd2, &tempfds);
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
            continue;
        }
        Send_Cli_Data(tempfds, acceptfd, &key);
        if (FD_ISSET(acceptfd, &tempfds)) {
            Handle_Cli_Data(acceptfd, &key2);
        }
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 2) {
        printf("input port!\n");
        exit(-1);
    }

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
    serveraddr.sin_port = htons(atoi(argv[1]));
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

    Socket_Select(acceptfd);
    close(sockfd);
}
