#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include "common.h"

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

int chans_update_maxfd(chans *ctx)
{
    int i, maxfd = -1;
    for (i = 0; i < 64; i++) {
        if (maxfd < ctx->ufd[i].fd) {
            maxfd = ctx->ufd[i].fd;
        }
    }
    ctx->maxfd = maxfd;
    return 0;
}

int chans_add_channel(chans *ctx, int sockfd, int type)
{
    int i, found;
    for (i = 0; i < 64; i++) {
        if (ctx->ufd[i].fd == 0) {
            found = i;
            break;
        }
    }

    userinfo *info;
    info = &ctx->ufd[found];
    info->type = type;
    info->fd = sockfd;
    if (CHANS_MASTER == type) {
        ctx->master_sock = sockfd;
    }
    chans_update_maxfd(ctx);
    return 0;
}

int Creat_Connectfd(chans *ctx)
{
    int i, num = 0;
    for (i = 0; i < 64; i++) {
        if (ctx->ufd[i].fd > 0) {
            num++;
        }
    }

    if (num >= 64) {
        return 0;
    }

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
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <
        0) {
        perror("connect failed.");
        exit(-1);
    }

    chans_add_channel(ctx, sockfd, CHANS_CLIENT);
    return 0;
}

int chans_master(chans *ctx, int id)
{
    userinfo info;
    info = ctx->ufd[id];
    int sock = info.fd;

    char recvbuf[1024];
    memset(recvbuf, 0, sizeof(recvbuf));
    int ret = recv_all(ctx->master_sock, recvbuf, sizeof(proxy_hdr));
    if (ret < 0) {
        perror("read error");
        exit(0);
    }

    proxy_hdr *hdr = (proxy_hdr *)recvbuf;
    hdr->flag = ntohl(hdr->flag);
    hdr->package_len = ntohl(hdr->package_len);

    if (hdr->flag == 1) {
        Creat_Connectfd(ctx);
        return 0;
    }

    memset(recvbuf, 0, sizeof(recvbuf));
    ret = recv_all(ctx->master_sock, recvbuf, hdr->package_len);
    if (ret < 0) {
        perror("read error");
        exit(0);
    }

    int val = send_all(sock, recvbuf, ret);
    if (val < 0) {
        int i;
        for (i = 0; i < 64; i++) {
            close(sock);
            ctx->ufd[id].fd = 0;
            perror("send data error");
            break;
        }
    }
    return 0;
}

int chans_listen(chans *ctx, int id)
{
    int i, sockfd, client, num = 0;
    for (i = 0; i < 64; i++) {
        if (ctx->ufd[i].fd > 0) {
            num++;
        }
    }

    if (num >= 64) {
        return 0;
    }

    userinfo info;
    info = ctx->ufd[id];
    sockfd = info.fd;

    struct sockaddr_in clientaddr;
    bzero(&clientaddr, sizeof(clientaddr));
    socklen_t clientlen = sizeof(clientaddr);
    if ((client = accept(sockfd, (struct sockaddr *)&clientaddr, &clientlen)) <
        0) {
        perror("accept error.");
        exit(-1);
    }

    chans_add_channel(ctx, client, CHANS_CLIENT);

    int len = sizeof(proxy_hdr);
    proxy_hdr hdr;
    bzero(&hdr, len);
    int flag = 1;
    hdr.flag = htonl(flag);

    int val = send_all(ctx->master_sock, &hdr, len);
    if (val < 0) {
        perror("send data error");
        exit(0);
    }
    return 0;
}

int chans_proxy_transform(chans *ctx, int id)
{
    userinfo info;
    info = ctx->ufd[id];
    int sockfd = info.fd;

    char recvbuf[1024];
    memset(recvbuf, 0, sizeof(recvbuf));
    int ret = recv_data(sockfd, recvbuf, sizeof(recvbuf));
    if (ret == 0) {
        return 0;
    }

    if (ret < 0) {
        close(sockfd);
        ctx->ufd[id].fd = 0;
        printf("(%s) port[%d] close\n", strerror(errno), id);
        return 0;
    }

    int len = sizeof(proxy_hdr);
    int flag = 0;
    proxy_hdr *hdr = malloc(len + ret);
    bzero(hdr, len + ret);
    hdr->package_len = htonl(ret);
    hdr->flag = htonl(flag);
    memcpy(hdr->buf, recvbuf, ret);

    int val = send_all(ctx->master_sock, hdr, len + ret);
    if (val < 0) {
        perror("send data error");
        exit(0);
    }

    free(hdr);
    return 0;
}

int chans_handler(chans *ctx, int id)
{
    int type = ctx->ufd[id].type;

    switch (type) {
    case CHANS_MASTER:
        chans_master(ctx, id);
        break;
    case CHANS_LISTEN:
        chans_listen(ctx, id);
        break;
    case CHANS_CLIENT:
        chans_proxy_transform(ctx, id);
        break;
    default:
        break;
    }
    return 0;
}

int chans_process(chans *ctx)
{
    int i;
    fd_set tempfds;
    for (i = 0; i < 64; i++) {
        ctx->ufd[i].fd = 0;
    }

    while (1) {
        FD_ZERO(&tempfds);
        for (i = 0; i < 64; i++) {
            if (ctx->ufd[i].fd > 0)
                FD_SET(ctx->ufd[i].fd, &tempfds);
        }
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        int val = select(ctx->maxfd + 1, &tempfds, NULL, NULL, &tv);
        if (val < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select failed.");
            exit(-1);
        } else if (val == 0) {
            continue;
        }

        for (i = 0; i < 64; i++) {
            if (!FD_ISSET(ctx->ufd[i].fd, &tempfds)) {
                continue;
            }
            chans_handler(ctx, i);
        }
    }
    return 0;
}
