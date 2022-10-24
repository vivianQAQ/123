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

    char buf[1024] = {0};
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork failed.");
    } else if (pid == 0) {
        while (1) {
            memset(buf, 0, sizeof(buf));
            int val = read(0, buf, sizeof(buf));
            if (val <= 0) {
                exit(-1);
            }
            //buf[strlen(buf) - 1] = '\0';
            send(sockfd, buf, sizeof(buf), 0);
        }
    } else {
        while (1) {
            int ret = 0;
            memset(buf, 0, sizeof(buf));
            ret = recv(sockfd, buf, sizeof(buf), 0);
            if (ret <= 0) {
                if ((errno == EINTR || errno == EWOULDBLOCK ||
                     errno == EAGAIN)) {
                    continue;
                }
                exit(-1);
            }
            //memset(&buf[ret - 1], '\0', 1);
            printf("recvbuf is %s\n", buf);
        }
    }

    close(sockfd);
    return 0;
}
