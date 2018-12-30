/*======================================
// Drv_Usart_QShell.cͷ�ļ�
//======================================
#include "Drv_Usart_QShell.h"
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
#define	DRV_USART_Q_SHELL_USED		1
#if DRV_USART_Q_SHELL_USED
#include "Q_Shell.h"
#define	Q_SHELL_PORT_USED		PORT_USART1
#define UART_BUF_LEN 128
#endif	//DRV_USART_Q_SHELL_USED
//======================================
//======================================


/********************************/
extern uint8 Drv_Usart_Init(uint8 *gbuf, uint16 gbufmax, uint16 gbufR, PORT_ENUM PortID, uint32 baud, uint16 check);	//���ڳ�ʼ��
extern uint16 Drv_Usart_Read(uint8 *buf, uint16 len, uint8 PortID);
extern uint16 Drv_Usart_Write(uint8 *buf, uint16 len, uint8 PortID);
/********************************/
extern void USART_IRQHandler(uint8 PortID);

/********************************/
#if DRV_USART_Q_SHELL_USED
//======================================
typedef struct /* ����� */
{
	char *gUartBuf;	//����ָ��
	char EscBuf[4];	//����֧���洢��
	uint8 lenmax;	//��󻺴��С
	char Idx;		//�����дָ��
	uint8 EscFlag;	//������ɱ�־
	uint8 EchoEnFlag;	//������ɱ�־��־
// 	uint8 PswLevel;	//����ȼ�(Ԥ��)
// 	uint8 Flag;	//���ֱ�־[������������,QShell�ر�״̬,,,]
}STR_QSHELL_BUF;
extern STR_QSHELL_BUF strQShellBuf;
//======================================
extern void QShell_Init( uint8 *gbuf, uint8 gbufmax );
extern void QShell_OnTimerScan( void );	//Q_Shell��ʱɨ����մ����������Ҫ��ʱ����

//======================================
#endif	//DRV_USART_Q_SHELL_USED


/********************************/
#endif	//DRV_USART_DEF


