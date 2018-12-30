/*--------------�ļ���Ϣ--------------------------------------------------------
**��   ��   ��: UC1701X_Drive.c
**��   ��   ��: �����
**�ա�      ��: 2016-07-18
**��        ��: UC1701X_Drive.C�ļ� 12864-LCD��������
**--------------��ǰ�汾�޶�----------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**------------------------------------------------------------------------------
�ͺ�: JLX12864G-1353-PN�����нӿ�
����IC��:UC1701X
����Ѷ���ӣ���ַ  http://www.jlxlcd.cn; http://www.jlxlcd.com.cn
**------------------------------------------------------------------------------
*******************************************************************************/
#include "UC1701X_Drive.h"
// #include "Drv_Port.h"
/******************************************************************************/
//================================= ͷ�ļ����� =================================
// #include "Font_ASCII.h"

//==============================================================================
//��ʾ����
uint8 MONLCD_DispBuf[MONLCD_MAX_X][MONLCD_MAX_Y/8];

//================================ IO�ڲ������� ================================
//------------------------------------------------
//------------------------------------------------
//--------------------------------
#ifdef _MCU_STM32_
//--------------------------------
//--------------------------------
// R/S: PB14
#define	DRV_PORT_UC1701X_RS		GPIOB
#define	DRV_BPORT_UC1701X_RS    GPIO_Pin_14
// RST: PA11
#define	DRV_PORT_UC1701X_RST	GPIOA
#define	DRV_BPORT_UC1701X_RST   GPIO_Pin_11
// CS: PA12
#define	DRV_PORT_UC1701X_CS		GPIOA
#define	DRV_BPORT_UC1701X_CS    GPIO_Pin_12
// SCK: PB13
#define	DRV_PORT_UC1701X_SCK	GPIOB
#define	DRV_BPORT_UC1701X_SCK   GPIO_Pin_13
// SDA: PB15
#define	DRV_PORT_UC1701X_SDA	GPIOB
#define	DRV_BPORT_UC1701X_SDA   GPIO_Pin_15
//--------------------------------
// BL: PC13
#define	DRV_PORT_UC1701X_BL		GPIOC
#define	DRV_BPORT_UC1701X_BL    GPIO_Pin_13

//--------------------------------
#define		LCD_CS_H		DRV_PORT_UC1701X_CS->BSRR = DRV_BPORT_UC1701X_CS    //Ƭѡ�˿�
#define		LCD_RS_H		DRV_PORT_UC1701X_RS->BSRR = DRV_BPORT_UC1701X_RS	//����/����
#define		LCD_SCK_H		DRV_PORT_UC1701X_SCK->BSRR = DRV_BPORT_UC1701X_SCK  //д����
#define		LCD_SDA_H		DRV_PORT_UC1701X_SDA->BSRR = DRV_BPORT_UC1701X_SDA  //������
#define		LCD_RESET_H		DRV_PORT_UC1701X_RST->BSRR = DRV_BPORT_UC1701X_RST  //��λ
#define		LCD_BL_H		DRV_PORT_UC1701X_BL->BSRR = DRV_BPORT_UC1701X_BL    //BackLight

#define		LCD_CS_L		DRV_PORT_UC1701X_CS->BRR = DRV_BPORT_UC1701X_CS     //Ƭѡ�˿�
#define		LCD_RS_L		DRV_PORT_UC1701X_RS->BRR = DRV_BPORT_UC1701X_RS     //����/����
#define		LCD_SCK_L		DRV_PORT_UC1701X_SCK->BRR = DRV_BPORT_UC1701X_SCK   //д����
#define		LCD_SDA_L		DRV_PORT_UC1701X_SDA->BRR = DRV_BPORT_UC1701X_SDA   //������
#define		LCD_RESET_L		DRV_PORT_UC1701X_RST->BRR = DRV_BPORT_UC1701X_RST   //��λ
#define		LCD_BL_L		DRV_PORT_UC1701X_BL->BRR = DRV_BPORT_UC1701X_BL     //BackLight

