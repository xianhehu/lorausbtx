#ifndef __LMIC_LOWLEVEL_API_H__
#define __LMIC_LOWLEVEL_API_H__

#include <stdint.h>

typedef struct {
    uint8_t mode;
#define RADIO_TXRX_CFG_MODE_FSK         0
#define RADIO_TXRX_CFG_MODE_LORA        1
    uint8_t pow;
    uint32_t freq;
    uint8_t sf;
    uint8_t low_speed;
    uint8_t bw;
#define RADIO_TXRX_CFG_BW_125K          0
#define RADIO_TXRX_CFG_BW_250K          1
#define RADIO_TXRX_CFG_BW_500K          2
    uint8_t crc;
#define RADIO_TXRX_CFG_CRC_OFF          0
#define RADIO_TXRX_CFG_CRC_ON           1
    uint8_t coderate;
#define RADIO_TXRX_CFG_CR_4_5           0
#define RADIO_TXRX_CFG_CR_4_6           1
#define RADIO_TXRX_CFG_CR_4_7           2
#define RADIO_TXRX_CFG_CR_4_8           3

    uint8_t invert;
    uint8_t syncword;
    uint8_t premble;
    uint8_t rxmode;
#define RADIO_TXRX_CFG_RXMODE_SINGLE    0
#define RADIO_TXRX_CFG_RXMODE_SCAN      1
    
    uint8_t bitrate;
    uint8_t symtimeout;
} radio_txrx_cfg_t;


void RADIO_StartSend(radio_txrx_cfg_t cfg, uint8_t *data, uint8_t len);
void RADIO_StartReceive(radio_txrx_cfg_t cfg, uint8_t *data, uint8_t size);
void RADIO_Sleep(void);
void RADIO_ReadPacketRssi(int16_t *rssi, int8_t *snr);
void RADIO_ReadRssi(int16_t *rssi, int8_t *snr);
void RADIO_DioIsrCB(uint8_t state, uint8_t datalen);
void CPU_DisableInt(void);
void CPU_EnableInt(void);

void TIMER_TxRxTimeoutCreate(uint32_t time_ms);
void TIMER_TxRxTimeoutStart(void);
void TIMER_TxRxTimeoutStop(void);

extern void lmic_timeoutcb(void);

#define DIO_ISR_STATE_RXTIMEOUT          0
#define DIO_ISR_STATE_RXDONE             1
#define DIO_ISR_STATE_TXDONE             2

#endif