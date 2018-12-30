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
#include "lmiclowlevelapi.h"
#include "osal.h"
#include "Drv_SX127x.h"

// RADIO STATE
// (initialized by radio_init(), used by radio_rand1())

#ifdef CFG_sx1276_radio
#define LNA_RX_GAIN (0x20|0x1)
#elif CFG_sx1272_radio
#define LNA_RX_GAIN (0x20|0x03)
#else
#error Missing CFG_sx1272_radio/CFG_sx1276_radio
#endif
   
//static timer_t timer_txrxtimeout;
static osal_timer_t timer_txrxtimeout;

void RADIO_StartSend(radio_txrx_cfg_t cfg, uint8_t *data, uint8_t len)
{
#if 1
    strSX127x[SX127x_CHIP1].radio.modulation = RADIO_MTYPE_LORA;
    strSX127x[SX127x_CHIP1].radio.send_power = cfg.pow;
    strSX127x[SX127x_CHIP1].radio.freq = cfg.freq;
    strSX127x[SX127x_CHIP1].radio.bandwidth = 125000;
    strSX127x[SX127x_CHIP1].flag.bits.sf = cfg.sf;
    strSX127x[SX127x_CHIP1].flag.bits.low_speed = cfg.low_speed;
    strSX127x[SX127x_CHIP1].flag.bits.coderate = cfg.coderate+1;
    strSX127x[SX127x_CHIP1].flag.bits.tx_IQInvert = cfg.invert;
    strSX127x[SX127x_CHIP1].radio.flag.bits.isCRC = cfg.crc;
    strSX127x[SX127x_CHIP1].radio.syncword = cfg.syncword;
    
    strSX127x[SX127x_CHIP1].buff = data;
    SX127x_IOCtl_IRQClear();
    Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_PARAM);
    Drv_SX127x_Radio_Write(SX127x_CHIP1,data, len);
    Drv_SX127x_Radio_Send(SX127x_CHIP1, 3000);
#else
    
#endif
}

void RADIO_StartReceive(radio_txrx_cfg_t cfg, uint8_t *data, uint8_t size)
{
#if 1
    strSX127x[SX127x_CHIP1].radio.modulation = RADIO_MTYPE_LORA;
    strSX127x[SX127x_CHIP1].radio.freq = cfg.freq;
    strSX127x[SX127x_CHIP1].radio.bandwidth = 125000;
    strSX127x[SX127x_CHIP1].flag.bits.sf = cfg.sf;
    strSX127x[SX127x_CHIP1].flag.bits.low_speed = cfg.low_speed;
    strSX127x[SX127x_CHIP1].flag.bits.coderate = cfg.coderate+1;
    strSX127x[SX127x_CHIP1].flag.bits.rx_IQInvert = cfg.invert;
    strSX127x[SX127x_CHIP1].radio.flag.bits.isCRC = cfg.crc;
    strSX127x[SX127x_CHIP1].radio.syncword = cfg.syncword;
    strSX127x[SX127x_CHIP1].radio.symtimeout = cfg.symtimeout;
    strSX127x[SX127x_CHIP1].flag.bits.rxContinous = cfg.rxmode;
    
    strSX127x[SX127x_CHIP1].buff = data;
    strSX127x[SX127x_CHIP1].size = size;
    SX127x_IOCtl_IRQClear();
    //TASK_RadioCheckStart();
    Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_PARAM);
    Drv_SX127x_Radio_Listen(SX127x_CHIP1, 3000, true);
#else
    
#endif
}

void RADIO_Sleep(void)
{
}

void RADIO_ReadPacketRssi(int16_t *rssi, int8_t *snr)
{
    *rssi = strSX127x[SX127x_CHIP1].radio.RSSI;
    *snr  = strSX127x[SX127x_CHIP1].radio.SNR;
}

void RADIO_Reset(void)
{
    Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_RESET);
}

void CPU_DisableInt(void)
{
}

void CPU_EnableInt(void)
{
}

/*创建发送接收超时计数器
*/
void TIMER_TxRxTimeoutCreate(uint32_t time_ms)
{
    timer_txrxtimeout.mode=0;
    timer_txrxtimeout.period=time_ms;
    timer_txrxtimeout.func=lmic_timeoutcb;
    osal_timerCreate(&timer_txrxtimeout);
}

void TIMER_TxRxTimeoutStart(void)
{
    osal_timerStart(&timer_txrxtimeout);
}

void TIMER_TxRxTimeoutStop(void)
{
    osal_timerStop(&timer_txrxtimeout);
}
