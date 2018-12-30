/*
********************************************************************************
                                 Header File

                    (c) Copyright 2015~2025; Embedded Studio
         All rights reserved.  Protected by international copyright laws!

File        :  Drv_SX127x_IOCTL.h
By          :  Flyer
Version     :  V0.1.0
Createdate  :  2016-07-07
----------------------------- Liscensing terms ---------------------------------

********************************************************************************
*/


#ifndef DRV_SX127x_DEF
#define DRV_SX127x_DEF
/********************************/
#include "Drv_SX127x_IOCTL.h"
#include "radio.h"


/********************************/
// --- LoRa Op mode ---
typedef enum
{
	LORA_MODE_SLEEP = 0,    // 0, Sleep
	LORA_MODE_STDBY,        // 1, Standby
	LORA_MODE_FSTX,         // 2, Frequency synthesis TX (FSTX)
	LORA_MODE_TX,           // 3, Transmit(TX)
	LORA_MODE_FSRX,         // 4, Frequency synthesis RX (FSRX)
	LORA_MODE_RXCONTINUOUS, // 5, Receive continues
	LORA_MODE_RXSINGLE,     // 6, Receive single(RXSINGLE)
	LORA_MODE_CAD           // 7, Channel activity detection (CAD)
}SX127X_LORA_MODE;
// --------------------
// --- FSK Op mode ---
typedef enum
{
	FSK_MODE_SLEEP = 0,    // 0, Sleep
	FSK_MODE_STDBY,        // 1, Standby
	FSK_MODE_FSTX,         // 2, Frequency synthesis TX (FSTX)
	FSK_MODE_TX,           // 3, Transmit(TX)
	FSK_MODE_FSRX,         // 4, Frequency synthesis RX (FSRX)
	FSK_MODE_RX            // 5, Receiver mode (RX)
}SX127X_FSK_MODE;
// -------------------
// --- LoRa param ---
// SF 
#define LORA_PARAM_SF_6          (6)  // SF6: 64
#define LORA_PARAM_SF_7          (7)  // SF7: 128
#define LORA_PARAM_SF_8          (8)  // SF8: 256
#define LORA_PARAM_SF_9          (9)  // SF9: 512
#define LORA_PARAM_SF_10         (10) // SF10: 1024
#define LORA_PARAM_SF_11         (11) // SF11: 2048
#define LORA_PARAM_SF_12         (12) // SF12: 4096
// Coderate
#define LORA_PARAM_CODERATE_4_5  (1) // 4/5
#define LORA_PARAM_CODERATE_4_6  (2) // 4/6
#define LORA_PARAM_CODERATE_4_7  (3) // 4/7
#define LORA_PARAM_CODERATE_4_8  (4) // 4/8
// Preamble length
#define LORA_PREAMBLE_MIN        (4) // preamble length: min
#define LORA_PREAMBLE_STD        (6) // preamble length: standard
#define LORA_PREAMBLE_MAX        (8) // preamble length: max
// ------------------
// --- Define the opt of control() ---
#define SX127X_CONTROL_OPT_RESET               (1) // Reset
#define SX127X_CONTROL_OPT_TX                  (2) // Switch to TX
#define SX127X_CONTROL_OPT_RX                  (3) // Switch to RX
#define SX127X_CONTROL_OPT_EN_DIO0_INT         (4) // Enable DIO_0 interrupt
#define SX127X_CONTROL_OPT_DIS_DIO0_INT        (5) // Disable DIO_0 interrupt 
#define SX127X_CONTROL_OPT_LED_ON              (6) // LED on
#define SX127X_CONTROL_OPT_LED_OFF             (7) // LED off
// -----------------------------------
#define SX127X_BUFF_LEN          (128)
// --- SX127x struct define ---
typedef union
{
	uint32 flag;
	struct
	{
		uint32 chip_type             : 4; // See: chip type define
		// --- LoRa specific param ---
		uint32 sf                    : 4; // Current spreading factor
		uint32 coderate              : 3; // Code rate
		uint32 rx_IQInvert           : 1; // RX IQ invert: 0, disable; 1, enable
		uint32 tx_IQInvert           : 1; // TX IQ invert: 0, disable; 1, enable
        uint32 low_speed             : 1;
        uint32 rxContinous           : 1; // 0:single, 1:continous
        uint32 intcount              : 4;
		// ---------------------------
		uint32 mode                  : 4; // Op mode, see SX127X_LORA_MODE or SX127X_FSK_MODE
		uint32 availBuffLen          : 8; // Availbale buff length
	}bits;  
}SX127X_FLAG;

struct sx127x
{
	struct radio_channel radio;                 // Super struct
	SX127X_FLAG          flag;                   // Flag param  
        uint8                size;
	uint8                *buff;                  // Buffer (save Rx or Tx data)
};
// ----------------------------
extern struct sx127x strSX127x[SX127x_CHIP_MAX];

// -----------------------------------
//======================================



/********************************/
extern void Drv_SX127x_WAIT_MS(uint16 ms);	//等待延时函数(ms)
extern uint8 Drv_SX127x_WAIT_DIO0ms(SX127x_CHIP_ENUM ChipID, uint32 ms);	//超时等待DIO0高电平，0-不超时,1-超时

