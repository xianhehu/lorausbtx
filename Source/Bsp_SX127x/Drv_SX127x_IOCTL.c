/*======================================
// Drv_SX127x_IOCTL.c文件
//======================================
======================================*/
#include "Drv_SX127x_IOCTL.h"
#include "Drv_SX127x.h"

/********************************/
STR_SX127x_IOCtl_PIN strSX127xIOCtl_Pin[SX127x_CHIP_MAX];
/********************************/
// SX127x_IOCtl_PIN_SPI1_NSS,		PA15
// SX127x_IOCtl_PIN_SPI1_NRESET,	PA0
// SX127x_IOCtl_PIN_SPI1_CTX,		PB9
// SX127x_IOCtl_PIN_SPI1_CPS,		PB8
// SX127x_IOCtl_PIN_SPI1_RXTX,		PB7
// SX127x_IOCtl_PIN_SPI1_DIO0,	/* 接收中断 */		PA1
// IOCtl定义
const uint16 SX127xIOCtl_gpio_pins[SX127x_IOCtl_PIN_MAX] = { GPIO_Pin_15 , GPIO_Pin_0 , GPIO_Pin_9 , GPIO_Pin_8 , GPIO_Pin_7 , GPIO_Pin_1 };
const GPIO_TypeDef * cSX127xIOCtl_gpio_port[SX127x_IOCtl_PIN_MAX] = { GPIOA, GPIOA, GPIOB, GPIOB, GPIOB, GPIOA };
const uint32 SX127xIOCtl_gpio_port_clk[SX127x_IOCtl_PIN_MAX] = { RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB, RCC_AHBPeriph_GPIOB, RCC_AHBPeriph_GPIOB, RCC_AHBPeriph_GPIOA };
const GPIOMode_TypeDef SX127xIOCtl_gpio_Mode[SX127x_IOCtl_PIN_MAX] = { GPIO_Mode_OUT , GPIO_Mode_OUT , GPIO_Mode_OUT , GPIO_Mode_OUT , GPIO_Mode_IN , GPIO_Mode_IN };
const GPIOOType_TypeDef SX127xIOCtl_gpio_OType[SX127x_IOCtl_PIN_MAX] = { GPIO_OType_PP , GPIO_OType_PP , GPIO_OType_PP , GPIO_OType_PP , (GPIOOType_TypeDef)0 , (GPIOOType_TypeDef)0 };
// const GPIOMode_TypeDef SX127xIOCtl_gpio_Mode[SX127x_IOCtl_PIN_MAX] = { GPIO_Mode_OUT , GPIO_Mode_OUT , GPIO_Mode_OUT , GPIO_Mode_IN , GPIO_Mode_IN , GPIO_Mode_IN };
// const GPIOOType_TypeDef SX127xIOCtl_gpio_OType[SX127x_IOCtl_PIN_MAX] = { GPIO_OType_PP , GPIO_OType_PP , GPIO_OType_PP , (GPIOOType_TypeDef)0  , (GPIOOType_TypeDef)0 , (GPIOOType_TypeDef)0 };
/********************************/
// 接收中断IO数量
#define SX127xIOCtl_IRQ_PIN_MAX		1
// 接收中断IO
const DRV_SX127x_IOCtl_PIN_ENUM SX127xIOCtl_IRQ_PIN[SX127xIOCtl_IRQ_PIN_MAX] = { SX127x_IOCtl_PIN_SPI1_DIO0 };
/********************************/
typedef struct /* IO中断配置参数 */
{
	uint32_t	EXTI_Line;	//
	uint8_t		EXTI_PortSourceGPIO;	//
	uint8_t		EXTI_PinSource;	//
	uint8_t		NVIC_IRQChannel;	//
	uint8_t		NVIC_IRQChannelPriority;	//[0~3]
}STR_IOCtl_IRQ;
//======================================
const STR_IOCtl_IRQ strSX127xIOCtl_IRQ[SX127xIOCtl_IRQ_PIN_MAX] = { {EXTI_Line1, EXTI_PortSourceGPIOA, EXTI_PinSource1, EXTI0_1_IRQn, 0} };
/********************************/
GPIO_TypeDef **SX127xIOCtl_gpio_port;



//======================================
//接口数据初始化
//======================================
void Drv_SX127x_IOCtl_DataInit(SX127x_CHIP_ENUM ChipID)
{
	if(ChipID == SX127x_CHIP1)
	{
		strSX127xIOCtl_Pin[ChipID].PortID = PORT_SPI1;
		strSX127xIOCtl_Pin[ChipID].Pin_NSS = SX127x_IOCtl_PIN_SPI1_NSS;
		strSX127xIOCtl_Pin[ChipID].Pin_NRESET = SX127x_IOCtl_PIN_SPI1_NRESET;
		strSX127xIOCtl_Pin[ChipID].Pin_CTX = SX127x_IOCtl_PIN_SPI1_CTX;
		strSX127xIOCtl_Pin[ChipID].Pin_CPS = SX127x_IOCtl_PIN_SPI1_CPS;
		strSX127xIOCtl_Pin[ChipID].Pin_RXTX = SX127x_IOCtl_PIN_SPI1_RXTX;
		strSX127xIOCtl_Pin[ChipID].Pin_DIO0 = SX127x_IOCtl_PIN_SPI1_DIO0;
	}

}


