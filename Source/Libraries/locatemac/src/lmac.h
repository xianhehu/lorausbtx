#ifndef __LMAC_H__
#define __LMAC_H__

#include "DataType.h"
#include "lmac_cfg.h"
#include "lmacapi.h"


#define    LMAC_FRAME_MIN_LEN_DATA                  5
#define    LAMC_FRAME_MIN_LEN_ADV                   10

#pragma pack(1)
typedef struct {
    uint16_t active:1;
    uint16_t num:6;
    uint16_t reserv:1;
    uint16_t rate:4;
    uint16_t sf:4;
} lmac_channel_t;
#pragma pack()

typedef void (*lmac_task_func_t)(void *arg);

typedef struct {
    lmac_task_func_t func;
    void *           arg;
    uint32_t         time;
} lmac_task_time_t;

typedef struct {
    lmac_task_func_t func;
    void *           arg;
    bool             enable;
} lmac_task_imme_t;

typedef struct {
    /* common */
    uint32_t devid;
    uint8_t  state;
    uint32_t time_err_rate;
    uint8_t  rssi;
    int8_t   snr;

    /* scan configure */
    uint32_t scan_peroid;
    uint32_t scan_interval;
    
    /* channel adverting */
    uint8_t  adv_channel;
    uint8_t  adv_period;
    uint32_t adv_listen;
    uint8_t  adv_txslot;
    uint32_t adv_txtime;
    uint16_t adv_bsid;
    uint8_t  sync_times;

    /* data adverting */
    uint8_t  adv_recvslot;
    uint8_t  adv_recvchannel;
    uint8_t  adv_recvsf;

    /* channel configure */
    uint8_t  channel_num;
    uint8_t  channel_sf;
    uint16_t channel_solts;
    uint16_t slot_time;
    
    /* send configure */
    uint8_t  send_hassyncdata;
    uint8_t  send_confirm;
    uint8_t  send_acked;
    uint8_t  send_slot;
    uint8_t  send_len;
    uint8_t  send_buf[LMAC_FRAME_LEN_MAX];
    
    /* receive configure */
    uint8_t  recv_slot;
    uint8_t  recv_len;
    uint8_t  recv_buf[LMAC_FRAME_LEN_MAX];
} lmac_t;

typedef struct {
    /* scan */
    uint32_t scan_success;
    uint32_t scan_failed;
    /* adv sync */
    uint32_t adv_success;
    uint32_t adv_failed;
    /* sync send */
    uint32_t send_failed;
    uint32_t send_success;
    uint32_t send_csma;
} lmac_statistic_t;

#define GET_ARRY_SIZE(arry)  (sizeof(arry)/sizeof(arry[0]))

#endif