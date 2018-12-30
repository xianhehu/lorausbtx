/*
********************************************************************************
                                 SEASON_RTOS

                     (c) Copyright 2015; Season's work
         All rights reserved.  Protected by international copyright laws

File        :  radio_config.h
By          :  Season
Version     :  V0.1.0
Createdate  :  2015-12-10
----------------------------- Liscensing terms ---------------------------------

********************************************************************************
*/

#ifndef RADIO_CONFIG_H
#define RADIO_CONFIG_H

/****
Include
****/
#include "DataType.h"


/****
Defines
****/
#define SX127X_CHIP_TYPE            (2) // 2, SX1278

// --- Config SX1278 ---
#define SX127X_SPI_NAME             ("/dev/spi0") // SPI dev name

// --- PA0: nRESET ---
#define SX127X_RESET_PORT_BIT       (BIT0)
#define SX127X_RESET_HIGH()         (GPIO_SetBits(GPIOA, SX127X_RESET_PORT_BIT))   // High level
#define SX127X_RESET_LOW()          (GPIO_ResetBits(GPIOA, SX127X_RESET_PORT_BIT)) // Low level

// --- PB9: CTL2 (CTX) ---
#define SX127X_CTRL_2_PORT_BIT        (BIT9)
#define SX127X_CTRL_2_HIGH()          (GPIO_SetBits(GPIOB, SX127X_CTRL_2_PORT_BIT))   // High level
#define SX127X_CTRL_2_LOW()           (GPIO_ResetBits(GPIOB, SX127X_CTRL_2_PORT_BIT)) // Low level

// --- PB8: CTL1 (CPS) ---
#define SX127X_CTRL_1_PORT_BIT        (BIT8)
#define SX127X_CTRL_1_HIGH()          (GPIO_SetBits(GPIOB, SX127X_CTRL_1_PORT_BIT))   // High level
#define SX127X_CTRL_1_LOW()           (GPIO_ResetBits(GPIOB, SX127X_CTRL_1_PORT_BIT)) // Low level

#ifndef RF_USE_TCXO
// --- PB7: RXTX ---
#define SX127X_RXTX_PORT_BIT        (BIT7)
#define SX127X_RXTX_HIGH()          (GPIO_SetBits(GPIOB, SX127X_RXTX_PORT_BIT))   // High level
#define SX127X_RXTX_LOW()           (GPIO_ResetBits(GPIOB, SX127X_RXTX_PORT_BIT)) // Low level
#endif

// --- PA1: DIO0 ---
#define SX127X_DIO0_PORT_BIT        (BIT1)
#define SX127X_EN_DIO_INT()         (BSP_IntEn(BSP_INT_ID_EXTI0_1))  // Interrupt enable
#define SX127X_DIS_DIO_INT()        (BSP_IntDis(BSP_INT_ID_EXTI0_1)) // Interrupt disabled

// SX127X param
#define SX127X_DEFAULT_MODULATION         (0)         // 0, RADIO_MTYPE_LORA; 1, RADIO_MTYPE_2GFSK
#define SX127X_DEFAULT_FREQ_BAND          (2)         // Default freq band. 
#define SX127X_DEFAULT_FREQ               (470000000) // Default freq
#define SX127X_DEFAULT_PEND               (1)         // 1, Pend mode; 0, not 
#define SX127X_DEFAULT_BITRATE            (9600)      // Default bitrate
#define SX127X_DEFAULT_POWER              (17)        // default Power
#define SX127X_DEFAULT_CRC                (1)         // CRC on
#define SX127X_DEFAULT_LED                (0)         // LED indication: 1, available; 0, not available
#define SX127X_DEFAULT_USER               (0)         // Radio channel user ID: 0, CLI; 1, MAC

// LoRa param
#define SX127X_LORA_DEFAULT_BANDWIDTH     (125000)    // Default bandwidth
#define SX127X_LORA_DEFAULT_PREAMBLE      (8)         // default preamble len 
#define SX127X_LORA_DEFAULT_SF            (7)        // default SF: 11
#define SX127X_LORA_DEFAULT_CODERATE      (1)         // default coderat: 1-4/5; 2-4/6

// FSK param
#define SX127X_FSK_DEFAULT_RX_BANDWIDTH   (50000)    // FSK default Rx bandwidth 
#define SX127X_FSK_DEFAULT_PREAMBLE_LEN   (8)        // FSK default preamble length
// ---------------------

#define RADIO_PHY_NUM                     (1) // The number of PHY
#define RFD_MAX_STORE_NUM                 (50) // The max number of RFD storage piece
#define RFD_STORE_LIST_NUM                (1)

/****
Global variable
****/

/****
Global func
****/

/******************************************************************************/
#endif
