#ifndef __CHANNELS_H__
#define __CHANNELS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CHANS_MAP(XX) \
    XX(0, EMPTY)      \
    XX(1, MASTER)     \
    XX(2, LISTENER)   \
    XX(3, WAIT)       \
    XX(4, CLIENT)     \
    XX(5, CLOSED)

enum chans_enum {
#define XX(num, name) CHANS_##name = num,
    CHANS_MAP(XX)
#undef XX
};

#define CHANS_CMD(XX) \
    XX(0, NONE)       \
    XX(1, CONN_REQ)   \
    XX(2, CONN_RSP)   \
    XX(3, TRANS)      \
    XX(4, CLOSE)

enum chans_cmd_enum {
#define XX(num, name) CMD_##name = num,
    CHANS_CMD(XX)
#undef XX
};
#define DEF_CHANS_NUM 8

typedef struct channel {
    int type;           /* channel type/state */
    int self;           /* my own channel identifier */
    int remote_id;      /* channel identifier for remote peer */
    int have_remote_id; /* non-zero if remote_id is valid */

    int flags;    /* close sent/rcvd */
    int rfd;      /* read fd */
    int wfd;      /* write fd */
    int efd;      /* extended fd */
    int sock;     /* sock fd */
    int ctl_chan; /* control channel (multiplexed connections) */

} channel_t;

typedef struct chans_ctx {
    int maxfd;
    int chans_alloc;
    int chans_empty;
    int master_sock;
    uint32_t s5_bind;
    channel_t *chans;
} chans_ctx_t;

typedef struct proxy_hdr {
    char cmd;
    char is_ok;
    int self;
    int remote_id;
    int data_len;
    char data[];
} proxy_hdr_t;

typedef struct conn_info {
    uint32_t bind;
    char name[64];
    uint16_t port;
} conn_info_t;

int chans_init_ctx(chans_ctx_t *ctx);
channel_t *chans_add_channel(chans_ctx_t *ctx, int sock, int type);
int chans_process(chans_ctx_t *ctx);
int chans_set_proxy_bind(chans_ctx_t *ctx, char *addr);
void chans_status(chans_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif
