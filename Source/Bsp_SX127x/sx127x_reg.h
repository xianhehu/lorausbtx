/*
********************************************************************************
                                 Header File

                    (c) Copyright 2015~2025; Embedded Studio
         All rights reserved.  Protected by international copyright laws!

File        :  sx127x_reg.h
By          :  Season
Version     :  V0.1.0
Createdate  :  2016-01-31
----------------------------- Liscensing terms ---------------------------------

********************************************************************************
*/

#ifndef SX127X_REG_H
#define SX127X_REG_H

/****
Include
****/

/****
Defines
****/
// --- Public reg define ---                      - Default - Description -   
#define SX127X_PUB_RegFifo                  (0x00) // 0x00, FIFO read/write access, rw
#define SX127X_PUB_RegOpMode                (0x01) // 0x01, Operating mode & LoRa/FSK selection
#define SX127X_PUB_RegFrfMsb                (0x06) // 0x6C, RF Carrier Frequency, Most Significant Bits
#define SX127X_PUB_RegFrfMid                (0x07) // 0x80, RF Carrier Frequency, Intermediate Bits
#define SX127X_PUB_RegFrfLsb                (0x08) // 0x00, RF Carrier Frequency, Least Significant Bits
#define SX127X_PUB_RegPaConfig              (0x09) // 0x4F, PA selection and Output Power control
#define SX127X_PUB_RegPaRamp                (0x0A) // 0x09, Control of PA ramp time, low phase noise PLL
#define SX127X_PUB_RegOcp                   (0x0B) // 0x2B, Over Current Protection control
#define SX127X_PUB_RegLna                   (0x0C) // 0x20, LNA settings

// --- Public Reg ---
#define SX127X_PUB_RegDioMapping1           (0x40) // 0x00, Mapping of pins DIO0 to DIO3
#define SX127X_PUB_RegDioMapping2           (0x41) // 0x00, Mapping of pins DIO4 to DIO5, ClkOut freq
#define SX127X_PUB_RegVersion               (0x42) // 0x12, Semtech ID ralating the silicon revision

#define SX127X_PUB_RegTcxo                  (0x4B) // 0x09, TCXO or XTAL input setting
#define SX127X_PUB_RegPaDac                 (0x4D) // 0x84, Higher power settings of the PA
#define SX127X_PUB_RegFormerTemp            (0x5B) //     , Stored temperature during the former IQ Calibration

#define SX127X_PUB_RegAgcRef                (0x61) // 0x13, Adjustment of the AGC thresholds
#define SX127X_PUB_RegAgcThresh1            (0x62) // 0x0E, Adjustment of the AGC thresholds    
#define SX127X_PUB_RegAgcThresh2            (0x63) // 0x5B, Adjustment of the AGC thresholds  
#define SX127X_PUB_RegAgcThresh3            (0x64) // 0xDB, Adjustment of the AGC thresholds  
#define SX127X_PUB_RegPll                   (0x70) // 0xD0, Control of the PLL bandwidth
// -------------------------

