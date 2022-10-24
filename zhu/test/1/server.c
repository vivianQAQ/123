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

void Send_Data(fd_set tempfds, int acceptfd, RC4_KEY *key)
{
    if (FD_ISSET(0, &tempfds)) {
        char buf[256];
        memset(buf, 0, sizeof(buf));
        int ret = read(0, buf, sizeof(buf));
        if (ret <= 0)
            exit(0);
        int len = strlen(buf);

        unsigned char *str = (unsigned char *)malloc(len + 1);
        memset(str, 0, len + 1);
        RC4(key, strlen(buf), (unsigned char *)buf, str);
        printf("buf : %ld , %s\n", strlen(buf), buf);
        printf("send : %ld , %s\n", strlen((char *)str), str);
        send(acceptfd, str, sizeof(str), 0);
    }
}

void Recv_Data(fd_set tempfds, int acceptfd, RC4_KEY *key)
{
    if (FD_ISSET(acceptfd, &tempfds)) {
        char buf[256];
        memset(buf, 0, sizeof(buf));
        int ret = recv(acceptfd, buf, sizeof(buf), 0);
        if (ret <= 0)
            exit(0);
        unsigned char *plain = (unsigned char *)malloc(ret + 1);
        memset(plain, 0, ret + 1);
        RC4(key, strlen(buf), (unsigned char *)buf, plain);

        printf("buf : %ld , %s\n", strlen(buf), buf);
        printf("recv : %ld , %s\n", strlen((char *)plain), plain);
    }
}

void Socket_Select(int acceptfd)
{
    RC4_KEY key, key2;
    unsigned char *data = (unsigned char *)("hello");
    int length = strlen((char *)data);
    RC4_set_key(&key, length, data);
    RC4_set_key(&key2, length, data);

    int maxfd;
    fd_set tempfds;
    while (1) {
        maxfd = acceptfd;
        FD_ZERO(&tempfds);
        FD_SET(0, &tempfds);
        FD_SET(acceptfd, &tempfds);

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
        Send_Data(tempfds, acceptfd, &key);
        Recv_Data(tempfds, acceptfd, &key2);
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
    serveraddr.sin_addr.s_addr = inet_addr("10.0.0.87");
    socklen_t serverlen = sizeof(serveraddr);
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
