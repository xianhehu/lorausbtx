/*******************************************************************************
 * Copyright (c) 2014-2015 IBM Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    IBM Zurich Research Lab - initial API, implementation and documentation
 *******************************************************************************/

#include "lmic.h"
#include "lmiclowlevelapi.h"
#include <stdarg.h>
#include "Drv_Usart.h"
#include "oslmic.h"
#include "radio_p2p.h"

// RADIO STATE
// (initialized by radio_init(), used by radio_rand1())
static u1_t randbuf[16];
static u1_t currentmode = 0;
static u1_t txrxstate = 0;
static radio_txrx_cfg_t cfg;

#ifdef RADIO_DEBUG
static u4_t tx_time;
static u4_t rx_time;
#endif

enum { RXMODE_SINGLE, RXMODE_SCAN, RXMODE_RSSI };

void RADIO_DioIsrCB(u1_t state, u1_t datalen)
{
    TIMER_TxRxTimeoutStop();
    
    if (datalen > 0) {
        LMIC.dataLen = datalen;
    }
    if (state == DIO_ISR_STATE_RXDONE) {
        RADIO_ReadPacketRssi(&LMIC.rssi, &LMIC.snr);
    }
    
    radio_irq_handler(state);
}

void RadioDebug(char *fmt,...)
{
#ifdef RADIO_DEBUG
    static char buf[200];
    int len = 0;
    va_list varg;
    va_start(varg, fmt);
    len = vsnprintf(buf, sizeof(buf)-1, fmt, varg);
    va_end(varg);
    Drv_Usart_Write(buf, len, PORT_USART1);
#endif
}

static void txfsk () {

}

static void txlora () {
    // select LoRa modem (from sleep mode)
    //writeReg(RegOpMode, OPMODE_LORA);
    currentmode = RADIO_TXRX_CFG_MODE_LORA;
    txrxstate = 0;
    //LMIC.freq = 482300000;
    memset(&cfg, 0, sizeof(cfg));
    cfg.mode = RADIO_TXRX_CFG_MODE_LORA;
    cfg.pow = LMIC.txpow;
    cfg.freq = LMIC.freq;
    cfg.sf = getSf(LMIC.rps)+6;
    cfg.low_speed = 1;
    cfg.bw = getBw(LMIC.rps);
    cfg.crc = getNocrc(LMIC.rps) == 0?1:0;
    cfg.coderate = getCr(LMIC.rps);
    cfg.invert = 0;
    cfg.premble = 8;
    cfg.syncword = 0x34;
#if 0
    cfg.freq=470300000;
    cfg.sf=7;
#endif
    TIMER_TxRxTimeoutStart();
    RadioDebug("tx freq:%d,sf:%d,bw:%d,pow:%d,seq:%d\r\n", 
               cfg.freq, cfg.sf, cfg.bw, cfg.pow, LMIC.seqnoUp);
    RADIO_StartSend(cfg, LMIC.frame, LMIC.dataLen);
    /*  */
}

// start transmitter (buf=LMIC.frame, len=LMIC.dataLen)
static void starttx () {
    if(getSf(LMIC.rps) == FSK) { // FSK modem
        txfsk();
    } else { // LoRa modem
        txlora();
    }
    // the radio will go back to STANDBY mode as soon as the TX is finished
    // the corresponding IRQ will inform us about completion.
}

