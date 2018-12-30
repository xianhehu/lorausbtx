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
#include "radio.h"
#include "os.h"
#include "sx127x.h"
#include "gateway.h"
  
// Registers Mapping
#define RegFifo                                    0x00 // common
#define RegOpMode                                  0x01 // common
#define FSKRegBitrateMsb                           0x02
#define FSKRegBitrateLsb                           0x03
#define FSKRegFdevMsb                              0x04
#define FSKRegFdevLsb                              0x05
#define RegFrfMsb                                  0x06 // common
#define RegFrfMid                                  0x07 // common
#define RegFrfLsb                                  0x08 // common
#define RegPaConfig                                0x09 // common
#define RegPaRamp                                  0x0A // common
#define RegOcp                                     0x0B // common
#define RegLna                                     0x0C // common
#define FSKRegRxConfig                             0x0D
#define LORARegFifoAddrPtr                         0x0D
#define FSKRegRssiConfig                           0x0E
#define LORARegFifoTxBaseAddr                      0x0E
#define FSKRegRssiCollision                        0x0F
#define LORARegFifoRxBaseAddr                      0x0F
#define FSKRegRssiThresh                           0x10
#define LORARegFifoRxCurrentAddr                   0x10
#define FSKRegRssiValue                            0x11
#define LORARegIrqFlagsMask                        0x11
#define FSKRegRxBw                                 0x12
#define LORARegIrqFlags                            0x12
#define FSKRegAfcBw                                0x13
#define LORARegRxNbBytes                           0x13
#define FSKRegOokPeak                              0x14
#define LORARegRxHeaderCntValueMsb                 0x14
#define FSKRegOokFix                               0x15
#define LORARegRxHeaderCntValueLsb                 0x15
#define FSKRegOokAvg                               0x16
#define LORARegRxPacketCntValueMsb                 0x16
#define LORARegRxpacketCntValueLsb                 0x17
#define LORARegModemStat                           0x18
#define LORARegPktSnrValue                         0x19
#define FSKRegAfcFei                               0x1A
#define LORARegPktRssiValue                        0x1A
#define FSKRegAfcMsb                               0x1B
#define LORARegRssiValue                           0x1B
#define FSKRegAfcLsb                               0x1C
#define LORARegHopChannel                          0x1C
#define FSKRegFeiMsb                               0x1D
#define LORARegModemConfig1                        0x1D
#define FSKRegFeiLsb                               0x1E
#define LORARegModemConfig2                        0x1E
#define FSKRegPreambleDetect                       0x1F
#define LORARegSymbTimeoutLsb                      0x1F
#define FSKRegRxTimeout1                           0x20
#define LORARegPreambleMsb                         0x20
#define FSKRegRxTimeout2                           0x21
#define LORARegPreambleLsb                         0x21
#define FSKRegRxTimeout3                           0x22
#define LORARegPayloadLength                       0x22
#define FSKRegRxDelay                              0x23
#define LORARegPayloadMaxLength                    0x23
#define FSKRegOsc                                  0x24
#define LORARegHopPeriod                           0x24
#define FSKRegPreambleMsb                          0x25
#define LORARegFifoRxByteAddr                      0x25
#define LORARegModemConfig3                        0x26
#define FSKRegPreambleLsb                          0x26
#define FSKRegSyncConfig                           0x27
#define LORARegFeiMsb                              0x28
#define FSKRegSyncValue1                           0x28
#define LORAFeiMib                                 0x29
#define FSKRegSyncValue2                           0x29
#define LORARegFeiLsb                              0x2A
#define FSKRegSyncValue3                           0x2A
#define FSKRegSyncValue4                           0x2B
#define LORARegRssiWideband                        0x2C
#define FSKRegSyncValue5                           0x2C
#define FSKRegSyncValue6                           0x2D
#define FSKRegSyncValue7                           0x2E
#define FSKRegSyncValue8                           0x2F
#define FSKRegPacketConfig1                        0x30
#define FSKRegPacketConfig2                        0x31
#define LORARegDetectOptimize                      0x31
#define FSKRegPayloadLength                        0x32
#define FSKRegNodeAdrs                             0x33
#define LORARegInvertIQ                            0x33
#define FSKRegBroadcastAdrs                        0x34
#define FSKRegFifoThresh                           0x35
#define FSKRegSeqConfig1                           0x36
#define FSKRegSeqConfig2                           0x37
#define LORARegDetectionThreshold                  0x37
#define FSKRegTimerResol                           0x38
#define FSKRegTimer1Coef                           0x39
#define LORARegSyncWord                            0x39
#define FSKRegTimer2Coef                           0x3A
#define FSKRegImageCal                             0x3B
#define FSKRegTemp                                 0x3C
#define FSKRegLowBat                               0x3D
#define FSKRegIrqFlags1                            0x3E
#define FSKRegIrqFlags2                            0x3F
#define RegDioMapping1                             0x40 // common
#define RegDioMapping2                             0x41 // common
#define RegVersion                                 0x42 // common
// #define RegAgcRef                                  0x43 // common
// #define RegAgcThresh1                              0x44 // common
// #define RegAgcThresh2                              0x45 // common
// #define RegAgcThresh3                              0x46 // common
// #define RegPllHop                                  0x4B // common
// #define RegTcxo                                    0x58 // common
#define RegPaDac                                   0x4D // common
// #define RegPll                                     0x5C // common
// #define RegPllLowPn                                0x5E // common
// #define RegFormerTemp                              0x6C // common
// #define RegBitRateFrac                             0x70 // common
// ----------------------------------------
// spread factors and mode for RegModemConfig2
#define SX1272_MC2_FSK  0x00
#define SX1272_MC2_SF7  0x70
#define SX1272_MC2_SF8  0x80
#define SX1272_MC2_SF9  0x90
#define SX1272_MC2_SF10 0xA0
#define SX1272_MC2_SF11 0xB0
#define SX1272_MC2_SF12 0xC0
// bandwidth for RegModemConfig1
#define SX1272_MC1_BW_125  0x00
#define SX1272_MC1_BW_250  0x40
#define SX1272_MC1_BW_500  0x80
// coding rate for RegModemConfig1
#define SX1272_MC1_CR_4_5 0x08
#define SX1272_MC1_CR_4_6 0x10
#define SX1272_MC1_CR_4_7 0x18
#define SX1272_MC1_CR_4_8 0x20
#define SX1272_MC1_IMPLICIT_HEADER_MODE_ON 0x04 // required for receive
#define SX1272_MC1_RX_PAYLOAD_CRCON        0x02
#define SX1272_MC1_LOW_DATA_RATE_OPTIMIZE  0x01 // mandated for SF11 and SF12
// transmit power configuration for RegPaConfig
#define SX1272_PAC_PA_SELECT_PA_BOOST 0x80
#define SX1272_PAC_PA_SELECT_RFIO_PIN 0x00


