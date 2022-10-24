#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include <netdb.h>

#include "debug.h"
#include "channels.h"

char *chans_str[] = {
#define XX(num, name) [num] = (#name),
    CHANS_MAP(XX)
#undef XX
};

int chans_close(chans_ctx_t *ctx, int id, int remote_id);

int chans_init_ctx(chans_ctx_t *ctx)
{
    int i;
    ctx->chans = (channel_t *)malloc(sizeof(channel_t) * DEF_CHANS_NUM);
    if (!ctx->chans) {
        return -1;
    }
    memset(ctx->chans, 0x00, sizeof(channel_t) * DEF_CHANS_NUM);
    ctx->chans_alloc = DEF_CHANS_NUM;
    ctx->chans_empty = DEF_CHANS_NUM;

    for (i = 0; i < DEF_CHANS_NUM; i++) {
        ctx->chans[i].type = CHANS_EMPTY;
    }

    return 0;
}

void chans_clear(chans_ctx_t *ctx)
{
    for (int i = 0; i < ctx->chans_alloc; i++) {
        if (0 < ctx->chans[i].sock) {
            chans_close(ctx, i, -1);
        }
    }
}

void chans_update_maxfd(chans_ctx_t *ctx)
{
    int maxfd = -1;

    for (int i = 0; i < ctx->chans_alloc; i++) {
        if (maxfd < ctx->chans[i].sock) {
            maxfd = ctx->chans[i].sock;
        }
    }

    ctx->maxfd = maxfd;
}

channel_t *chans_add_channel(chans_ctx_t *ctx, int sock, int type)
{
    int i;
    int found;
    int new_size;
    int chans_alloc;
    channel_t *chans, *chan;

    for (i = 0; i < ctx->chans_alloc; i++) {
        if (CHANS_EMPTY == ctx->chans[i].type) {
            found = i;
            break;
        }
    }

    if (i == ctx->chans_alloc) {
        chans_alloc = ctx->chans_alloc + 10;
        new_size = sizeof(channel_t) * chans_alloc;
        chans = (channel_t *)realloc(ctx->chans, new_size);
        if (!chans) {
            return NULL;
        }

        for (i = ctx->chans_alloc; i < chans_alloc; i++) {
            chans[i].type = CHANS_EMPTY;
        }

        found = ctx->chans_alloc;
        ctx->chans_alloc = chans_alloc;
        ctx->chans = chans;
        ctx->chans_empty += 10;
    }

    chan = &ctx->chans[found];
    chan->type = type;
    chan->sock = sock;
    chan->self = found;
    chan->rfd = sock;
    if (CHANS_MASTER == type) {
        ctx->master_sock = sock;
    }

    ctx->chans_empty -= 1;
    chans_update_maxfd(ctx);

    return chan;
}

int chans_recv_all(int sock, char *data, int data_len)
{
    int n;
    size_t sum = 0;
    char *offset = data;

    while (sum < data_len) {
        n = recv(sock, (void *)offset, data_len - sum, 0);

        if (n < 0 && (errno == EINTR || errno == EAGAIN)) {
            continue;
        }

        if (n <= 0) {
            return sum;
        }

        sum += n;
        offset += n;
    }

    return sum;
}

int chans_send_all(int sock, char *data, int data_len)
{
    int n;
    size_t sum = 0;
    char *offset = data;

    while (sum < data_len) {
        n = send(sock, (void *)offset, data_len - sum, 0);

        if (n < 0 && (errno == EINTR || errno == EAGAIN)) {
            continue;
        }

        if (n < 0) {
            return sum;
        }

        sum += n;
        offset += n;
    }

    return sum;
}

int chans_send_conn_req(chans_ctx_t *ctx, int self)
{
    proxy_hdr_t hdr;

    memset(&hdr, 0x00, sizeof(hdr));

    hdr.cmd = CMD_CONN_REQ;
    hdr.self = htonl(self);
    hdr.data_len = 0;

    chans_send_all(ctx->master_sock, (void *)&hdr, sizeof(hdr));

    return 0;
}

int chans_send_conn_rsp(chans_ctx_t *ctx, int self, int remote_id, int is_ok)
{
    proxy_hdr_t hdr;

    memset(&hdr, 0x00, sizeof(hdr));

    hdr.cmd = CMD_CONN_RSP;
    hdr.is_ok = is_ok;
    hdr.self = htonl(self);
    hdr.remote_id = htonl(remote_id);
    hdr.data_len = 0;

    chans_send_all(ctx->master_sock, (void *)&hdr, sizeof(hdr));

    return 0;
}