// start LoRa receiver (time=LMIC.rxtime, timeout=LMIC.rxsyms, result=LMIC.frame[LMIC.dataLen])
static void rxlora (u1_t rxmode) {
    // select LoRa modem (from sleep mode)
    currentmode = RADIO_TXRX_CFG_MODE_LORA;
    txrxstate = 1;
    //LMIC.freq = 482300000;
    memset(&cfg, 0, sizeof(cfg));
    cfg.mode = RADIO_TXRX_CFG_MODE_LORA;
    cfg.freq = LMIC.freq;
    cfg.sf = getSf(LMIC.rps)+6;
    cfg.low_speed = 1;
    cfg.bw = getBw(LMIC.rps);
    cfg.crc = getNocrc(LMIC.rps) == 0?1:0;
    cfg.coderate = getCr(LMIC.rps);
    cfg.invert = 1;
    cfg.premble = 8;
    cfg.syncword = 0x34;
#if 1
    cfg.freq=470300000;
    cfg.sf=7;
#endif
    //rxmode = RXMODE_SCAN;
    if (rxmode == RXMODE_SCAN) {
        cfg.rxmode = RADIO_TXRX_CFG_RXMODE_SCAN;
    }
    else {
        cfg.rxmode = RADIO_TXRX_CFG_RXMODE_SINGLE;
        //cfg.rxmode = RADIO_TXRX_CFG_RXMODE_SCAN;
        cfg.symtimeout = LMIC.rxsyms;
    }

    TIMER_TxRxTimeoutStart();
    os_waitUntil(LMIC.rxtime);
#ifdef RADIO_DEBUG
    rx_time = os_getTime();
#endif
    RADIO_StartReceive(cfg, LMIC.frame, MAX_LEN_FRAME);
}

static void rxfsk (u1_t rxmode) {

}

static void startrx (u1_t rxmode) {
    if(getSf(LMIC.rps) == FSK) { // FSK modem
        rxfsk(rxmode);
    } else { // LoRa modem
        rxlora(rxmode);
    }
    // the radio will go back to STANDBY mode as soon as the RX is finished
    // or timed out, and the corresponding IRQ will inform us about completion.
}

// return next random byte derived from seed buffer
// (buf[0] holds index of next byte to be returned)
u1_t radio_rand1 () {
    u1_t i = randbuf[0];
    if( i==16 ) {
        os_aes(AES_ENC, randbuf, 16); // encrypt seed with any key
        i = 0;
    }
    u1_t v = randbuf[i++];
    randbuf[0] = i;
    return v;
}

u1_t radio_rssi () {
    s2_t rssi;
    s1_t snr;
    RADIO_ReadRssi(&rssi, &snr);
    return -rssi;
}

static const u2_t LORA_RXDONE_FIXUP[] = {
    [FSK]  =     us2osticks(0), // (   0 ticks)
    [SF7]  =     us2osticks(0), // (   0 ticks)
    [SF8]  =  us2osticks(1648), // (  54 ticks)
    [SF9]  =  us2osticks(3265), // ( 107 ticks)
    [SF10] =  us2osticks(7049), // ( 231 ticks)
    [SF11] = us2osticks(13641), // ( 447 ticks)
    [SF12] = us2osticks(31189), // (1022 ticks)
};

// called by hal ext IRQ handler
// (radio goes to stanby mode after tx/rx operations)
void radio_irq_handler (u1_t state) {
    ostime_t now = os_getTime();
    if (currentmode == RADIO_TXRX_CFG_MODE_LORA) { // LORA modem
        if( state == DIO_ISR_STATE_TXDONE ) {
            // save exact tx time
            LMIC.txend = now - us2osticks(43); // TXDONE FIXUP
#ifdef RADIO_DEBUG
            tx_time = os_getTime();
#endif
        } else if( state == DIO_ISR_STATE_RXDONE ) {
            // save exact rx time
            if(getBw(LMIC.rps) == BW125) {
                now -= LORA_RXDONE_FIXUP[getSf(LMIC.rps)];
            }
            LMIC.rxtime = now;
#ifdef RADIO_DEBUG
            RadioDebug("rx freq:%d,sf:%d,bw:%d,rxdelay:%d,recieved:%d\r\n", 
               cfg.freq, cfg.sf, cfg.bw, rx_time-tx_time, LMIC.dataLen);
#endif
        } else if( state == DIO_ISR_STATE_RXTIMEOUT ) {
            // indicate timeout
            LMIC.dataLen = 0;
#ifdef RADIO_DEBUG
            RadioDebug("rx freq:%d,sf:%d,bw:%d,rxdelay:%d,rxtimeout:%d\r\n", 
               cfg.freq, cfg.sf, cfg.bw, rx_time-tx_time, os_getTime()-rx_time);
#endif
        }
    } else { // FSK modem
        if( state == DIO_ISR_STATE_TXDONE ) {
            // save exact tx time
            LMIC.txend = now;
        } else if( state == DIO_ISR_STATE_RXDONE ) {
            // save exact rx time
            LMIC.rxtime = now;
        } else if( state == DIO_ISR_STATE_RXTIMEOUT ) {
            // indicate timeout
            LMIC.dataLen = 0;
        } else {
            while(1);
        }
    }
    // run os job (use preset func ptr)
    os_setCallback(&LMIC.osjob, LMIC.osjob.func);
}

