#ifndef __LMAC_API_H__
#define __LMAC_API_H__

#include "DataType.h"

typedef enum {
    LMAC_STATE_NOSCAN=0,
    LMAC_STATE_SCANING,
    LMAC_STATE_NOJOIN,
    LMAC_STATE_JOINSEND,
    LMAC_STATE_JOINRECV,
    LMAC_STATE_JOINED,
    LMAC_STATE_SYNCING,
    LMAC_STATE_SYNCED,
    LMAC_STATE_DATASEND,
    LMAC_STATE_DATARECV,
} lmac_state_t;

typedef struct {
    uint32_t devid; /* mote deveui */
    uint16_t bsid;  /* basestation id */
    /* time slot */
    uint32_t slots;
    uint32_t slot;
    uint32_t time_error_rate;
    /* scan info */
    uint32_t period;
} lmac_cfg_t;

typedef struct {
    uint8_t fcnt;
    uint8_t *buf;
    uint8_t len;
    bool    confirm;
} uplink_data_t;

typedef struct {
    uint8_t *buf;
    uint8_t size;
    uint8_t fcnt;
    int16_t rssi;
    int8_t  snr;
} dnlink_data_t;

void LMAC_Init(void);
void LMAC_Reset(void);
void LMAC_GetConfig(lmac_cfg_t *cfg);
void LMAC_SetConfig(lmac_cfg_t *cfg);

int  LMAC_GetNetState(void);
int  LMAC_SetStateCB(uint8_t state, void(*func)(void));
int  LMAC_StartScan(void);
int  LMAC_StartSync(void);
int  LMAC_SyncSend(uplink_data_t *data);
int  LMAC_GetSyncSendResult(void);
int  LMAC_ReadDlData(dnlink_data_t *data);

#endif