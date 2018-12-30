/*======================================
// Drv_Usart.c�ļ�
// ��������
//======================================
======================================*/
#include "Drv_Usart.h"
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
typedef struct /* ����� */
{
	uint8 *pbuf;    //����ָ��
	uint16 lenmax;	//��󻺴��С
	uint16 lenw;	//д����IP
	uint16 lenr;	//������IP
}STR_GLOB_BUF;
//======================================
/********************************/
typedef struct /* */
{
#if DRV_USART_BUF_TXD_EN
	STR_GLOB_BUF strGlobBuf[2];//�շ������[0-��,1-��]
#else
	STR_GLOB_BUF strGlobBuf[1];//�շ������[0-��,1-��]
#endif  //DRV_USART_BUF_TXD_EN
	PORT_ENUM  portID;	//���ں�
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
 
static char *gUartBuf = NULL;
static uint32_t gUartBufSize = 0;
static uint32_t gUartRxBytes = 0;

/********************************/
//======================================
//���ڳ�ʼ��
//======================================
uint8 Drv_Usart_Init(uint8 *gbuf, uint16 gbufmax, uint16 gbufR, 
                     PORT_ENUM PortID, uint32 baud, uint16 check)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	if(PortID < PORT_MAX)
	{
		if(gbufR > gbufmax)
			return 1;//���ջ���̫��
		//--------------------------
		//�����ʼ��
		strUsartData[PortID].portID = PortID;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenmax = gbufR;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].pbuf = gbuf;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenw = 0;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenr = 0;
#if DRV_USART_BUF_TXD_EN
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_TXD_SEL].lenmax = gbufmax-gbufR;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_TXD_SEL].pbuf = 
                gbuf+strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL].lenmax;
                
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_TXD_SEL].lenw = 0;
		strUsartData[PortID].strGlobBuf[DRV_USART_BUF_TXD_SEL].lenr = 0;
#else
#endif  //DRV_USART_BUF_TXD_EN
		//--------------------------
		//��ʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		//--------------------------
		//�˿ڳ�ʼ��
#if DRV_USART_USED1
		if(PortIDEnum[strUsartData[PortID].portID] == USART1)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
			//����IO��ʼ��
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			// PA9--TXD,PA10--RXD
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
			//TX IO ��ʼ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			//RX IO ��ʼ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
			GPIOA->ODR |= GPIO_Pin_9 | GPIO_Pin_10;
		}
#endif  //DRV_USART_USED1
#if DRV_USART_USED2
		if(PortIDEnum[strUsartData[PortID].portID] == USART2)
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
			//����IO��ʼ��
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
			// PA2--TXD,PA3--RXD
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
			//TX IO ��ʼ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			//RX IO ��ʼ��
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
		//��������
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
		/* ���ڽ����ж�ʹ�� USART_IT_RXNE : ���ղ����ж� */
		USART_ITConfig((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID], USART_IT_RXNE, ENABLE);  //�����ж�
		/* Enable USART */
		USART_Cmd((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID], ENABLE);
		//======================================
		return 0;
	}
	else
		return 1;//�˶˿�δ��ͨ
}

void Drv_Usart_Enable(PORT_ENUM PortID, bool enable)
{
    USART_Cmd((USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID], enable?ENABLE:DISABLE);
}

//======================================
//���ڶ����յ�������
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
//����DMA��������
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
//���ڽ����жϴ���
//======================================
//======================================
void USART_IRQHandler(uint8 PortID)
{
    USART_TypeDef *uart=(USART_TypeDef *)PortIDEnum[strUsartData[PortID].portID];
    
    if(uart->ISR & (USART_FLAG_RXNE))
    {//���յ�����
        STR_GLOB_BUF *gbuf=&strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL];
        
        gbuf->pbuf[gbuf->lenw++] = uart->RDR;
        
        if(gbuf->lenw >= gbuf->lenmax)
            gbuf->lenw = 0;
    }

    uart->ICR = 0xFF;  //���־
}

uint32_t UART_RxBytes(uint8 PortID)
{
    uint16_t bytes=0;
    STR_GLOB_BUF *gbuf=&strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL];
    
    if (gbuf->lenw>=gbuf->lenr) {
        bytes = gbuf->lenw-gbuf->lenr;
    }
    else {
        bytes += gbuf->lenmax-gbuf->lenw;
        bytes += gbuf->lenr;
    }
    
    return bytes;
}

void UART_Clear(uint8 PortID)
{
    STR_GLOB_BUF *gbuf=&strUsartData[PortID].strGlobBuf[DRV_USART_BUF_RXD_SEL];
    
    gbuf->lenr=0;
    gbuf->lenw=0;
}

