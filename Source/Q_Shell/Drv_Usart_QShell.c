/*======================================
// Drv_Usart_QShell.c文件
// 带Q_Shell的串口驱动
//======================================
#include "Drv_Usart.h"
======================================*/
#include "Drv_Usart_QShell.h"
/********************************/
/********************************/


/********************************/
#define	DRV_USART_BUF_TXD_EN		0
//======================================
typedef enum {
	DRV_USART_BUF_RXD_SEL,
	DRV_USART_BUF_TXD_SEL,
	DRV_USART_BUF_MAX
} DRV_USART_BUF_ENUM;
//===============================================
typedef struct /* 缓存池 */
{
	uint8 *pbuf;    //缓存指针
	uint16 lenmax;	//最大缓存大小
	uint16 lenw;	//写缓存IP
	uint16 lenr;	//读缓存IP
}STR_GLOB_BUF;
//======================================
/********************************/
typedef struct /* */
{
#if DRV_USART_BUF_TXD_EN
	STR_GLOB_BUF strGlobBuf[2];//收发缓存池[0-收,1-发]
#else
	STR_GLOB_BUF strGlobBuf[1];//收发缓存池[0-收,1-发]
#endif  //DRV_USART_BUF_TXD_EN
	PORT_ENUM  portID;	//串口号
}STR_USART_DATA;
STR_USART_DATA strUsartData[PORT_MAX];
/********************************/
const 
USART_TypeDef *PortIDEnum[PORT_MAX] = {
#if DRV_USART_USED1
	USART1
#endif  //DRV_USART_USED1
#if DRV_USART_USED2
	,USART2
#endif  //DRV_USART_USED2
};
/********************************/


/********************************/
//======================================
//串口初始化
//======================================
uint8 Drv_Usart_Init(uint8 *gbuf, uint16 gbufmax, uint16 gbufR, PORT_ENUM PortID, uint32 baud, uint16 check)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	if(PortID < PORT_MAX)
	{
		if(gbufR > gbufmax)
			return 1;//接收缓存太大
		//--------------------------
		//缓存初始化
		strUsartData[PortID].portID = PortID;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenmax = gbufR;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].pbuf = gbuf;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenw = 0;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenr = 0;
#if DRV_USART_BUF_TXD_EN
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_TXD_SEL].lenmax = gbufmax-gbufR;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_TXD_SEL].pbuf = gbuf+strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenmax;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_TXD_SEL].lenw = 0;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_TXD_SEL].lenr = 0;
#else
#endif  //DRV_USART_BUF_TXD_EN
		//--------------------------
		//打开时钟
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		//--------------------------
		//端口初始化
#if DRV_USART_USED1
		if(PortIDEnum[strUsartData[PortID].portID] == USART1)
		{
			//串口IO初始化
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			// PA9--TXD,PA10--RXD
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
			//TX IO 初始化
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			//RX IO 初始化
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
			GPIOA->ODR |= GPIO_Pin_9 | GPIO_Pin_10;
		}
#endif  //DRV_USART_USED1
#if DRV_USART_USED2
		if(PortIDEnum[strUsartData[PortID].portID] == USART2)
		{
			//串口IO初始化
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			// PA9--TXD,PA10--RXD
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
			//TX IO 初始化
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			//RX IO 初始化
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
			GPIOA->ODR |= GPIO_Pin_2 | GPIO_Pin_3;
		}
#endif  //DRV_USART_USED2
		//--------------------------
		NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);
		//--------------------------
		//串口设置
		USART_InitStructure.USART_BaudRate				= baud;
		if(check == USART_Parity_No)
			USART_InitStructure.USART_WordLength		= USART_WordLength_8b;
		else
			USART_InitStructure.USART_WordLength		= USART_WordLength_9b;
		USART_InitStructure.USART_StopBits				= USART_StopBits_1;
		USART_InitStructure.USART_Parity				= check ;
		USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode					= USART_Mode_Rx | USART_Mode_Tx;
		USART_Init((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID], &USART_InitStructure);
		/* 串口接收中断使能 USART_IT_RXNE : 接收不空中断 */
		USART_ITConfig((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID], USART_IT_RXNE, ENABLE);  //接收中断
		/* Enable USART */
		USART_Cmd((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID], ENABLE);
		//======================================
		return 0;
	}
	else
		return 1;//此端口未开通
}

