/*--------------文件信息--------------------------------------------------------
**文   件   名: UC1701X_Drive.c
**创   建   人: 余大信
**日　      期: 2016-07-18
**描        述: UC1701X_Drive.C文件 12864-LCD驱动程序
**--------------当前版本修订----------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**------------------------------------------------------------------------------
型号: JLX12864G-1353-PN，串行接口
驱动IC是:UC1701X
晶联讯电子：网址  http://www.jlxlcd.cn; http://www.jlxlcd.com.cn
**------------------------------------------------------------------------------
*******************************************************************************/
#include "UC1701X_Drive.h"
// #include "Drv_Port.h"
/******************************************************************************/
//================================= 头文件包含 =================================
// #include "Font_ASCII.h"

//==============================================================================
//显示缓存
uint8 MONLCD_DispBuf[MONLCD_MAX_X][MONLCD_MAX_Y/8];

//================================ IO口操作定义 ================================
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
#define		LCD_CS_H		DRV_PORT_UC1701X_CS->BSRR = DRV_BPORT_UC1701X_CS    //片选端口
#define		LCD_RS_H		DRV_PORT_UC1701X_RS->BSRR = DRV_BPORT_UC1701X_RS	//数据/命令
#define		LCD_SCK_H		DRV_PORT_UC1701X_SCK->BSRR = DRV_BPORT_UC1701X_SCK  //写数据
#define		LCD_SDA_H		DRV_PORT_UC1701X_SDA->BSRR = DRV_BPORT_UC1701X_SDA  //读数据
#define		LCD_RESET_H		DRV_PORT_UC1701X_RST->BSRR = DRV_BPORT_UC1701X_RST  //复位
#define		LCD_BL_H		DRV_PORT_UC1701X_BL->BSRR = DRV_BPORT_UC1701X_BL    //BackLight

#define		LCD_CS_L		DRV_PORT_UC1701X_CS->BRR = DRV_BPORT_UC1701X_CS     //片选端口
#define		LCD_RS_L		DRV_PORT_UC1701X_RS->BRR = DRV_BPORT_UC1701X_RS     //数据/命令
#define		LCD_SCK_L		DRV_PORT_UC1701X_SCK->BRR = DRV_BPORT_UC1701X_SCK   //写数据
#define		LCD_SDA_L		DRV_PORT_UC1701X_SDA->BRR = DRV_BPORT_UC1701X_SDA   //读数据
#define		LCD_RESET_L		DRV_PORT_UC1701X_RST->BRR = DRV_BPORT_UC1701X_RST   //复位
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
//初始化
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
// ms延时
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
// 函数名称: UC1701X_write_data
// 功能描述: 
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
// 函数名称: UC1701X_write_command
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
// 函数名称: UC1701X_LCD_Init
// 功能描述: 
// *****************************************************************************
void UC1701X_LCD_Init()
{
// 	uint8 i,j;
	Drv_Port_UC1701X_Init();
	LCD_BL_L;
	//===============================================
	LCD_RESET_L;	// 低电平复位
	UC1701X_Delay_ms(20);
	LCD_RESET_H;	//复位完毕
	UC1701X_Delay_ms(20);        
	UC1701X_write_command(0xe2);	 /*软复位*/
	UC1701X_Delay_ms(5);
	UC1701X_write_command(0x2c);  /*升压步聚1*/
	UC1701X_Delay_ms(5);	
	UC1701X_write_command(0x2e);  /*升压步聚2*/
	UC1701X_Delay_ms(5);
	UC1701X_write_command(0x2f);  /*升压步聚3*/
	UC1701X_Delay_ms(5);
	UC1701X_write_command(0x24);  /*粗调对比度，可设置范围0x20～0x27*/
	UC1701X_write_command(0x81);  /*微调对比度*/
	UC1701X_write_command(0x1d);  /*微调对比度的值，可设置范围0x00～0x3f*/
	UC1701X_write_command(0xa2);  /*1/9偏压比（bias）*/
	UC1701X_write_command(0xc8);  /*行扫描顺序：从上到下*/
	UC1701X_write_command(0xa0);  /*列扫描顺序：从左到右*/
	UC1701X_write_command(0x40);  /*起始行：第一行开始*/
	UC1701X_write_command(0xaf);  /*开显示*/
	//===============================================
// 	for(i=0;i<(MONLCD_MAX_X);i++)
// 		for(j=0;j<(MONLCD_MAX_Y>>3);j++)
// 			MONLCD_DispBuf[i][j] = 0;
	//===============================================
	UC1701X_LCD_DISACOL(0x05);

}

// *****************************************************************************
// 函数名称: UC1701X_LCD_ContrastSet
// 功能描述: 调整对比度
// *****************************************************************************
// void UC1701X_LCD_ContrastSet(uint8 dat)
// {   
// 	if(dat > 63)
// 		dat = 63;
// 	UC1701X_WR_REG(Set_VRef); //设置参考电压
// 	UC1701X_WR_REG(dat); //42（8.2V) 0~63	//调整对比度
// }   


// *****************************************************************************
// 函数名称: UC1701X_LCD_DISACOL
// 功能描述: 全屏显示某种颜色
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
// 函数名称: UC1701X_LCD_DISACOL
// 功能描述: 全屏显示更新
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
// 函数名称: UC1701X_LCD_DISCLEAR
// 功能描述: 清除显示屏的显示内容
// *****************************************************************************
void UC1701X_LCD_DISCLEAR(void)
{   
	UC1701X_LCD_DISACOL(0x00);
}   

// *****************************************************************************
// 函数名称: UC1701X_LCD_SetRamAddr
// 功能描述: 定义显示地址
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
// 函数名称: UC1701X_LCD_SetAc
// 功能描述: 定义AC
// *****************************************************************************
#if 0
void UC1701X_LCD_SetAc(uint8 x, uint8 y)
{
	UC1701X_write_cmd(0x21);
	UC1701X_write_data16(y,x+4);
	UC1701X_write_cmd(0x22);
}
#endif












