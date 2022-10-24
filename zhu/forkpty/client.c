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

int main(int argc, const char *argv[])
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed.");
        exit(-1);
    }

    //connect();
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

    RC4_KEY key1, key2;
    unsigned char *data = (unsigned char *)("hello");
    int length = strlen((char *)data);
    RC4_set_key(&key1, length, data);
    RC4_set_key(&key2, length, data);

    //终端属性
    FILE *in;
    in = stdin;
    struct termios newts, oldts;
    if (tcgetattr(fileno(in), &oldts) != 0) {
        perror("tcgetattr error");
        exit(-1);
    }
    newts = oldts;
    newts.c_lflag &= ~(ICANON | ECHO | ISIG);
    newts.c_cc[VTIME] = 0;
    newts.c_cc[VMIN] = 0;
    //tcflush(fileno(in), TCIFLUSH);
    if (tcsetattr(fileno(in), TCSAFLUSH, &newts) != 0) {
        perror("tcsetattr error");
        exit(-1);
    }

    char buf[1024] = {0};
    while (1) {
        int maxfd;
        fd_set tempfds;
        FD_ZERO(&tempfds);
        FD_SET(sockfd, &tempfds);
        FD_SET(0, &tempfds);
        maxfd = sockfd;
        int val = select(maxfd + 1, &tempfds, NULL, NULL, NULL);
        if (val < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select failed");
            exit(-1);
        }
        if (FD_ISSET(0, &tempfds)) {
            memset(buf, 0, sizeof(buf));
            int ret = read(0, buf, sizeof(buf));
            if (ret <= 0) {
                if ((errno == EINTR || errno == EWOULDBLOCK ||
                     errno == EAGAIN)) {
                    continue;
                }
                exit(-1);
            }
            unsigned char *str;
            str = (unsigned char *)malloc(ret + 1);
            memset(str, 0, ret + 1);
            RC4(&key1, ret, (unsigned char *)buf, str);
            write(sockfd, str, ret);
        }
        if (FD_ISSET(sockfd, &tempfds)) {
            memset(buf, 0, sizeof(buf));
            int ret = read(sockfd, buf, sizeof(buf));
            if (ret <= 0) {
                if ((errno == EINTR || errno == EWOULDBLOCK ||
                     errno == EAGAIN)) {
                    continue;
                }
                close(sockfd);
                exit(-1);
            }
            unsigned char *plain;
            plain = (unsigned char *)malloc(ret + 1);
            memset(plain, 0, ret + 1);
            RC4(&key2, ret, (unsigned char *)buf, plain);
            write(1, plain, ret);
        }
    }

    close(sockfd);
    return 0;
}
