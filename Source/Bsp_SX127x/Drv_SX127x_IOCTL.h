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

#ifndef __SX127x_IOCTL_H
#define __SX127x_IOCTL_H
/********************************/
#include "Drv_Spi.h"


/********************************/


/********************************/
typedef enum {
	DRV_SX127x_IOCTL_LOW,
	DRV_SX127x_IOCTL_HIG,
	DRV_SX127x_IOCTL_MAX
} PORT_SX127x_IOCTL_OUT_ENUM;
//======================================
typedef enum {
	SX127x_CHIP1,	//
// 	SX127x_CHIP2,	//
	SX127x_CHIP_MAX
} SX127x_CHIP_ENUM;	//chip(Module) define
//======================================
typedef enum {
	SX127x_IOCtl_PIN_SPI1_NSS,	/* CS */
	SX127x_IOCtl_PIN_SPI1_NRESET,	/* Reset */
	SX127x_IOCtl_PIN_SPI1_CTX,	/* Ctl1 */
	SX127x_IOCtl_PIN_SPI1_CPS,	/* Ctl2 */
	SX127x_IOCtl_PIN_SPI1_RXTX,	/* send state */
	SX127x_IOCtl_PIN_SPI1_DIO0,	/* RxTx Int */
	//------------------------
	//------------------------
	SX127x_IOCtl_PIN_MAX
} DRV_SX127x_IOCtl_PIN_ENUM;
//======================================
typedef struct
{
	PORT_SPI_ENUM PortID;    //SPI BUS
	DRV_SX127x_IOCtl_PIN_ENUM	Pin_NSS;	//CS
	DRV_SX127x_IOCtl_PIN_ENUM	Pin_NRESET;	//Reset
	DRV_SX127x_IOCtl_PIN_ENUM	Pin_CTX;	//CTL2 (CTX)
	DRV_SX127x_IOCtl_PIN_ENUM	Pin_CPS;	//CTL1 (CPS)
	DRV_SX127x_IOCtl_PIN_ENUM	Pin_RXTX;	//
	DRV_SX127x_IOCtl_PIN_ENUM	Pin_DIO0;	//
}STR_SX127x_IOCtl_PIN;
//======================================
extern STR_SX127x_IOCtl_PIN strSX127xIOCtl_Pin[SX127x_CHIP_MAX];
//======================================



/********************************/
extern void Drv_SX127x_IOCtl_Init(SX127x_CHIP_ENUM ChipID);	//Init hardware
extern void Drv_SX127x_IOCtl_Out(DRV_SX127x_IOCtl_PIN_ENUM PinID, uint8 bhl);	//GPIO Out control
extern uint8 Drv_SX127x_IOCtl_In(DRV_SX127x_IOCtl_PIN_ENUM PinID);	//GPIO Input
extern void Drv_SX127x_IOCtl_IRQ_EN(DRV_SX127x_IOCtl_PIN_ENUM PinID, FunctionalState bhl);	//GPIO_IN INT Enable/Disable

/********************************/
extern void SX127x_IOCtl_IRQHandler(DRV_SX127x_IOCtl_PIN_ENUM PinID);	//GPIO_IN IRQHandler

bool SX127x_IOCtl_IRQPend(void);
void SX127x_IOCtl_IRQClear(void);



/********************************/


/********************************/
#endif	//__SX127x_IOCTL_H