//======================================
//接口初始化
//======================================
void Drv_SX127x_IOCtl_Init(SX127x_CHIP_ENUM ChipID)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8 i;
	if(ChipID >= SX127x_CHIP_MAX)
		return ;
	Drv_SX127x_IOCtl_DataInit(ChipID);
	SX127xIOCtl_gpio_port = (GPIO_TypeDef **)cSX127xIOCtl_gpio_port;
// 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	for( i = 0; i < SX127x_IOCtl_PIN_MAX; i++ )
	{
		// Enable clock to port.
		RCC_AHBPeriphClockCmd(SX127xIOCtl_gpio_port_clk[i], ENABLE);
		// Pin set.
		GPIO_InitStructure.GPIO_Pin = SX127xIOCtl_gpio_pins[i];
		GPIO_InitStructure.GPIO_Mode = SX127xIOCtl_gpio_Mode[i];
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = SX127xIOCtl_gpio_OType[i];
		GPIO_Init(SX127xIOCtl_gpio_port[i], &GPIO_InitStructure);
	}
	//======================================
	// Configure EXTI line
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Enable SYSCFG clock
	for(i=SX127xIOCtl_IRQ_PIN_MAX;i;i--)
	{
		SYSCFG_EXTILineConfig(strSX127xIOCtl_IRQ[i-1].EXTI_PortSourceGPIO, strSX127xIOCtl_IRQ[i-1].EXTI_PinSource); // Connect EXTI1 Line to PA1 pin
		EXTI_InitStructure.EXTI_Line    = strSX127xIOCtl_IRQ[i-1].EXTI_Line;
		EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // Interrupt on rising-edge
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure); 
		/* Enable and set EXTI9_5 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = strSX127xIOCtl_IRQ[i-1].NVIC_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPriority = strSX127xIOCtl_IRQ[i-1].NVIC_IRQChannelPriority;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	//======================================
	// SPI初始化
	Drv_Spix_Init(strSX127xIOCtl_Pin[ChipID].PortID);
	//======================================
	Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, DRV_SX127x_IOCTL_HIG);
	//--------------------------
	//--------------------------
}   

//======================================
// 控制输出
//======================================
void Drv_SX127x_IOCtl_Out(DRV_SX127x_IOCtl_PIN_ENUM PinID, uint8 bhl)
{
	if(PinID < SX127x_IOCtl_PIN_MAX)
		SX127xIOCtl_gpio_port[PinID]->ODR = (SX127xIOCtl_gpio_port[PinID]->ODR & ~SX127xIOCtl_gpio_pins[PinID]) | (bhl ? SX127xIOCtl_gpio_pins[PinID] : 0);
}

//======================================
// 状态输入
//======================================
uint8 Drv_SX127x_IOCtl_In(DRV_SX127x_IOCtl_PIN_ENUM PinID)
{
	if(PinID < SX127x_IOCtl_PIN_MAX)
	{
		if(SX127xIOCtl_gpio_port[PinID]->IDR & SX127xIOCtl_gpio_pins[PinID])
			return 1;
	}
	return 0;
}

//======================================
// IO中断许可
//======================================
void Drv_SX127x_IOCtl_IRQ_EN(DRV_SX127x_IOCtl_PIN_ENUM PinID, FunctionalState bhl)
{
	uint8 i;
	for(i=0;i<SX127xIOCtl_IRQ_PIN_MAX;i++)
	{
		if(PinID == SX127xIOCtl_IRQ_PIN[i])
		{
			if(bhl)
				*(__IO uint32_t *)EXTI_BASE |= SX127xIOCtl_gpio_pins[SX127xIOCtl_IRQ_PIN[i]];
			else
				*(__IO uint32_t *)EXTI_BASE &= ~SX127xIOCtl_gpio_pins[SX127xIOCtl_IRQ_PIN[i]];
// 			EXTI_InitStruct->EXTI_LineCmd = DISABLE;
			break;
		}
	}
}

//======================================
// 接收中断处理
//======================================
void SX127x_IOCtl_IRQHandler(DRV_SX127x_IOCtl_PIN_ENUM PinID)
{
	uint8 i;
	for(i=0;i<SX127xIOCtl_IRQ_PIN_MAX;i++)
	{
		if(PinID == SX127xIOCtl_IRQ_PIN[i])
		{//中断脚
			break;
		}
	}
        strSX127x[SX127x_CHIP1].flag.bits.intcount++;
}

bool SX127x_IOCtl_IRQPend(void)
{
    if (strSX127x[SX127x_CHIP1].flag.bits.intcount > 0) {
        strSX127x[SX127x_CHIP1].flag.bits.intcount--;
        return true;
    }
    return false;
}

void SX127x_IOCtl_IRQClear(void)
{
    strSX127x[SX127x_CHIP1].flag.bits.intcount = 0;
}


//======================================
//======================================