/********************************/
extern void Drv_SX127x_Init(SX127x_CHIP_ENUM ChipID);	//模块初始化(包含跟SX127x相关的IO和SPI)
extern sint8 Drv_SX127x_Control(SX127x_CHIP_ENUM ChipID, uint32 opt);	//SX127x芯片控制操作
extern sint8 Drv_SX127x_SingleRead(SX127x_CHIP_ENUM ChipID, const uint8 addr, uint8 *read_data);	//单字节读取
extern sint8 Drv_SX127x_SingleWrite(SX127x_CHIP_ENUM ChipID, const uint8 addr, const uint8 write_data);	//单字节写
extern void Drv_SX127x_SetFreq(SX127x_CHIP_ENUM ChipID);	//设置发送频率
extern sint8 Drv_SX127x_CheckVersion(SX127x_CHIP_ENUM ChipID);	//检查芯片型号
extern void Drv_SX127X_RxChainCalibration(SX127x_CHIP_ENUM ChipID);	//接收校准
extern void Drv_SX127x_LoRa_SetMode(SX127x_CHIP_ENUM ChipID, SX127X_LORA_MODE setMode);	//
extern sint8 Drv_SX127x_SetModulation(SX127x_CHIP_ENUM ChipID);	//
extern void Drv_SX127x_LoRa_SetSF(SX127x_CHIP_ENUM ChipID);	//
extern void Drv_SX127x_LoRa_SetCodeRate(SX127x_CHIP_ENUM ChipID);	//
extern void Drv_SX127x_LoRa_SetCRC(SX127x_CHIP_ENUM ChipID);	//
extern void Drv_SX127x_LoRa_SetBandwidth(SX127x_CHIP_ENUM ChipID);	//
extern void Drv_SX127x_LoRa_SetSymbTimeout(SX127x_CHIP_ENUM ChipID, uint16 value);	//
extern void Drv_SX127x_LoRa_SetPreambleLength(SX127x_CHIP_ENUM ChipID);	//
extern void Drv_SX127x_FSK_SetMode(SX127x_CHIP_ENUM ChipID, SX127X_FSK_MODE setMode);
extern void Drv_SX127x_FSK_SetDefaultParam(SX127x_CHIP_ENUM ChipID);
extern void Drv_SX127x_FSK_SetBitrate(SX127x_CHIP_ENUM ChipID);
extern sint8 Drv_SX127x_FSK_SetFdev(SX127x_CHIP_ENUM ChipID);
extern void Drv_SX127x_FSK_SetCRC(SX127x_CHIP_ENUM ChipID);
extern void Drv_SX127x_FSK_SetRxBandwidth(SX127x_CHIP_ENUM ChipID);
extern void Drv_SX127x_FSK_SetPreambleLength(SX127x_CHIP_ENUM ChipID);
extern void Drv_SX127x_SetPABOOST(SX127x_CHIP_ENUM ChipID);
extern void Drv_SX127x_SetOutPower(SX127x_CHIP_ENUM ChipID);
extern uint8 Drv_SX127x_Radio_SymTimeout(SX127x_CHIP_ENUM ChipID);
extern sint8 Drv_SX127x_Radio_DioIntHandle(SX127x_CHIP_ENUM ChipID, uint8_t *len);

/*************** static *****************/
extern void Drv_SX127x_LoRa_Flush_RSSIandSNR(SX127x_CHIP_ENUM ChipID, bool isCurrentRSSI);
extern sint8 Drv_SX127x_BurstRead(SX127x_CHIP_ENUM ChipID, const uint8 addr, uint8 *read_buff, uint8 len);
extern sint8 Drv_SX127x_BurstWrite(SX127x_CHIP_ENUM ChipID, const uint8 addr, uint8 *write_buff, uint8 len);
extern void Drv_SX127x_FSK_Flush_RSSI(SX127x_CHIP_ENUM ChipID);


/********************************/
extern uint8 Drv_SX127x_Radio_Read(SX127x_CHIP_ENUM ChipID, uint8 *buff, uint8 len);
extern uint8 Drv_SX127x_Radio_Write(SX127x_CHIP_ENUM ChipID, const uint8 *buff, const uint8 len);
extern sint8 Drv_SX127x_Radio_Send(SX127x_CHIP_ENUM ChipID, uint32 time_dly);
extern sint8 Drv_SX127x_Radio_Listen(SX127x_CHIP_ENUM ChipID, uint32 timeout, bool isRev);

extern sint8 Drv_SX127x_Radio_Control(SX127x_CHIP_ENUM ChipID, uint32 opt);
extern sint8 Drv_SX127x_Radio_CheckFreqBand(uint16 freq_band, uint32 freq);

extern void Drv_SX127x_Radio_ReadWideRssi(SX127x_CHIP_ENUM ChipID);

extern bool SX127x_IOCtl_IRQPend(void);

/********************************/
#define DRV_SX127x_TEST_EN		0
#if DRV_SX127x_TEST_EN
extern void Drv_SX127x_Test_RadioFIFO(SX127x_CHIP_ENUM ChipID);
#endif	//DRV_SX127x_TEST_EN





/********************************/


/********************************/
#endif	//DRV_SX127x_DEF