int chans_proxy_connect(chans_ctx_t *ctx, int id)
{
    int sock;
    channel_t *chan;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("192.168.0.1");
    server.sin_port = htons(80);

    socklen_t addrlen = sizeof(struct sockaddr);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Couldn't make socket!\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server, addrlen) == -1) {
        printf("Could not connect to remote shell!\n");
        return -1;
    }

    chan = chans_add_channel(ctx, sock, CHANS_CLIENT);
    if (!chan) {
        chans_send_conn_rsp(ctx, -1, id, 0);
        return -1;
    }

    chan->wfd = ctx->master_sock;
    chan->have_remote_id = 1;
    chan->remote_id = id;
    chans_send_conn_rsp(ctx, chan->self, id, 1);

    return 0;
}

int chans_proxy_confirm(chans_ctx_t *ctx, proxy_hdr_t *hdr)
{
    int is_ok = hdr->is_ok;
    int self = ntohl(hdr->remote_id);

    if (!is_ok) {
        chans_close(ctx, self, -1);
        return -1;
    }

    int remote_id = ntohl(hdr->self);

    channel_t *chan = &ctx->chans[self];
    chan->wfd = ctx->master_sock;
    chan->have_remote_id = 1;
    chan->remote_id = remote_id;

    chan->type = CHANS_CLIENT;

    return 0;
}

int chans_master_transform(chans_ctx_t *ctx, int id, char *data, int data_len)
{
    channel_t *chan;
    chan = &ctx->chans[id];

    printf("trans id: %d wfd:%d\n", id, chan->sock);

    chans_send_all(chan->sock, data, data_len);

    return 0;
}

int chans_send_close(chans_ctx_t *ctx, int id)
{
    channel_t *chan;
    proxy_hdr_t hdr;

    chan = &ctx->chans[id];
    memset(&hdr, 0x00, sizeof(hdr));

    hdr.cmd = CMD_CLOSE;
    hdr.self = htonl(chan->self);
    hdr.remote_id = htonl(chan->remote_id);

    chans_send_all(ctx->master_sock, (void *)&hdr, sizeof(hdr));

    return 0;
}

int chans_master(chans_ctx_t *ctx, int id)
{
    int n;
    int data_len;
    int sock;
    proxy_hdr_t hdr;
    channel_t *chan;
    char buf[4096] = {0};

    chan = &ctx->chans[id];
    sock = chan->sock;
    memset(buf, 0x00, sizeof(buf));

    n = recv(sock, (void *)&hdr, sizeof(hdr), 0);
    if (n != sizeof(hdr)) {
        chans_clear(ctx);
        return -1;
    }

    data_len = ntohl(hdr.data_len);
    if (data_len > 0) {
        chans_recv_all(sock, (void *)buf, data_len);
    }
    printf("master cmd:%d\n", hdr.cmd);
    printf("data len:%d\n", data_len);

    switch (hdr.cmd) {
    case CMD_CONN_REQ:
        chans_proxy_connect(ctx, ntohl(hdr.self));
        break;
    case CMD_CONN_RSP:
        chans_proxy_confirm(ctx, &hdr);
        break;
    case CMD_TRANS:
        chans_master_transform(ctx, ntohl(hdr.remote_id), buf, data_len);
        break;
    case CMD_CLOSE:
        chans_close(ctx, ntohl(hdr.remote_id), ntohl(hdr.self));
        break;
    default:
        break;
    }

    return 0;
}

int chans_listener(chans_ctx_t *ctx, int id)
{
    int sock, client;
    channel_t *chan;
    struct sockaddr_in client_addr;
    socklen_t sock_len = sizeof(struct sockaddr_in);

    chan = &ctx->chans[id];
    sock = chan->sock;

    client = accept(sock, (struct sockaddr *)&client_addr, &sock_len);
    if (0 > client) {
        return -1;
    }

    chan = chans_add_channel(ctx, client, CHANS_WAIT);
    if (!chan) {
        return -1;
    }

    chans_send_conn_req(ctx, chan->self);

    return 0;
}