// sx1276 RegModemConfig1
#define SX1276_MC1_BW_125                0x70
#define SX1276_MC1_BW_250                0x80
#define SX1276_MC1_BW_500                0x90
#define SX1276_MC1_CR_4_5            0x02
#define SX1276_MC1_CR_4_6            0x04
#define SX1276_MC1_CR_4_7            0x06
#define SX1276_MC1_CR_4_8            0x08

#define SX1276_MC1_IMPLICIT_HEADER_MODE_ON    0x01

// sx1276 RegModemConfig2
#define SX1276_MC2_RX_PAYLOAD_CRCON        0x04

// sx1276 RegModemConfig3
#define SX1276_MC3_LOW_DATA_RATE_OPTIMIZE  0x08
#define SX1276_MC3_AGCAUTO                 0x04

// preamble for lora networks (nibbles swapped)
static uint8_t LORA_MAC_PREAMBLE = 0x34;

#define RXLORA_RXMODE_RSSI_REG_MODEM_CONFIG1 0x0A
#ifdef CFG_sx1276_radio
#define RXLORA_RXMODE_RSSI_REG_MODEM_CONFIG2 0x70
#elif CFG_sx1272_radio
#define RXLORA_RXMODE_RSSI_REG_MODEM_CONFIG2 0x74
#endif



// ----------------------------------------
// Constants for radio registers
#define OPMODE_LORA      0x80
#define OPMODE_MASK      0x07
#define OPMODE_SLEEP     0x00
#define OPMODE_STANDBY   0x01
#define OPMODE_FSTX      0x02
#define OPMODE_TX        0x03
#define OPMODE_FSRX      0x04
#define OPMODE_RX        0x05
#define OPMODE_RX_SINGLE 0x06
#define OPMODE_CAD       0x07

// ----------------------------------------
// Bits masking the corresponding IRQs from the radio
#define IRQ_LORA_RXTOUT_MASK 0x80
#define IRQ_LORA_RXDONE_MASK 0x40
#define IRQ_LORA_CRCERR_MASK 0x20
#define IRQ_LORA_HEADER_MASK 0x10
#define IRQ_LORA_TXDONE_MASK 0x08
#define IRQ_LORA_CDDONE_MASK 0x04
#define IRQ_LORA_FHSSCH_MASK 0x02
#define IRQ_LORA_CDDETD_MASK 0x01

