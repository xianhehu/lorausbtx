#ifndef __RADIO_P2P_H__
#define __RADIO_P2P_H__

typedef struct {
    uint8_t  lora;
    uint8_t  syncword;
    uint8_t  sw[6];
    uint8_t  crc;
    uint8_t  bw;
    uint8_t  sf;
    uint8_t  codr;
    uint8_t  ipol;
    uint8_t  bitrate;
    uint16_t preamble;
    uint32_t freq;
} radio_p2p_cfg_com_t;

typedef struct {
    uint8_t  pw;
    uint8_t  len;
    uint32_t time;
    uint8_t  *buf;
} radio_p2p_cfg_tx_t;

typedef struct {
    uint32_t tout;
    uint32_t time;
    uint8_t  statistic;
    uint8_t  size;
    uint8_t  *buf;
} radio_p2p_cfg_rx_t;

typedef struct {
    radio_p2p_cfg_com_t com;
    radio_p2p_cfg_tx_t  tx;
    radio_p2p_cfg_rx_t  rx;
} radio_p2p_cfg_t;


void txtest (radio_p2p_cfg_t *p2pcfg);
void rxtest (radio_p2p_cfg_t *p2pcfg);
radio_p2p_cfg_t* P2P_GetCfg(void);

#endif