#ifndef _RADIO_H_
#define _RADIO_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
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
    uint8_t  *data;
} radio_cfg_t;

void RADIO_StartTx(radio_cfg_t* cfg);

#endif