/*======================================
// Drv_Spi.c文件
//======================================
======================================*/
#include "Drv_Spi.h"
/********************************/
/********************************/

					  

					  
//======================================
//串行外设接口SPI的初始化，SPI配置成主模式							  
//本例程选用SPI1对W25X16进行读写操作，对SPI1进行初始化
//======================================
void Drv_Spix_Init(PORT_SPI_ENUM PortID)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	//--------------------------
	/* SPI1 configuration */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI1设置为两线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                   //设置SPI1为主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	 		               //串行时钟在不操作时，时钟为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		               //第一个时钟沿开始采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			               //NSS信号由软件（使用SSI位）管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //定义波特率预分频的值:波特率预分频值为8
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;						   //CRC值计算的多项式
	//--------------------------
#if DRV_SPI_USED1
	if(PortID == PORT_SPI1)
	{
		/* Enable SPI1 and GPIOA clocks */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
#if 0
		/* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_AF_0;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		//SPI1 NSS (CS)
		GPIO_SetBits(GPIOC, GPIO_Pin_4); 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
#else
		/* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_0);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_0);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_0);
		//SPI1 NSS (CS)
// 		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
// 		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
// 		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
// 		GPIO_Init(GPIOA, &GPIO_InitStructure);
// 		GPIO_SetBits(GPIOA, GPIO_Pin_15); 
// 		GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_1);
#endif

		SPI_Init(SPI1, &SPI_InitStructure);
		/* Enable SPI1  */
		SPI_Cmd(SPI1, ENABLE); 											  //使能SPI1外设
	}
#endif	//DRV_SPI_USED1
	//--------------------------
	//--------------------------
#if DRV_SPIx_TESTDEMO_EN
	Drv_Spix_TestDemo(PORT_SPI1);
#endif	//DRV_SPIx_TESTDEMO_EN
	//--------------------------
}   

//======================================
//SPIx 读写一个字节
//返回值:读取到的字节
//======================================
uint8 Drv_Spix_ReadWriteByte(PORT_SPI_ENUM PortID, uint8 TxData)
{		
	//--------------------------
#if DRV_SPI_USED1
	if(PortID == PORT_SPI1)
	{
		/* Loop while DR register in not emplty */
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		/* Send byte through the SPI1 peripheral */
		SPI_SendData8(SPI1, TxData);
		/* Wait to receive a byte */
// 		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
		/* Return the byte read from the SPI bus */
		return SPI_ReceiveData8(SPI1);
	}
#endif	//DRV_SPI_USED1
	//--------------------------
#if DRV_SPI_USED2
	if(PortID == PORT_SPI2)
	{
		/* Loop while DR register in not emplty */
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
		/* Send byte through the SPI1 peripheral */
		SPI_I2S_SendData(SPI2, TxData);
		/* Wait to receive a byte */
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
		/* Return the byte read from the SPI bus */
		return SPI_I2S_ReceiveData(SPI2);
	}
#endif	//DRV_SPI_USED2
	//--------------------------
	return 0xFF;
}

//======================================
//SPIx 读写一个字
//返回值:读取到的字
//======================================
uint16 Drv_Spix_ReadWriteWord(PORT_SPI_ENUM PortID, uint16 TxData)
{		
	//--------------------------
#if DRV_SPI_USED1
	if(PortID == PORT_SPI1)
	{
		/* Loop while DR register in not emplty */
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		/* Send byte through the SPI1 peripheral */
		SPI_I2S_SendData16(SPI1, TxData);
		/* Wait to receive a byte */
// 		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
		/* Return the byte read from the SPI bus */
		return SPI_I2S_ReceiveData16(SPI1);
	}
#endif	//DRV_SPI_USED1
	//--------------------------
	return 0xFF;
}

//======================================
// isBlocking未使用
//======================================
sint32 Drv_Spix_Transfer(PORT_SPI_ENUM PortID, const uint8 *tx_buff, uint32 tx_len, 
							   uint8 *rev_buff, uint32 rev_len, bool isBlocking)
{
	uint32 i = 0;
#if DRV_SPI_USED1
	while((rev_buff != NULL) && (rev_len > 0) && (rev_len < tx_len));
	if(PortID == PORT_SPI1)
	{
		for(i=0;i<tx_len;i++)
		{
			if(rev_buff != NULL)
				rev_buff[i] = Drv_Spix_ReadWriteByte(PortID, tx_buff[i]);
			else//只写
				Drv_Spix_ReadWriteByte(PortID, tx_buff[i]);
		}
	}
	return i;
#endif	//DRV_SPI_USED1
}



#if DRV_SPIx_TESTDEMO_EN
//======================================
//SPIx 读写测试
//======================================
void Drv_Spix_TestDemo(PORT_SPI_ENUM PortID)
{
	static uint8 dat;
	//--------------------------
#if DRV_SPI_USED1
	if(PortID == PORT_SPI1)
	{
		uint16 d;
		uint8 i,j;
		while(1)
		{
			for (i=1;i;i++)
			{
				for (j=1;j;j++)
				{
					dat = Drv_Spix_ReadWriteByte(PORT_SPI1, i);
					for(d=10;d;d--){}
				}
				for(d=10;d;d--){}
			}
		}
	}
#endif	//DRV_SPI_USED1
	//--------------------------
}
#endif	//DRV_SPIx_TESTDEMO_EN



