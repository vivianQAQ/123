#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CHANS_MAP(XX) \
    XX(0, EMPTY)      \
    XX(1, MASTER)     \
    XX(2, LISTEN)     \
    XX(3, WAIT)       \
    XX(4, CLIENT)     \
    XX(5, CLOSED)

enum chans_enum {
#define XX(num, name) CHANS_##name = num,
    CHANS_MAP(XX)
#undef XX
};

#pragma pack(push, 1)
typedef struct proxy_hdr_t {
    int package_len;
    int flag;
    char buf[0];
} proxy_hdr;
#pragma pack(pop)

typedef struct userinfo_t {
    int type;
    int fd;
} userinfo;

typedef struct chans_t {
    int flag;
    int master_sock;
    int maxfd;
    userinfo ufd[64];
} chans;

int chans_add_channel(chans *ctx, int fd, int type);
int chans_process(chans *ctx);

//int recv_all(int fd, void * buf, int len);
//int recv_data(int fd, void * buf, int len);
//int send_all(int fd, void * buf, int len);

#ifdef __cplusplus
}
#endif

#endif
