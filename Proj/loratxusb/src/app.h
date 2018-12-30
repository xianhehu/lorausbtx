#ifndef _APP_H_

#define _APP_H_

#include "types.h"

#define OS_SEM_AT_RX       1
#define OS_SEM_SX127X_ISR  2
#define OS_SEM_GW_LOCK     3
#define OS_SEM_GWTX_LOCK   4

#define USB_TX_DATA_SIZE 512

void *Malloc(uint16_t len);
void  MFree(void *buf);

void AppStart(void);

void USB_Send_Data(uint8_t data);

#endif