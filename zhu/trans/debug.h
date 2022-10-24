/*************************************************************************
  * File Name: src/common/debug.h
  * Description:
  * Author: sun.qingmeng
  * Mail:sqm2050@gmail.com
  * Created_Time: 2020-09-18 14:53:58
  * Last modified: 2020-09-18 15:57:19
 ************************************************************************/
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <pcap/pcap.h>

#define LOGE(...) printf(__VA_ARGS__)
#define LOGI(...) printf(__VA_ARGS__)

//assert when compile
#define STATIC_ZERO_ASSERT(p) (sizeof(struct { int:-!(p);}))
#define STATIC_NULL_ASSERT(p) ((void *)STATIC_ZERO_ASSERT(p))
#define STATIC_ASSERT(p) ((void)STATIC_ZERO_ASSERT(p))
//assert when preprocess
#define STATIC_ASSERT_MSG(COND, MSG) \
	typedef char Static_Assert_##MSG[(COND)?1:-1]


static inline int dump_pcap(char *out, uint8_t *data, int len)
{
	/* open outhandle */
	pcap_t *outhandle = pcap_open_dead(DLT_EN10MB, 65535);
	if (!outhandle) {
		printf("outhandle is NULL\n");
		return -1;
	}

	/* open out file to outdumper */
	pcap_dumper_t *outdumper = pcap_dump_open(outhandle, out);
    struct pcap_pkthdr header;
    header.caplen = header.len = len;
    pcap_dump((void *)outdumper, &header, data);
	pcap_dump_close(outdumper);
	return 0;
}

static inline void hexdump(FILE *fp, const char *name, const void *ptr, size_t len)
{
	const char *p = (const char *)ptr;
	unsigned int of = 0;

	if (name)
		fprintf(fp, "%s hexdump (%zd bytes):\n", name, len);

	for (of = 0 ; of < len ; of += 16) {
		char hexen[16*3+1];
		char charen[16+1];
		int hof = 0;

		int cof = 0;
		int i;

		for (i = of ; i < (int)of + 16 && i < (int)len ; i++) {
			hof += sprintf(hexen+hof, "%02x ", p[i] & 0xff);
			cof += sprintf(charen+cof, "%c",
				       isprint((int)p[i]) ? p[i] : '.');
		}
		fprintf(fp, "%08x: %-48s %-16s\n", of, hexen, charen);
	}
}

#ifdef __cplusplus
}
#endif

#endif