//======================================
//串口读接收到的数据
//======================================
uint16 Drv_Usart_Read(uint8 *buf, uint16 len, uint8 PortID)
{
  uint16 i;
  if(PortID < PORT_MAX)
  {
	  if(len == 0)
		  len = strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenmax-1;
	  i = 0;
	  while(strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenr != strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenw)
	  {
		  if(i < len)
		  {
			  buf[i] = strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].pbuf[strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenr];
			  i ++;
			  strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenr ++;
			  if(strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenr >= strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenmax)
				  strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenr = 0;
		  }
		  else
		  {
			  return i;
		  }
	  }
	  return i;
  }
  else
	  return 0;
}

//======================================
//串口DMA发送数据
//======================================
uint16 Drv_Usart_Write(uint8 *buf, uint16 len, uint8 PortID)
{
  //======================================
  uint16 i;
  for(i=0;i<len;i++)
  {
	  USART_SendData((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID],buf[i]);
	  while (!(PortIDEnum[strUsartData[PortID].portID]->ISR & USART_FLAG_TXE));
  }
  //======================================
  return i;
}



//======================================
//======================================
//串口接收中断处理
//======================================
//======================================
void USART_IRQHandler(uint8 PortID)
{
	if(PortIDEnum[strUsartData[PortID].portID]->ISR & (USART_FLAG_RXNE))
	{//接收到数据
		{
			strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].pbuf[strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenw] = PortIDEnum[strUsartData[PortID].portID]->RDR;
			strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenw ++;
			if(strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenw >= strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenmax)
				strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenw = 0;
		}
// 		((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID])->ISR &= ~(USART_FLAG_RXNE);  //清标志
	}

// 	if(PortIDEnum[strUsartData[PortID].portID]->ISR & (USART_FLAG_TC))
// 	{//数据发送完成
// 		((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID])->ISR &= ~(USART_FLAG_TC);  //清标志
// 	}
// 	if(PortIDEnum[strUsartData[PortID].portID]->ISR & (USART_FLAG_FE))
// 	{//数据溢出
// 		((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID])->ISR &= ~(USART_FLAG_FE);  //清标志
// 	}
// 	if(PortIDEnum[strUsartData[PortID].portID]->ISR & (USART_FLAG_TXE))
// 	{//
// 		((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID])->ISR &= ~(USART_FLAG_TXE);  //清标志
// 	}
// 	if(PortIDEnum[strUsartData[PortID].portID]->ISR & (USART_FLAG_ORE))
// 	{//
// 		((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID])->ISR &= ~(USART_FLAG_ORE);  //清标志
// 	}
	((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID])->ICR = 0xFF;  //清标志
}

// void USART_IRQHandler(uint8 PortID)
// #if DRV_USART_USED1
// #endif  //DRV_USART_USED1
// #if DRV_USART_USED2
// #endif  //DRV_USART_USED2
// #if DRV_USART_USED3
// #endif  //DRV_USART_USED3




/********************************/




#if DRV_USART_Q_SHELL_USED
/********************************/
#define Q_SHELL_PORT	((USART_TypeDef *)PortIDEnum[strUsartData[Q_SHELL_PORT_USED].portID])
//======================================
#ifdef __CC_ARM                 /* ARM C Compiler */
#pragma import(__use_no_semihosting)
struct __FILE  
{  
	int handle;  
};  
FILE __stdout;  