int chans_proxy_transform(chans_ctx_t *ctx, int id)
{
    int n;
    int sock;
    proxy_hdr_t hdr;
    channel_t *chan;
    char buf[4096] = {0};

    memset(&hdr, 0x00, sizeof(hdr));
    memset(&buf, 0x00, sizeof(buf));

    chan = &ctx->chans[id];
    sock = chan->sock;

    n = recv(sock, buf, sizeof(buf), 0);
    if (n < 0 && (errno == EINTR || errno == EAGAIN)) {
        return 0;
    }

    if (0 >= n) {
        chans_close(ctx, id, -1);
        return 0;
    }

    hdr.cmd = CMD_TRANS;
    hdr.self = htonl(chan->self);
    hdr.remote_id = htonl(chan->remote_id);
    hdr.data_len = htonl(n);

    printf("%s:%d self:%d remote:%d\n", __func__, __LINE__, chan->self,
           chan->remote_id);
    printf("%s:%d data len:%d\n", __func__, __LINE__, n);

    chans_send_all(chan->wfd, (void *)&hdr, sizeof(hdr));
    chans_send_all(chan->wfd, (void *)buf, n);

    return 0;
}

int chans_handler(chans_ctx_t *ctx, int id)
{
    int type = ctx->chans[id].type;

    printf("id: %d type: %s(%d)\n", id, chans_str[type], type);
    switch (type) {
    case CHANS_MASTER:
        chans_master(ctx, id);
        break;
    case CHANS_LISTENER:
        chans_listener(ctx, id);
        break;
    case CHANS_CLIENT:
        chans_proxy_transform(ctx, id);
        break;
    case CHANS_CLOSED:
        break;
    default:
        break;
    }
    return 0;
}

int chans_close(chans_ctx_t *ctx, int id, int remote_id)
{
    channel_t *chan = &ctx->chans[id];

    if (CHANS_EMPTY == chan->type) {
        return 0;
    }

    if ((remote_id > 0) && (chan->remote_id != remote_id)) {
        return 0;
    }

    chan->type = CHANS_EMPTY;

#ifdef WIN32
    closesocket(chan->sock);
#else
    close(chan->sock);
#endif

    chan->sock = -1;
    printf("%s:%d self: %d have remote:%d remote:%d\n", __func__, __LINE__,
           chan->self, chan->have_remote_id, chan->remote_id);
    if (chan->have_remote_id) {
        chans_send_close(ctx, id);
        printf("send close msg\n");
    }
    chan->have_remote_id = 0;
    chan->remote_id = -1;
    ctx->chans_empty += 1;

    return 0;
}

void chans_rfds_set(chans_ctx_t *ctx, fd_set *rfds)
{
    FD_ZERO(rfds);

    for (int i = 0; i < ctx->chans_alloc; i++) {
        if (0 < ctx->chans[i].sock) {
            FD_SET(ctx->chans[i].sock, rfds);
        }
    }
}

int chans_process(chans_ctx_t *ctx)
{
    int ret;
    fd_set rfds;
    struct timeval tv;

    while (1) {
        chans_rfds_set(ctx, &rfds);

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        ret = select(ctx->maxfd + 1, &rfds, NULL, NULL, &tv);
        if (ret == -1 && errno == EINTR) {
            continue;
        }

        if (ret < 0) {
            printf("exit: %s  (%d)\n", strerror(errno), errno);
            break;
        }

        chans_status(ctx);
        if (ctx->chans_empty == ctx->chans_alloc) {
            break;
        }

        for (int i = 0; i < ctx->chans_alloc; i++) {
            if (0 > ctx->chans[i].sock) {
                continue;
            }

            if (!FD_ISSET(ctx->chans[i].sock, &rfds)) {
                continue;
            }

            chans_handler(ctx, i);
        }
    }

    return 0;
}

int chans_set_proxy_bind(chans_ctx_t *ctx, char *addr)
{
    struct in_addr s;
    inet_pton(AF_INET, addr, (void *)&s);
    ctx->s5_bind = s.s_addr;
    return 0;
}

void chans_status(chans_ctx_t *ctx)
{
    int i, empty = 0;
    channel_t *chan;

    for (i = 0; i < ctx->chans_alloc; i++) {
        chan = &ctx->chans[i];
        if (CHANS_EMPTY == chan->type) {
            empty++;
            continue;
        }

        printf("self: %-4d sock: %-4d type: %-2d(%s) remote: %d\n", chan->self,
               chan->sock, chan->type, chans_str[chan->type], chan->remote_id);
    }
    printf("\ntotal: %d empty: %d used: %d\n", ctx->chans_alloc, empty,
           ctx->chans_alloc - empty);
}