// --- LoRa Mode ---
#define SX127X_LORA_RegFifoAddrPtr          (0x0D) // 0x08, FIFO SPI pointer
#define SX127X_LORA_RegFifoTxBaseAddr       (0x0E) // 0x02, Start Tx data
#define SX127X_LORA_RegFifoRxBaseAddr       (0x0F) //     , Start Rx data
#define SX127X_LORA_RegFifoRxCurrentAddr    (0x10) //     , start addr of last packet received
#define SX127X_LORA_RegIrqFlagsMask         (0x11) //     , Optional IRQ flag mask
#define SX127X_LORA_RegIrqFlags             (0x12) //     , IRQ flags
#define SX127X_LORA_RegRxNbBytes            (0x13) //     , Number of received bytes
#define SX127X_LORA_RegRxHeaderCntValueMsb  (0x14) //     , Number of valid headers received
#define SX127X_LORA_RegRxHeaderCntValueLsb  (0x15) //     , Number of valid headers received
#define SX127X_LORA_RegRxPacketCntValueMsb  (0x16) //     , Number of valid packets received
#define SX127X_LORA_RegRxPacketCntValueLsb  (0x17) //     , Number of valid packets received
#define SX127X_LORA_RegModemStat            (0x18) //     , Live LoRa modem status
#define SX127X_LORA_RegPktSnrValue          (0x19) //     , Espimation of last packet SNR
#define SX127X_LORA_RegPktRssiValue         (0x1A) //     , RSSI of last packet
#define SX127X_LORA_RegRssiValue            (0x1B) //     , Current RSSI
#define SX127X_LORA_RegHopChannel           (0x1C) //     , FHSS start channel
#define SX127X_LORA_RegModemConfig1         (0x1D) //     , Modem PHY config 1
#define SX127X_LORA_RegModemConfig2         (0x1E) //     , Modem PHY config 2
#define SX127X_LORA_RegSymbTimeoutLsb       (0x1F) //     , Receiver timeout value
#define SX127X_LORA_RegPreambleMsb          (0x20) //     , Size of preamble
#define SX127X_LORA_RegPreambleLsb          (0x21) //     , Size of preamble
#define SX127X_LORA_RegPayloadLength        (0x22) //     , LoRa payload length
#define SX127X_LORA_RegMaxPayloadLength     (0x23) //     , LoRa max payload length
#define SX127X_LORA_RegHopPerid             (0x24) //     , PHSS Hop perid
#define SX127X_LORA_RegFifoRxByteAddr       (0x25) //     , Address of last byte written in FIFO
#define SX127X_LORA_RegModemConfig3         (0x26) //     , Modem PHY config 3
#define SX127X_LORA_RegFeiMsb               (0x28) // 0x55, Estimated frequency error      
#define SX127X_LORA_RegFeiMid               (0x29) // 0x55, Estimated frequency error  
#define SX127X_LORA_RegFeiLsb               (0x2A) // 0x55, Estimated frequency error 
#define SX127X_LORA_RegRssiWideband         (0x2C) // 0x55, Wideband RSSI measurement
#define SX127X_LORA_RegDetectOptimize       (0x31) // 0x40, LoRa detection Optimize for SF6
#define SX127X_LORA_RegInvertIQ             (0x33) // 0x00, Invert LoRa I and Q signals. BIT6: RX IQ invert (1, enable; 0, disable); BIT0: Tx IQ invert (0, enable; 1, disable)
#define SX127X_LORA_RegDetectionThreshold   (0x37) // 0x00, LoRa detection threshold for SF6
#define SX127X_LORA_RegSyncWord             (0x39) // 0x12, LoRa Sync Word

// -----------------

// --- FSK/OOK Mode ---                           - Default - Description -   
#define SX127X_FSK_RegBitratMsb             (0x02) // 0x1A, Bit Rate setting, Most Significant Bits
#define SX127X_FSK_RegBitrateLsb            (0x03) // 0x0B, Bit Rate setting, Least Significant Bits
#define SX127X_FSK_RegFdevMsb               (0x04) // 0x00, Frequency Deviation setting, Most Significant Bits
#define SX127X_FSK_RegFdevLsb               (0x05) // 0x52, Frequency Deviation setting, Least Significant Bits