#define IRQ_FSK1_MODEREADY_MASK         0x80
#define IRQ_FSK1_RXREADY_MASK           0x40
#define IRQ_FSK1_TXREADY_MASK           0x20
#define IRQ_FSK1_PLLLOCK_MASK           0x10
#define IRQ_FSK1_RSSI_MASK              0x08
#define IRQ_FSK1_TIMEOUT_MASK           0x04
#define IRQ_FSK1_PREAMBLEDETECT_MASK    0x02
#define IRQ_FSK1_SYNCADDRESSMATCH_MASK  0x01
#define IRQ_FSK2_FIFOFULL_MASK          0x80
#define IRQ_FSK2_FIFOEMPTY_MASK         0x40
#define IRQ_FSK2_FIFOLEVEL_MASK         0x20
#define IRQ_FSK2_FIFOOVERRUN_MASK       0x10
#define IRQ_FSK2_PACKETSENT_MASK        0x08
#define IRQ_FSK2_PAYLOADREADY_MASK      0x04
#define IRQ_FSK2_CRCOK_MASK             0x02
#define IRQ_FSK2_LOWBAT_MASK            0x01

// ----------------------------------------
// DIO function mappings                D0D1D2D3
#define MAP_DIO0_LORA_RXDONE   0x00  // 00------
#define MAP_DIO0_LORA_TXDONE   0x40  // 01------
#define MAP_DIO1_LORA_RXTOUT   0x00  // --00----
#define MAP_DIO1_LORA_NOP      0x30  // --11----
#define MAP_DIO2_LORA_NOP      0xC0  // ----11--

#define MAP_DIO0_FSK_READY     0x00  // 00------ (packet sent / payload ready)
#define MAP_DIO1_FSK_NOP       0x30  // --11----
#define MAP_DIO2_FSK_TXNOP     0x04  // ----01--
#define MAP_DIO2_FSK_TIMEOUT   0x08  // ----10--


// FSK IMAGECAL defines
#define RF_IMAGECAL_AUTOIMAGECAL_MASK               0x7F
#define RF_IMAGECAL_AUTOIMAGECAL_ON                 0x80
#define RF_IMAGECAL_AUTOIMAGECAL_OFF                0x00  // Default

#define RF_IMAGECAL_IMAGECAL_MASK                   0xBF
#define RF_IMAGECAL_IMAGECAL_START                  0x40

#define RF_IMAGECAL_IMAGECAL_RUNNING                0x20
#define RF_IMAGECAL_IMAGECAL_DONE                   0x00  // Default

#ifdef CFG_sx1276_radio
#define LNA_RX_GAIN (0x20|0x00)
#elif CFG_sx1272_radio
#define LNA_RX_GAIN (0x20|0x03)
#else
#error Missing CFG_sx1272_radio/CFG_sx1276_radio
#endif

static uint8_t readReg (uint8_t ch, uint8_t reg) {
    uint8_t ret = 0;
    
    SX127X_Read(ch, reg, &ret, 1);
    
    return ret;
}

static void writeReg(uint8_t ch, uint8_t reg, uint8_t data, bool check)
{
    int trys = 0;
    
    SX127X_Write(ch, reg, &data, 1);
    
    if (!check)
        return;

    while(readReg(ch, reg) != data && trys++ < 1000) {
        SX127X_Write(ch, reg, &data, 1);
        
        os_delay(2);
    }

    if (trys >= 1000)
        ASSERT(false);
}

static void writeBuf (uint8_t ch, uint8_t reg, uint8_t* buf, uint8_t len) {
    SX127X_Write(ch, reg, buf, len);
}

static void readBuf (uint8_t ch, uint8_t reg, uint8_t* buf, uint8_t len) {
    SX127X_Read(ch, reg, buf, len);
}

void opmode (uint8_t ch, uint8_t mode) {
    writeReg(ch, RegOpMode, (readReg(ch, RegOpMode) & ~OPMODE_MASK) | mode, false);
}

static void opmodeLora(uint8_t ch) {
    uint8_t u = OPMODE_LORA;
    
    u |= 0x8;   // TBD: sx1276 high freq
    writeReg(ch, RegOpMode, u, false);
}

static void opmodeFSK(uint8_t ch) {
    uint8_t u = 0;

    u |= 0x8;   // TBD: sx1276 high freq
    writeReg(ch, RegOpMode, u, false);
}

