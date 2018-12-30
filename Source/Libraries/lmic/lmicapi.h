#ifndef __LMICAPI_H__
#define __LMICAPI_H__
#include "DataType.h"
#include "lmic.h"

#pragma pack (4)
typedef struct {
    uint8_t valid;
    uint8_t active;
    uint8_t lclass;
    uint8_t netstat;
    uint8_t appeui[8];
    uint8_t appkey[16];
    uint8_t appskey[16];
    uint8_t nwkskey[16];
    uint8_t gappskey[16];
    uint8_t gnwkskey[16];
    uint32_t devaddr;
    uint32_t gaddr;
    uint8_t  datarate;
    uint32_t rx2freq;
    uint8_t  rx2dr;
    uint8_t  rxdelay;
    uint8_t  rxdroffset;
#ifdef CFG_eu868
    uint32_t ulfreqs[16];
    uint32_t dlfreqs[16];
    uint16_t channelMap;
    uint8_t  channelDrMap[16];
#endif
#ifdef CFG_zte470
    uint8_t  band;
    uint32_t txfreq;
    uint32_t rxfreq;
#if defined(CFG_zte470_support_bands)
    uint8_t join_band_seq[MAX_BANDS];
    band_t bands[MAX_BANDS];
    uint16_t channelDrMap[MAX_CHANNELS];
    uint16_t channelMap[MAX_CHANNELS>>4];
#else
    struct band_t bands[1];
    uint16_t channelDrMap[(u1_t)MAX_CHANNELS/MAX_BANDS];
    uint16_t channelMap[1];
#endif
#endif
} flash_param_t;
#pragma pack ()

extern flash_param_t flash_param;

void lmicapi_init(void);
int  lmicapi_getid(uint8_t *id);
int  lmicapi_setid(uint8_t *id);
int  lmicapi_setappkey(uint8_t *key);
int  lmicapi_getappkey(uint8_t *key);
int  lmicapi_setappeui(uint8_t *eui);
int  lmicapi_getappeui(uint8_t *eui);
void lmicapi_start(void);
void lmicapi_stop(void);
void lmicapi_reset(void);
int  lmicapi_send(uint8_t port, uint8_t *data, uint8_t len, uint8_t mode);
int  lmic_receive(uint8_t *data);
int  lmicapi_netstatus(void);
int  lmicapi_receive(uint8_t *data, uint8_t *len);
void lmicapi_saveparam(void);
void lmicapi_readparam(void);
void lmicapi_readid(void);

int  lmicapi_setbasefreq(uint32_t txfreq, uint32_t rxfreq);
int  lmicapi_setband(uint8_t band);
int  lmicapi_setsf(uint8_t sf);
int  lmicapi_setrx2param(uint32_t freq, uint8_t dr);
void lmicapi_getrx2param(uint32_t *freq, uint8_t *dr);
int  lmicapi_transstatus(void);
#endif
