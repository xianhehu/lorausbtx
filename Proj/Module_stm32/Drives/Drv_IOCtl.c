/*======================================
// Drv_IOCtl.c文件
// 普通输入输出控制
//======================================
======================================*/
#include "Drv_IOCtl.h"
/********************************/


/********************************/
// IOCtl定义
const uint16_t IOCtl_gpio_pins[IOCtl_PIN_MAX] = { GPIO_Pin_0 , GPIO_Pin_1 , GPIO_Pin_2  };
const GPIO_TypeDef * cIOCtl_gpio_port[IOCtl_PIN_MAX] = { GPIOB, GPIOB, GPIOB };
const uint32_t IOCtl_gpio_port_clk[IOCtl_PIN_MAX] = { RCC_AHBPeriph_GPIOB, RCC_AHBPeriph_GPIOB, RCC_AHBPeriph_GPIOB };
// const uint8 IOCtl_gpio_port_clk_type[IOCtl_PIN_MAX] = { 1, 1, 1, 1 };
const GPIOMode_TypeDef IOCtl_gpio_Mode[IOCtl_PIN_MAX] = { GPIO_Mode_OUT , GPIO_Mode_OUT , GPIO_Mode_OUT  };
const GPIOOType_TypeDef IOCtl_gpio_OType[IOCtl_PIN_MAX] = { GPIO_OType_PP , GPIO_OType_PP , GPIO_OType_PP  };
/********************************/
GPIO_TypeDef **IOCtl_gpio_port;
/********************************/



/********************************/

/********************************/
//======================================
//初始化
//======================================
void Drv_IOCtl_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t i;
	IOCtl_gpio_port = (GPIO_TypeDef **)cIOCtl_gpio_port;
// 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	for( i = 0; i < IOCtl_PIN_MAX; i++ )
	{
		// Enable clock to port.
// 		if(IOCtl_gpio_port_clk_type[i] != 1)
// 			RCC_APB2PeriphClockCmd(IOCtl_gpio_port_clk[i], ENABLE);
// 		else
			RCC_AHBPeriphClockCmd(IOCtl_gpio_port_clk[i], ENABLE);
		// Pin set.
		GPIO_InitStructure.GPIO_Pin = IOCtl_gpio_pins[i];
		GPIO_InitStructure.GPIO_Mode = IOCtl_gpio_Mode[i];
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = IOCtl_gpio_OType[i];
		GPIO_Init(IOCtl_gpio_port[i], &GPIO_InitStructure);
	}
	//======================================
}

//======================================
//IOCtl输出
//======================================
void Drv_IOCtl_Out(uint8_t pin, uint8_t bhl)
{
	if(pin < IOCtl_PIN_MAX)
		IOCtl_gpio_port[pin]->ODR = (IOCtl_gpio_port[pin]->ODR & ~IOCtl_gpio_pins[pin]) | (bhl ? IOCtl_gpio_pins[pin] : 0);
}

//======================================
//IOCtl输出
//======================================
void Drv_IOCtl_Not(uint8_t pin)
{
	uint16_t bhl = ~IOCtl_gpio_port[pin]->ODR & IOCtl_gpio_pins[pin];
	if(pin < IOCtl_PIN_MAX)
		IOCtl_gpio_port[pin]->ODR = (IOCtl_gpio_port[pin]->ODR & ~IOCtl_gpio_pins[pin]) | (bhl ? IOCtl_gpio_pins[pin] : 0);
}
//======================================
//IOCtl输入
//======================================
uint8_t Drv_IOCtl_In(uint8_t pin)
{
	if(pin < IOCtl_PIN_MAX)
	{
		if(IOCtl_gpio_port[pin]->IDR & IOCtl_gpio_pins[pin])
			return 1;
	}
	return 0;
}


//======================================
//IOCtl方向设置
//======================================
void Drv_IOCtl_ModeSet(uint8_t pin, GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	if(pin < IOCtl_PIN_MAX)
	{
		GPIO_InitStructure.GPIO_Pin = IOCtl_gpio_pins[pin];
		GPIO_InitStructure.GPIO_Mode = mode;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(IOCtl_gpio_port[pin], &GPIO_InitStructure);
	}
}

void Drv_IOCtl_PowerOn(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, GPIO_Pin_13);
}


void Drv_IOCtl_GnssOn(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

/********************************/


