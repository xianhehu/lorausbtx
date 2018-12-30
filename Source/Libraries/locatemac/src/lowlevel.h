#ifndef __LOW_LEVEL_H__
#define __LOW_LEVEL_H__

#include "DataType.h"
#include "common.h"
#include "osal.h"

typedef struct {
    uint32_t freq;
    uint8_t  bw;
    uint8_t  sf;
    uint8_t  pw;
    uint8_t  len;
    uint8_t  *buf;
} lmac_ll_cfg_tx_t;

typedef struct {
    uint8_t  scanRssi;
    uint32_t freq;
    uint8_t  bw;
    uint8_t  sf;
    uint8_t  sym_timout;
    uint8_t  size;
    uint8_t  *buf;
} lmac_ll_cfg_rx_t;

typedef struct {
    void(*func)(void*, void *);
    uint32_t time;
} lmac_timer_t;

void      LMAC_LL_Lock(void);
void      LMAC_LL_Unlock(void);

void      LMAC_LL_Init(void);
void      LMAC_LL_RadioSend(lmac_ll_cfg_tx_t *cfg);
void      LMAC_LL_RadioListen(lmac_ll_cfg_rx_t *cfg);
uint8_t   LMAC_LL_RadioRecv(uint8_t *buf);
void      LMAC_LL_RadioReadPacketRssi(int16_t *rssi, int8_t *snr);
void      LMAC_LL_RadioStop(void);

void      LMAC_LL_Delay(uint32_t ms);
void      LMAC_LL_Sleep(uint32_t ms);
void      LMAC_LL_WaitEvent(void);
void      LMAC_LL_TrigEvent(void);
uint32_t  LMAC_LL_GetTime(void);

void      LMAC_LL_StartTimer(lmac_timer_t *tmr);
void      LMAC_LL_StopTimer(void);

void      LMAC_LL_CreateTask(void (*func)(void *arg));

void      LMAC_LL_IntCB(void);

uint32_t  LMAC_LL_Rand(void);
#endif