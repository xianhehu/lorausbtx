/*======================================
// Drv_Usart.c头文件
//======================================
#include "Drv_Usart.h"
======================================*/
#include "DataType.h"
/********************************/
/********************************/


#ifndef	DRV_USART_DEF
#define	DRV_USART_DEF
#define	DRV_USART_USED	1
/********************************/
#define	DRV_USART_USED1		1
#define	DRV_USART_USED2		0
#define	DRV_USART_USED3		0
//======================================
typedef enum {
#if DRV_USART_USED1
	PORT_USART1,
#endif  //DRV_USART_USED1
#if DRV_USART_USED2
	PORT_USART2,
#endif  //DRV_USART_USED2
#if DRV_USART_USED3
	PORT_USART3,
#endif  //DRV_USART_USED3
	PORT_MAX
} PORT_ENUM;
//======================================
#define	DRV_USART_Q_SHELL_USED		0
#if DRV_USART_Q_SHELL_USED
#include "Q_Shell.h"
#define	Q_SHELL_PORT_USED		PORT_USART1
#define UART_BUF_LEN 128
#endif	//DRV_USART_Q_SHELL_USED
//======================================
// #define USART1_DMA_CHANNEL			DMA1_Channel2
// #define USART1_DMA_IRQ_CHANNEL		DMA1_Channel2_3_IRQn
//======================================


/********************************/
extern uint8 Drv_Usart_Init(uint8 *gbuf, uint16 gbufmax, uint16 gbufR, PORT_ENUM PortID, uint32 baud, uint16 check);	//串口初始化
extern uint16 Drv_Usart_Read(uint8 *buf, uint16 len, uint8 PortID);
extern uint16 Drv_Usart_Write(uint8 *buf, uint16 len, uint8 PortID);
extern void   Drv_Usart_Enable(PORT_ENUM PortID, bool enable);

/********************************/
extern void USART_IRQHandler(uint8 PortID);

/********************************/
#if DRV_USART_Q_SHELL_USED
//======================================
extern void Q_Sh_Rxd_IRQHandler( void );

extern uint32_t UART_RxBytes(uint8 PortID);

//======================================
#endif	//DRV_USART_Q_SHELL_USED


/********************************/
#endif	//DRV_USART_DEF


