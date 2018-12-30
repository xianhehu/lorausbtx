#ifndef __GATEWAY_H__
#define __GATEWAY_H__

#include <stdint.h>

#pragma pack(1)
typedef struct {
    uint8_t  lora;
    uint8_t  ncrc;
    uint32_t freq;
    uint8_t  bw;
    uint8_t  sf;
    uint8_t  codr;
    uint8_t  rev1;
    uint16_t rev2;
    uint8_t  swd;
    uint16_t pream;
} gw_cfg_t;

typedef struct {
    uint8_t  imme;
    uint64_t tmst;
    uint8_t  ncrc;
    uint32_t freq;
    uint8_t  rfch;
    uint8_t  powe;
    uint8_t  lora;
    uint8_t  sf;
    uint8_t  bw;
    uint8_t  codr;
    uint8_t  fdev;
    uint8_t  ipol;
    uint16_t prea;
    uint8_t  size;
    uint8_t  data[];
} gw_pkt_tx_t;

typedef struct {
    uint64_t tmst;
    uint8_t  chan;
    uint8_t  rfch;
    uint32_t freq;
    uint8_t  stat;
    uint8_t  lora;
    uint8_t  sf;
    uint8_t  bw;
    uint8_t  codr;
    int8_t   lsnr;
    uint8_t  rssi;
    uint8_t  size;
    uint8_t  data[];
} gw_pkt_rx_t;
#pragma pack()

void     GW_Init               (void);
void     GW_Start              (void);
void     GW_Stop               (void);
void     GW_Cfg                (gw_cfg_t *cfg);

int      GW_PutTx              (gw_pkt_tx_t *p);
int      GW_PollTx             (gw_pkt_tx_t *p);
int      GW_PutRx              (gw_pkt_rx_t *p);
int      GW_PollRx             (gw_pkt_rx_t *p);
int      GW_PollRxByte         (uint8_t *b);

void     GW_DropTx             (void);
uint32_t GW_GetRxRingbufLen    (void);
int      GW_SendPkt            (gw_pkt_tx_t *txpkt);

#define SIZEOFPKTRX(p) (sizeof(gw_pkt_rx_t)+p->size)
#define SIZEOFPKTTX(p) (sizeof(gw_pkt_tx_t)+p->size)

#endif