void os_radio (u1_t mode) {
    CPU_DisableInt();
    switch (mode) {
      case RADIO_RST:
        // put radio to sleep
        RADIO_Sleep();
        break;

      case RADIO_TX:
        // transmit frame now
        starttx(); // buf=LMIC.frame, len=LMIC.dataLen
        break;
      
      case RADIO_RX:
        // receive frame now (exactly at rxtime)
        startrx(RXMODE_SINGLE); // buf=LMIC.frame, time=LMIC.rxtime, timeout=LMIC.rxsyms
        break;

      case RADIO_RXON:
        // start scanning for beacon now
        startrx(RXMODE_SCAN); // buf=LMIC.frame
        break;
    }
    CPU_EnableInt();
}

void lmic_timeoutcb(void)
{
    CPU_DisableInt();
    radio_irq_handler(txrxstate==0?DIO_ISR_STATE_TXDONE:DIO_ISR_STATE_RXTIMEOUT);
    CPU_EnableInt();
}

void txtest (radio_p2p_cfg_t *p2pcfg) {
    memset(&cfg, 0, sizeof(cfg));
    cfg.mode = p2pcfg->com.lora?RADIO_TXRX_CFG_MODE_LORA:RADIO_TXRX_CFG_MODE_FSK;
    cfg.freq = p2pcfg->com.freq;
    cfg.sf = p2pcfg->com.sf;
    cfg.low_speed = 1;
    cfg.bw = p2pcfg->com.bw;
    cfg.crc = p2pcfg->com.crc;
    cfg.coderate = p2pcfg->com.codr;
    cfg.invert = p2pcfg->com.ipol;
    cfg.premble = p2pcfg->com.preamble;
    cfg.syncword = p2pcfg->com.syncword;
    cfg.pow = p2pcfg->tx.pw;

    //LMIC.frame[0]=fcnt;
    memcpy(LMIC.frame, p2pcfg->tx.buf, p2pcfg->tx.len);
    RadioDebug("tx freq:%d,sf:%d,bw:%d,pow:%d,seq:%d\r\n", 
               cfg.freq, cfg.sf, cfg.bw, cfg.pow, LMIC.seqnoUp);
    RADIO_StartSend(cfg, LMIC.frame, p2pcfg->tx.len);
}

void rxtest (radio_p2p_cfg_t *p2pcfg) {
    currentmode = RADIO_TXRX_CFG_MODE_LORA;
    txrxstate = 1;

    memset(&cfg, 0, sizeof(cfg));
    cfg.mode = p2pcfg->com.lora?RADIO_TXRX_CFG_MODE_LORA:RADIO_TXRX_CFG_MODE_FSK;
    cfg.freq = p2pcfg->com.freq;
    cfg.sf = p2pcfg->com.sf;
    cfg.low_speed = 1;
    cfg.bw = p2pcfg->com.bw;
    cfg.crc = p2pcfg->com.crc;
    cfg.coderate = p2pcfg->com.codr;
    cfg.invert = p2pcfg->com.ipol;
    cfg.premble = p2pcfg->com.preamble;
    cfg.syncword = p2pcfg->com.syncword;
    cfg.rxmode = RADIO_TXRX_CFG_RXMODE_SCAN;

    RADIO_StartReceive(cfg, LMIC.frame, MAX_LEN_FRAME);
}