//================================
/********************************/
//================================
void UC1701X_BL_H(void)
{
	LCD_BL_L;
}
void UC1701X_BL_L(void)
{
	LCD_BL_H;
}

//======================================
//��ʼ��
//======================================
void Drv_Port_UC1701X_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	//--------------------------------
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);
	//--------------------------------
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//--------------------------------
	GPIO_InitStructure.GPIO_Pin =  DRV_BPORT_UC1701X_RS;
	GPIO_Init(DRV_PORT_UC1701X_RS, &GPIO_InitStructure);
	//--------------------------------
	GPIO_InitStructure.GPIO_Pin =  DRV_BPORT_UC1701X_RST;
	GPIO_Init(DRV_PORT_UC1701X_RST, &GPIO_InitStructure);
	//--------------------------------
	GPIO_InitStructure.GPIO_Pin =  DRV_BPORT_UC1701X_CS;
	GPIO_Init(DRV_PORT_UC1701X_CS, &GPIO_InitStructure);
	//--------------------------------
	GPIO_InitStructure.GPIO_Pin =  DRV_BPORT_UC1701X_SCK;
	GPIO_Init(DRV_PORT_UC1701X_SCK, &GPIO_InitStructure);
	//--------------------------------
	GPIO_InitStructure.GPIO_Pin =  DRV_BPORT_UC1701X_SDA;
	GPIO_Init(DRV_PORT_UC1701X_SDA, &GPIO_InitStructure);
	//--------------------------------
	GPIO_InitStructure.GPIO_Pin =  DRV_BPORT_UC1701X_BL;
	GPIO_Init(DRV_PORT_UC1701X_BL, &GPIO_InitStructure);
	//--------------------------------
	LCD_BL_H;
	//--------------------------------
}

//--------------------------------
#endif	//_MCU_STM32_
//--------------------------------
/********************************/




// *****************************************************************************
// ms��ʱ
// *****************************************************************************
void UC1701X_Delay_ms(uint16 ms)
{
	uint16 i;
	for(;ms;ms--)
	{
		for(i=7987;i;i--);// @48MHz
	}
}

// *****************************************************************************
// ��������: UC1701X_write_data
// ��������: 
// *****************************************************************************
void UC1701X_write_data(uint8 dat)
{
	char i;
	LCD_CS_L;
	LCD_RS_H;
	for(i=0;i<8;i++) 
	{
		LCD_SCK_L;
		if( dat & 0x80 )
			LCD_SDA_H;
		else
			LCD_SDA_L;
		LCD_SCK_H;
		dat <<= 1;	
	}
	LCD_CS_H;
}


// *****************************************************************************
// ��������: UC1701X_write_command
// *****************************************************************************
void UC1701X_write_command(uint8 dat)
{
	char i;
	LCD_CS_L;
	LCD_RS_L;
	for(i=0;i<8;i++) 
	{
		LCD_SCK_L;
		if( dat & 0x80 )
			LCD_SDA_H;
		else
			LCD_SDA_L;
		LCD_SCK_H;
		dat <<= 1;	
	}
	LCD_CS_H;
}

