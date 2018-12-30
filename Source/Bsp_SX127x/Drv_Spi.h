/*
********************************************************************************
                                 Header File

                    (c) Copyright 2015~2025; Embedded Studio
         All rights reserved.  Protected by international copyright laws!

File        :  Drv_Spi.h
By          :  Flyer
Version     :  V0.1.0
Createdate  :  2016-07-07
----------------------------- Liscensing terms ---------------------------------

********************************************************************************
*/

#include "DataType.h"


#ifndef __SPI_H
#define __SPI_H
/********************************/


/********************************/
#define	DRV_SPI_USED1		1
#define	DRV_SPI_USED2		0


/********************************/
typedef enum {
	PORT_SPI1,	//
// 	PORT_SPI2,	// 
	PORT_SPI_MAX
} PORT_SPI_ENUM;	//Bus define
//======================================



/********************************/
extern void Drv_Spix_Init(PORT_SPI_ENUM PortID);	//Init SPI
extern uint8 Drv_Spix_ReadWriteByte(PORT_SPI_ENUM PortID, uint8 TxData);//SPI read a byte
extern uint16 Drv_Spix_ReadWriteWord(PORT_SPI_ENUM PortID, uint16 TxData);//SPI write a byte

/********************************/
extern sint32 Drv_Spix_Transfer(PORT_SPI_ENUM PortID, const uint8 *tx_buff, uint32 tx_len, uint8 *rev_buff, uint32 rev_len, bool isBlocking);//SPI read/write multi bytes



/********************************/
#define DRV_SPIx_TESTDEMO_EN	0
#if DRV_SPIx_TESTDEMO_EN
void Drv_Spix_TestDemo(PORT_SPI_ENUM PortID);
#endif	//DRV_SPIx_TESTDEMO_EN

/********************************/
#endif	//__SPI_H