_sys_exit(int x)  
{  
	x = x;  
}
int fputc(int ch, FILE *f)
{
	USART_SendData(Q_SHELL_PORT, (unsigned char)ch); 
	while (USART_GetFlagStatus(Q_SHELL_PORT, USART_FLAG_TXE) == RESET);
	return ch;
}
#elif defined (__ICCARM__)      /* for IAR Compiler */
__interwork size_t __write(int handle, const unsigned char *buf, size_t size)
{
	size_t i;
	if( handle >= 0 )
	{
		for(i=0;i<size;i++)
		{
			USART_SendData(Q_SHELL_PORT, (unsigned char)buf[i]);
			while (USART_GetFlagStatus(Q_SHELL_PORT, USART_FLAG_TXE) == RESET);
		}
		return 1;
	}
	else
		return 0;
}
#endif


STR_QSHELL_BUF strQShellBuf;
//======================================
// Q_Shell参数初始化
//======================================
void QShell_Init( uint8 *gbuf, uint8 gbufmax )
{
	strQShellBuf.gUartBuf = (char *)gbuf;
	strQShellBuf.lenmax = gbufmax;
	strQShellBuf.Idx = 0;
	strQShellBuf.EscFlag = 0;
	strQShellBuf.EchoEnFlag = 0;
// 	strQShellBuf.PswLevel = 0xFF;
}

//======================================
// Q_Shell定时扫描接收处理操作，需要定时调用
//======================================
void QShell_OnTimerScan( void )
{
	uint8 Resieve;
	while(Drv_Usart_Read(&Resieve, 1, Q_SHELL_PORT_USED))
	{
		if(strQShellBuf.EscFlag)//溢出了就省略2个字节
		{
			strQShellBuf.EscBuf[strQShellBuf.EscFlag-1] = Resieve;//存入字符
			if(strQShellBuf.EscFlag == 2)//控制字符
			{
				strQShellBuf.EscBuf[2] = 0;
				if(strQShellBuf.EchoEnFlag)
				{
					Drv_Usart_Write("\r\n", 2, Q_SHELL_PORT_USED);//回显
					Q_Sh_CmdHandler(1, strQShellBuf.EscBuf);
				}
				strQShellBuf.EscFlag = 0;
			}
			else	strQShellBuf.EscFlag ++;//正常字符
		}
		else
		{
			if(Resieve == 13)//回车
			{
				strQShellBuf.gUartBuf[strQShellBuf.Idx] = 0;
				if(strQShellBuf.EchoEnFlag)
				{
					Drv_Usart_Write("\r\n", 2, Q_SHELL_PORT_USED);//回显
					Q_Sh_CmdHandler(0, strQShellBuf.gUartBuf);
				}
				strQShellBuf.Idx = 0;
			}
			else if(Resieve==0x08)//回删
			{
				if(strQShellBuf.Idx > 0)
				{
					strQShellBuf.gUartBuf[--strQShellBuf.Idx] = 0;
					if(strQShellBuf.EchoEnFlag)
					{
						Drv_Usart_Write("\b \b", 3, Q_SHELL_PORT_USED);//回显
					}
				}
			}
			else if(Resieve>=0x20)
			{
				strQShellBuf.gUartBuf[strQShellBuf.Idx++] = Resieve;
				if(strQShellBuf.EchoEnFlag)
				{
					if(QShell_PSW_WaitInput)
						Resieve = '*';//回显(密码替换)
					Drv_Usart_Write(&Resieve, 1, Q_SHELL_PORT_USED);//回显
				}
			}
			else if(Resieve == 0x1b)//溢出了
			{
				strQShellBuf.EscFlag = 1;
			}
			if(strQShellBuf.Idx >= UART_BUF_LEN) strQShellBuf.Idx --;//超出范围，就停止加入新的了。
		}
	} // End of while
}



//======================================
/********************************/
#endif	//DRV_USART_Q_SHELL_USED






/********************************/