// *****************************************************************************
// ��������: UC1701X_LCD_Init
// ��������: 
// *****************************************************************************
void UC1701X_LCD_Init()
{
// 	uint8 i,j;
	Drv_Port_UC1701X_Init();
	LCD_BL_L;
	//===============================================
	LCD_RESET_L;	// �͵�ƽ��λ
	UC1701X_Delay_ms(20);
	LCD_RESET_H;	//��λ���
	UC1701X_Delay_ms(20);        
	UC1701X_write_command(0xe2);	 /*��λ*/
	UC1701X_Delay_ms(5);
	UC1701X_write_command(0x2c);  /*��ѹ����1*/
	UC1701X_Delay_ms(5);	
	UC1701X_write_command(0x2e);  /*��ѹ����2*/
	UC1701X_Delay_ms(5);
	UC1701X_write_command(0x2f);  /*��ѹ����3*/
	UC1701X_Delay_ms(5);
	UC1701X_write_command(0x24);  /*�ֵ��Աȶȣ������÷�Χ0x20��0x27*/
	UC1701X_write_command(0x81);  /*΢���Աȶ�*/
	UC1701X_write_command(0x1d);  /*΢���Աȶȵ�ֵ�������÷�Χ0x00��0x3f*/
	UC1701X_write_command(0xa2);  /*1/9ƫѹ�ȣ�bias��*/
	UC1701X_write_command(0xc8);  /*��ɨ��˳�򣺴��ϵ���*/
	UC1701X_write_command(0xa0);  /*��ɨ��˳�򣺴�����*/
	UC1701X_write_command(0x40);  /*��ʼ�У���һ�п�ʼ*/
	UC1701X_write_command(0xaf);  /*����ʾ*/
	//===============================================
// 	for(i=0;i<(MONLCD_MAX_X);i++)
// 		for(j=0;j<(MONLCD_MAX_Y>>3);j++)
// 			MONLCD_DispBuf[i][j] = 0;
	//===============================================
	UC1701X_LCD_DISACOL(0x05);

}

// *****************************************************************************
// ��������: UC1701X_LCD_ContrastSet
// ��������: �����Աȶ�
// *****************************************************************************
// void UC1701X_LCD_ContrastSet(uint8 dat)
// {   
// 	if(dat > 63)
// 		dat = 63;
// 	UC1701X_WR_REG(Set_VRef); //���òο���ѹ
// 	UC1701X_WR_REG(dat); //42��8.2V) 0~63	//�����Աȶ�
// }   


// *****************************************************************************
// ��������: UC1701X_LCD_DISACOL
// ��������: ȫ����ʾĳ����ɫ
// *****************************************************************************
void UC1701X_LCD_DISACOL(uint8 dat)
{
	uint8 i,j;
	for(j=0;j<(MONLCD_MAX_Y>>3);j++)
	{
		UC1701X_write_command(0xb0+j);
		UC1701X_write_command(0x10);
		UC1701X_write_command(0x00);
		for(i=0;i<MONLCD_MAX_X;i++)
			UC1701X_write_data(dat);
	}
}

// *****************************************************************************
// ��������: UC1701X_LCD_DISACOL
// ��������: ȫ����ʾ����
// *****************************************************************************
#if 1
void UC1701X_LCD_DispUpdate(void)
{
	uint16 i,j;
	for(j=0;j<(MONLCD_MAX_Y>>3);j++)
	{
		UC1701X_write_command(0xb0+j);
		UC1701X_write_command(0x10);
		UC1701X_write_command(0x00);
		for(i=0;i<MONLCD_MAX_X;i++)
			UC1701X_write_data(MONLCD_DispBuf[i][j]);
	}
}
#endif

// *****************************************************************************
// ��������: UC1701X_LCD_DISCLEAR
// ��������: �����ʾ������ʾ����
// *****************************************************************************
void UC1701X_LCD_DISCLEAR(void)
{   
	UC1701X_LCD_DISACOL(0x00);
}   

// *****************************************************************************
// ��������: UC1701X_LCD_SetRamAddr
// ��������: ������ʾ��ַ
// *****************************************************************************
#if 0
void UC1701X_LCD_SetRamAddr(uint8 x, uint8 y)
{   
	UC1701X_WR_REG(Set_Page+y);
	UC1701X_WR_REG(Set_Col_MSB+0);
	UC1701X_WR_REG(Set_Col_LSB+0);
	UC1701X_WR_REG(Start_Line+0);
}   
#endif

// *****************************************************************************
// ��������: UC1701X_LCD_SetAc
// ��������: ����AC
// *****************************************************************************
#if 0
void UC1701X_LCD_SetAc(uint8 x, uint8 y)
{
	UC1701X_write_cmd(0x21);
	UC1701X_write_data16(y,x+4);
	UC1701X_write_cmd(0x22);
}
#endif












