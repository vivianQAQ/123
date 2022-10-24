#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>

#include "channels.h"

int open_listening_port(uint16_t port)
{
    int ret = 0;
    int sockfd;
    struct sockaddr_in addr;
    socklen_t sock_len = sizeof(struct sockaddr_in);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        printf("get socket fail.\n");
        return -1;
    }

    memset(&addr, 0x00, sock_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    ret = bind(sockfd, (struct sockaddr *)&addr, sock_len);
    if (0 > ret) {
        printf("bind fail.\n");
        return -1;
    }

    if (listen(sockfd, 2) == -1) {
        printf("listen fail.\n");
        return -1;
    }

    return sockfd;
}

int main()
{
    int sock;

#ifdef WIN32
    WSADATA wsadata;
    int err;

    err = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if (err != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
        return 1;
    }
#endif

    chans_ctx_t chans_ctx;
    chans_init_ctx(&chans_ctx);
    chans_set_proxy_bind(&chans_ctx, "192.168.23.32");
    chans_set_proxy_bind(&chans_ctx, "10.23.0.32");

    sock = open_listening_port(1183);
    if (0 > sock) {
        printf("listen port fail.\n");
        return -1;
    }

    chans_add_channel(&chans_ctx, sock, CHANS_LISTENER);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton("10.0.0.183", (struct in_addr *)&(server.sin_addr.s_addr));
    server.sin_port = htons(1181);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "call socket err(%d)\n", errno);
        return -1;
    }

    struct timeval timeout = {20, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout,
               sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout,
               sizeof(timeout));

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "call connect err(%d)\n", errno);
        return -1;
    }

    chans_add_channel(&chans_ctx, sock, CHANS_MASTER);

    chans_process(&chans_ctx);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    return 0;
}
