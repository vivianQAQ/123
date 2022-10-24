#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "common.h"

//直到读取的长度符合要求
int recv_all(int fd, void *buf, int len)
{
    int recvlen = 0;
    int should_recvlen = len;
    char *recvbuf = buf;
    while (1) {
        if (recvlen == len) {
            break;
        }

        int ret = recv(fd, recvbuf, should_recvlen, 0);
        if (ret <= 0) {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                break;
            }
            printf("(%s)\n", strerror(errno));
            return -1;
        }

        recvbuf += ret;
        should_recvlen -= ret;
        recvlen += ret;
    }

    return recvlen;
}

int send_all(int fd, void *buf, int len)
{
    int sendlen = 0;
    int should_sendlen = len;
    char *sendbuf = buf;
    while (1) {
        if (sendlen == len) {
            break;
        }

        int ret = send(fd, sendbuf, should_sendlen, 0);
        if (ret <= 0) {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                break;
            }
            printf("(%s)\n", strerror(errno));
            return -1;
        }

        sendbuf += ret;
        should_sendlen -= ret;
        sendlen += ret;
    }

    return sendlen;
}

int recv_data(int fd, void *buf, int len)
{
    int ret = recv(fd, buf, len, 0);
    if (ret <= 0) {
        if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
            return 0;
        }
        printf("(%s)\n", strerror(errno));
        return -1;
    }

    return ret;
}