#define SX127X_FSK_RegRxConfig              (0x0D) // 0x0E, AFC, AGC, ctrl
#define SX127X_FSK_RegRssiConfig            (0x0E) // 0x02, RSSI
#define SX127X_FSK_RegRssiCollision         (0x0F) // 0x0A, RSSI Collision detector
#define SX127X_FSK_RegRssiThresh            (0x10) // 0xFF, RSSI Threshold control
#define SX127X_FSK_RegRssiValue             (0x11) // n/a,  RSSI value in dBm
#define SX127X_FSK_RegRxBw                  (0x12) // 0x15, Channel Filter BW Control
#define SX127X_FSK_RegAfcBw                 (0x13) // 0x0B, AFC Channel Filter BW
#define SX127X_FSK_RegOokPeak               (0x14) // 0x28, OOK demodulator
#define SX127X_FSK_RegOokFix                (0x15) // 0x0C, Threshold of the OOK demod
#define SX127X_FSK_RegOokAvg                (0x16) // 0x12, Average of the OOK demod
#define SX127X_FSK_RegAfcFei                (0x1A) // 0x00, AFC and FEI control
#define SX127X_FSK_RegAfcMsb                (0x1B) // n/a, Frequency correction value of the AFC
#define SX127X_FSK_RegAfcLsb                (0x1C) // n/a, Frequency correction value of the AFC
#define SX127X_FSK_RegFeiMsb                (0x1D) // n/a, Value of the calculated frequency error
#define SX127X_FSK_RegFeiLsb                (0x1E) // n/a, Value of the calculated frequency error
#define SX127X_FSK_RegPreambleDetect        (0x1F) // 0xAA, Settings of the Preamble Detector
#define SX127X_FSK_RegRxTimeout1            (0x20) // 0x00, Timeout Rx request and RSSI
#define SX127X_FSK_RegRxTimeout2            (0x21) // 0x00, Timeout RSSI and PayloadReady
#define SX127X_FSK_RegRxTimeout3            (0x22) // 0x00, Timeout RSSI and SyncAddress
#define SX127X_FSK_RegRxDelay               (0x23) // 0x00, Delay between Rx cycles
#define SX127X_FSK_RegOsc                   (0x24) // 0x07, RC Oscillators Settings, CLKOUT frequency
#define SX127X_FSK_RegPreambleMsb           (0x25) // 0x00, Preamble length, MSB
#define SX127X_FSK_RegPreambleLsb           (0x26) // 0x03, Preamble length, LSB
#define SX127X_FSK_RegSyncConfig            (0x27) // 0x93, Sync Word Recognition control
#define SX127X_FSK_RegSyncValue1            (0x28) // 0x01, Sync Word bytes 1
#define SX127X_FSK_RegSyncValue2            (0x29) // 0x01, Sync Word bytes 2
#define SX127X_FSK_RegSyncValue3            (0x2A) // 0x01, Sync Word bytes 3
#define SX127X_FSK_RegSyncValue4            (0x2B) // 0x01, Sync Word bytes 4
#define SX127X_FSK_RegSyncValue5            (0x2C) // 0x01, Sync Word bytes 5
#define SX127X_FSK_RegPacketConfig1         (0x30) // 0x90, Packet mode settings
#define SX127X_FSK_RegPacketConfig2         (0x31) // 0x40, Packet mode settings
#define SX127X_FSK_RegPayloadLength         (0x32) // 0x40, Payload length setting
#define SX127X_FSK_RegNodeAdrs              (0x33) // 0x00, Node address
#define SX127X_FSK_RegBroadcastAdrs         (0x34) // 0x00, Broadcast address
#define SX127X_FSK_RegFifoThresh            (0x35) // 0x1F, Fifo threshold, Tx start condition
#define SX127X_FSK_RegSeqConfig1            (0x36) // 0x00, Top level Sequencer settings
#define SX127X_FSK_RegSeqConfig2            (0x37) // 0x00, Top level Sequencer settings
#define SX127X_FSK_RegTimerResol            (0x38) // 0x00, Timer 1 and 2 resolution control
#define SX127X_FSK_RegTimer1Coef            (0x39) // 0x12, Timer 1 setting
#define SX127X_FSK_RegTimer2Coef            (0x3A) // 0x20, Timer 2 setting
#define SX127X_FSK_RegImageCal              (0x3B) // 0x02, Image calibration engine control
#define SX127X_FSK_RegTemp                  (0x3C) //       Temperature Sensor value
#define SX127X_FSK_RegLowBat                (0x3D) // 0x02, Low Battery Indicator Settings
#define SX127X_FSK_RegIrqFlags1             (0x3E) // 0x80, Status register: PLL Lock state, Timeout, RSSI
#define SX127X_FSK_RegIrqFlags2             (0x3F) // 0x40, Status register: FIFO handling flags, Low Battery
#define SX127X_FSK_RegPllHop                (0x44) // 0x2D, Control the fast frequency hopping mode
#define SX127X_FSK_RegBitRateFrac           (0x5D) // 0x00, Fractional part in the Bit Rate division ratio
// -------------------- 

// --- Reg field define ---
// RegOpMode
#define SX127X_RegOpMode_MODE_MASK         (0xF8)
#define SX127X_RegOpMode_LORA_ON           (BIT7) // 1 - LoRa Mode; 0 - FSK/OOK Mode
#define SX127X_RegOpMode_MTYPE_MASK        (0x9F) 
#define SX127X_RegOpMode_MTYPE_FSK         (0x00) // 00 -> FSK
#define SX127X_RegOpMode_MTYPE_OOK         (BIT5) // 01-> OOK

