#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define DEBUG

#ifdef DEBUG
#define debug_print(...) printf(__VA_ARGS__)
#else
#define debug_print(...)
#endif

static inline void hexdump(FILE *fp, const char *name, const void *ptr,
                           size_t len)
{
    const char *p = (const char *)ptr;
    unsigned int of = 0;

    if (name)
        fprintf(fp, "%s hexdump (%zd bytes):\n", name, len);

    for (of = 0; of < len; of += 16) {
        char hexen[16 * 3 + 1];
        char charen[16 + 1];
        int hof = 0;

        int cof = 0;
        int i;

        for (i = of; i < (int)of + 16 && i < (int)len; i++) {
            hof += sprintf(hexen + hof, "%02x ", p[i] & 0xff);
            cof += sprintf(charen + cof, "%c", isprint((int)p[i]) ? p[i] : '.');
        }
        fprintf(fp, "%08x: %-48s %-16s\n", of, hexen, charen);
    }
}


#ifdef __cplusplus
}
#endif

#endif
