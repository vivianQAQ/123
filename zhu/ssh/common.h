#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
typedef struct UINFO {
    int package_len;
    int fd;
    int flag;
    char buf[0];
} UserInfo;
#pragma pack(pop)
typedef struct _USERFD {
    int fd1;
    int fd2;
} userfd;
//void Pack_Data(int packlen, int fd, int flag, char *buf, char *pktbuf);
int recv_all(int fd, void * buf, int len);
int recv_data(int fd, void * buf, int len);
int send_all(int fd, void * buf, int len);

#ifdef __cplusplus
}
#endif

#endif