// configure LoRa modem (cfg1, cfg2)
static void configLoraModem (uint8_t ch, radio_cfg_t* cfg) {
    uint8_t sf = cfg->sf;
    uint8_t mc1 = 0, mc2 = 0, mc3 = 0;

    switch (cfg->bw) {
    case 0: mc1 |= SX1276_MC1_BW_125; break;
    case 1: mc1 |= SX1276_MC1_BW_250; break;
    case 2: mc1 |= SX1276_MC1_BW_500; break;
    default:
        ASSERT(0);
    }
  
    switch( cfg->codr ) {
    case 0: mc1 |= SX1276_MC1_CR_4_5; break;
    case 1: mc1 |= SX1276_MC1_CR_4_6; break;
    case 2: mc1 |= SX1276_MC1_CR_4_7; break;
    case 3: mc1 |= SX1276_MC1_CR_4_8; break;
    default:
        ASSERT(0);
    }

#if 0
    //if (getIh(LMIC.rps)) {
        mc1 |= SX1276_MC1_IMPLICIT_HEADER_MODE_ON;
        writeReg(LORARegPayloadLength, getIh(LMIC.rps)); // required length
    //}
#endif

    // set ModemConfig1
    writeReg(ch, LORARegModemConfig1, mc1, true);

    mc2 = (SX1272_MC2_SF7 + ((sf-7)<<4));

    if (!cfg->ncrc) {
        mc2 |= SX1276_MC2_RX_PAYLOAD_CRCON;
    }

    writeReg(ch, LORARegModemConfig2, mc2, true);

    //mc3 = SX1276_MC3_AGCAUTO;
    if (sf == 11 || sf == 12) {
        mc3 |= SX1276_MC3_LOW_DATA_RATE_OPTIMIZE;
    }

    writeReg(ch, LORARegModemConfig3, mc3, true);
}

static void writeFreqReg(uint8_t ch, uint64_t freq)
{
    uint64_t frf = (freq << 19) / 32000000;
    
    writeReg(ch, RegFrfMsb, (uint8_t)(frf>>16), true);
    writeReg(ch, RegFrfMid, (uint8_t)(frf>> 8), true);
    writeReg(ch, RegFrfLsb, (uint8_t)(frf>> 0), true);
}

// configure the radio power
static void configPower (uint8_t ch, int8_t pw) {
    uint8_t reg_padac    = 0;
    uint8_t reg_paconfig = 0;

    if(pw > 14){
        reg_padac = 0x87;
    }
    else{
        reg_padac = 0x84;
    }
  
    writeReg(ch, RegPaDac, reg_padac, true);
    reg_paconfig = 0x4F; // Set to its default value
  
    if (reg_padac == 0x87){ // > 14dbm
        if (pw >= 20){
            pw = 20;
            reg_paconfig = 0xFF; // changed by solar
        }
        else{
            reg_paconfig = ((reg_paconfig & 0x8F) | 0x70);                                                                  // MaxPower
            reg_paconfig = ((reg_paconfig & 0xF0) | (uint8_t)((uint16_t)(pw - 5) & 0x0F) | 0x80); // OutputPower
        }
    }
    else{ // < 14dbm
        if (pw < 2){
            pw = 2;
        }
        reg_paconfig = ((reg_paconfig & 0x8F) | 0x70);                                                                  // MaxPower
        reg_paconfig = ((reg_paconfig & 0xF0) | (uint8_t)((uint16_t)(pw - 2) & 0x0F) | 0x80); // OutputPower
    }

    writeReg(ch, RegPaConfig, reg_paconfig, true);
}