//RegModemConfig1
#define SX127X_RegModemConfig1_CR_MASK     (0xF1) // CodingRate 
// bandwidt
#define SX127X_RegModemConfig1_BW_MASK     (0x0F)      // bandwidth mask 
#define SX127X_RegModemConfig1_BW_125K     (0x07 << 4) // 125kHz
#define SX127X_RegModemConfig1_BW_62D5K    (0x06 << 4) // 62.5kHz
#define SX127X_RegModemConfig1_BW_41D7K    (0x05 << 4) // 41.7kHz
#define SX127X_RegModemConfig1_BW_31D25K   (0x04 << 4) // 31.25kHz

// RegModemConfig2
#define SX127X_RegModemConfig2_SF_MASK     (0x0F) // SF mask
#define SX127X_RegModemConfig2_CRC         (BIT2) // CRC on bit
#define SX127X_RegModemConfig2_SMBT_MASK   (0xFC) // SymbTimeout MSB mask

// RegIrqFlagsMask
#define SX127X_RegIrqFlagsMask_CadDetected      (BIT0) // CAD Detected Interrupt Mask
#define SX127X_RegIrqFlagsMask_FhssChange       (BIT1) // FHSS change channel interrupt mask
#define SX127X_RegIrqFlagsMask_CadDone          (BIT2) // CAD complete interrupt mask
#define SX127X_RegIrqFlagsMask_TxDone           (BIT3) // FIFO Payload transmission complete
#define SX127X_RegIrqFlagsMask_ValidHeader      (BIT4) // Valid header received in RX
#define SX127X_RegIrqFlagsMask_PayloadCrcError  (BIT5) // Payload CRC error 
#define SX127X_RegIrqFlagsMask_RxDone           (BIT6) // Packet recption complete 
#define SX127X_RegIrqFlagsMask_RxTimeout        (BIT7) // Rx Timeout

// LORA_RegInvertIQ
#define SX127X_LORA_RegInvertIQ_TX              (BIT0) // 0, enable; 1, disable
#define SX127X_LORA_RegInvertIQ_RX              (BIT6) // 1, enable; 0, disable

// FSK: RegRxConfig, 0x0D
#define SX127X_RegRxConfig_AgcAutoOn            (BIT3) // 0, LNA gain forced by the LnaGain Setting; 1, LNA gain is controlled by the AGC
#define SX127X_RegRxConfig_AfcAutoOn            (BIT4) // 0, No AFC performed at receiver startup; 1, AFC is performed at each receiver statrtup

// FSK: RegPreambleDetect, 0x1F
#define SX127X_RegPreambleDetect_PreambleDetectorOn         (BIT7) // 0, Turned off; 1, Turned on
#define SX127X_RegPreambleDetect_PreambleDetectorSizeMask   (0x60) // 00, 1 byte; 01, 2 bytes; 10, 3 bytes; 11, reserved
#define SX127X_RegPreambleDetect_PreambleDetectorSizeOffset (BIT5)
#define SX127X_RegPreambleDetect_PreambleDetectorTolMask    (0x1F) // 

// FSK: RegPacketConfig1, 0x30
#define SX127X_RegPacketConfig1_PacketFormat                (BIT7) // 0, Fix length; 1, Variable length
#define SX127X_RegPacketConfig1_DcFree_None                 (0x00)
#define SX127X_RegPacketConfig1_DcFree_Manchester           (1 << 5) // 01, Manchester
#define SX127X_RegPacketConfig1_DcFree_Whitening            (2 << 5) // 10, Whitening
#define SX127X_RegPacketConfig1_CrcOn                       (BIT4) // 0, Off; 1, On
#define SX127X_RegPacketConfig1_CrcAutoClearOff             (BIT3) // 0, Clear FIFO and restart new packet reception. No PayloadReady interrupt issued.
                                                                   // 1, Do not clear FIFO. PayloadReady interrupt issued.
#define SX127X_RegPacketConfig1_CrcWhiteningType            (BIT0) // 0, CCITT CRC implementation with standard whitening;
                                                                   // 1, IBM CRC implementation with alternate whitening

// FSK: RegFdevMsb
#define SX127X_RegFdevMsb_MASK                 (0xC0)

// ------------------------
/****
Global variable
****/

/****
Global func
****/

/******************************************************************************/
#endif
