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
#include <openssl/rc4.h>

int main(int argc, const char *argv[])
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
    serveraddr.sin_port = htons(atoi(argv[2]));
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <
        0) {
        perror("connect failed.");
        exit(-1);
    }

    RC4_KEY key1,key2;
    unsigned char *data = (unsigned char *)("hello");
    int length = strlen((char *)data);
    RC4_set_key(&key1, length, data);
    RC4_set_key(&key2, length, data);

    char buf[256] = {0};
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork failed.");
    } else if (pid == 0) {
        while (1) {
            memset(buf, 0, sizeof(buf));
            int ret = read(0, buf, sizeof(buf));
            if (ret <= 0)
                exit(0);
            unsigned char *outbuf = (unsigned char *)malloc(ret + 1);
            memset(outbuf, 0, ret + 1);
            RC4(&key1, strlen(buf), (unsigned char *)buf, outbuf);
            //printf("buf : %ld , %s\n", strlen(buf), buf);
            //printf("send : %ld , %s\n", strlen((char *)outbuf), outbuf);
            send(sockfd, outbuf, sizeof(outbuf), 0);
        }
    } else {
        while (1) {
            memset(buf, 0, sizeof(buf));
            int ret = recv(sockfd, buf, sizeof(buf), 0);
            if (ret <= 0)
                exit(0);
            unsigned char *str = (unsigned char *)malloc(ret + 1);
            memset(str, 0, ret + 1);
            RC4(&key2, strlen(buf), (unsigned char *)buf, str);
            //printf("buf : %ld , %s\n", strlen(buf), buf);
            printf("%s\n", str);
        }
    }

    close(sockfd);
    return 0;
}
