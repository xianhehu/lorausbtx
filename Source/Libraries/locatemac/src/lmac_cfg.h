#ifndef __LMAC_CFG_H__
#define __LMAC_CFG_H__

#include "common.h"

#define LMAC_DEBUG                printk          // enable debug

#define OSTICKS_PER_SEC           1000
#define LMAC_TIME_ERROR_RATE      10000                  // error:1s/10000s

#define LMAC_SCAN_FREQ_NUM        3
#define LMAC_MAX_CHANNELS         3

#define LMAC_CHANNEL_FREQS_UP     {470100000, 470300000, 470500000}
#define LMAC_CHANNEL_FREQS_DN     {500100000, 500300000, 500500000}
#define LMAC_CHANNEL_USERATE_LOW  2              /* only 20% channel slots are used */

#define LMAC_FRAME_LEN_MAX        255
#define LMAC_DATA_LEN_MAX         64

#define LMAC_ADV_MAX_INTERVAL     (100000)  /* 100s */
#define LMAC_ADV_MAX_TIMES        10
#define LMAC_ADV_CHANNELS         4
#define LMAC_ADV_CHANNEL_FREQS    {500100000, 500300000, 500500000}

#define LMAC_ADV_RXTIME_FIXUP     (991+28) /* air time + receiver handle time */


#define LMAC_CHANNEL_MINRSSI_SF7  110   /* -110dB */
#define LMAC_CHANNEL_MINRSSI_SF8  115   /* -115dB */
#define LMAC_CHANNEL_MINRSSI_SF9  120   /* -120dB */
#define LMAC_CHANNEL_MINRSSI_SF10 125   /* -125dB */
#define LMAC_CHANNEL_MINRSSI_SF11 130   /* -130dB */

#define LMAC_SEND_CSMA_RSSI       (-100)

#endif