//static timer_t timer_txrxtimeout;
void txlora(radio_cfg_t* cfg)
{
    uint8_t ch = cfg->rfch;
    
    // select LoRa modem (from sleep mode)
    writeReg(ch, RegOpMode, OPMODE_LORA, false);
    
    opmodeLora(ch);
    
    ASSERT((readReg(ch, RegOpMode) & OPMODE_LORA) != 0);

    // enter standby mode (required for FIFO loading))
    opmode(ch, OPMODE_STANDBY);
    // configure LoRa modem (cfg1, cfg2)
    configLoraModem(ch, cfg);
    // configure frequency
    //configChannel();
    writeFreqReg(ch, cfg->freq);
    
    // configure output power
    writeReg(ch, RegPaRamp, (readReg(ch, RegPaRamp) & 0xF0) | 0x08, true); // set PA ramp-up time 50 uSec
    configPower(ch, cfg->powe);
    // set sync word
    writeReg(ch, LORARegSyncWord, LORA_MAC_PREAMBLE, true);
    
    if (cfg->ipol)
        writeReg(ch, LORARegInvertIQ, readReg(ch, LORARegInvertIQ)&0xfe, true);
    else
        writeReg(ch, LORARegInvertIQ, readReg(ch, LORARegInvertIQ)|1, true);
    
    // set the IRQ mapping DIO0=TxDone DIO1=NOP DIO2=NOP
    writeReg(ch, RegDioMapping1, MAP_DIO0_LORA_TXDONE, true);
    // clear all radio IRQ flags
    writeReg(ch, LORARegIrqFlags, 0xFF, false);
    // mask all IRQs but TxDone
    writeReg(ch, LORARegIrqFlagsMask, ~IRQ_LORA_TXDONE_MASK, true);
    // initialize the payload size and address pointers
    writeReg(ch, LORARegFifoTxBaseAddr, 0x00, false);
    writeReg(ch, LORARegFifoAddrPtr, 0x00, false);
    writeReg(ch, LORARegPayloadLength, cfg->size, true);

    // download buffer to the radio FIFO
    writeBuf(ch, RegFifo, cfg->data, cfg->size);

    // enable antenna switch for TX
    //hal_pin_rxtx(1);

    /* now we actually start the transmission */
    opmode(ch, OPMODE_TX);
 
    //os_delay(10);

#if 0
    //OSSemSet(&irq_sem, 0, &err);
    //SX127X_EN_DIO_INT();

    /* wait for txdone or timeout interrupt,
    timeout must be over tx time so that tx complete successfully,
    max payload need transmit 3s in LoraMAC specification */
    //OSSemPend(&irq_sem, ms2osticks(3000), OS_OPT_PEND_BLOCKING, &ts, &err);
    
    if(err != OS_ERR_NONE) {
        toutFlag = 1;      /* timout flag set to 1 */
        LMIC.txend = os_getTime();
    }
#else
    SX127X_Wait(ch, 3000);

    opmode(ch, OPMODE_SLEEP);
    
    writeReg(ch, RegOpMode, 0, true);
#endif
}

static uint16_t flag = 0;

static void txfsk (radio_cfg_t* cfg)
{
    uint8_t ch = cfg->rfch;

    // select FSK modem (from sleep mode)
    writeReg(ch, RegOpMode, 8, true); // FSK, BT=0.5
    //ASSERT(readReg(ch, RegOpMode) == 8);
    // enter standby mode (required for FIFO loading))
    while ((readReg(ch, RegOpMode)&1) == 0)
        opmode(ch, OPMODE_STANDBY);
    // set bitrate
    writeReg(ch, FSKRegBitrateMsb, 0x02, true); // 50kbps
    writeReg(ch, FSKRegBitrateLsb, 0x80, true);
    // set frequency deviation
    writeReg(ch, FSKRegFdevMsb, 0x01, true); // +/- 25kHz
    writeReg(ch, FSKRegFdevLsb, 0x99, true);
    // frame and packet handler settings
    writeReg(ch, FSKRegPreambleMsb, 0x00, true);
    writeReg(ch, FSKRegPreambleLsb, 0x05, true);
    writeReg(ch, FSKRegSyncConfig, 0x12, true);
    writeReg(ch, FSKRegPacketConfig1, 0x90, true); // packet whitening crcon
    writeReg(ch, FSKRegPacketConfig2, 0x40, true);
    writeReg(ch, FSKRegSyncValue1, 0xC1, true);
    writeReg(ch, FSKRegSyncValue2, 0x94, true);
    writeReg(ch, FSKRegSyncValue3, 0xC1, true);
    // configure frequency
    //configChannel(ch);
    writeFreqReg(ch, cfg->freq);
    
    // configure output power
    configPower(ch, cfg->powe);

    // set the IRQ mapping DIO0=PacketSent DIO1=NOP DIO2=NOP
    writeReg(ch, RegDioMapping1, MAP_DIO0_FSK_READY|MAP_DIO1_FSK_NOP|MAP_DIO2_FSK_TXNOP, true);

    // initialize the payload size and address pointers
    writeReg(ch, FSKRegPayloadLength, cfg->size+1, true); // (insert length byte into payload))

    writeReg(ch, FSKRegFifoThresh, 0x80, true);

    opmode(ch, OPMODE_TX);

    // download length byte and buffer to the radio FIFO
    writeReg(ch, RegFifo, cfg->size, false);
    writeBuf(ch, RegFifo, cfg->data, cfg->size);

    /* wait for rxdone or timeout interrupt */
    SX127X_Wait(ch, 1000);

    flag = readReg(ch, 1);
    flag = readReg(ch, FSKRegIrqFlags1);

    opmode(ch, OPMODE_SLEEP);
    writeReg(ch, RegOpMode, 0, true);
}

void RADIO_StartTx(radio_cfg_t* cfg)
{
    if (cfg->lora)
        txlora(cfg);
    else
        txfsk(cfg);
}
