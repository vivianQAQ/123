#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "debug.h"
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
    int sock, client;
    struct sockaddr_in client_addr;
    socklen_t sock_len = sizeof(struct sockaddr_in);

    chans_ctx_t chans_ctx;
    chans_init_ctx(&chans_ctx);

    sock = open_listening_port(1181);
    if (0 > sock) {
        printf("listen port fail.\n");
        return -1;
    }

    client = accept(sock, (struct sockaddr *)&client_addr, &sock_len);
    if (0 > client) {
        close(sock);
        return -1;
    }

    LOGE("new connect %d\n", client);

    close(sock);

    chans_add_channel(&chans_ctx, client, CHANS_MASTER);

    chans_process(&chans_ctx);

    return 0;
}
