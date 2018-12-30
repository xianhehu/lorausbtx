#include "DataType.h"
#include "Drv_IOCtl.h"
#include "Drv_SX127x.h"
#include "osal.h"
#include "Drv_Usart.h"
#include "uart_task.h"


void SystemClock_Config(void)
{
	//-----------------------------------------------
#if 1
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON); // HSE = 4MHz --> 32 MHz range
	RCC_WaitForHSEStartUp();
	RCC_HCLKConfig(RCC_SYSCLK_Div1); // HCLK  = AHBCLK  = PLL / AHBPRES(1) =  48MHz.(max)
	RCC_PCLKConfig(RCC_HCLK_Div1);
	// PCLK = PLL clock must be set from 16MHz --> 48MHz
	RCC_PLLConfig(RCC_PLLSource_HSE, // PLL source: HSE  
		(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL6));
#ifdef HSE_32M  
	RCC_PREDIV1Config(RCC_CFGR2_PREDIV1_DIV4); // PREDIV: /4
#endif // HSE_32M   
	RCC_PLLCmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) 
	{ // Wait for PLL to lock.                              
	}
#if 0
	FLASH_SetLatency(FLASH_Latency_1); // 1 Flash wait states                            
	FLASH_PrefetchBufferCmd(ENABLE);  
#endif
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // HCLK = SYSCLK = PLL                                  
	while (RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL) 
	{ // Check for clock source                               
	}  

#endif
}


//==============================================================================
#define USART_BUF_MAX    256
uint8 usart_buf[PORT_MAX][USART_BUF_MAX];//¡ä??¨²?o¡ä??¡§¨°?
uint8 gps_uart_buf[1024];

int main(void)
{ 
    Drv_IOCtl_Init();
    
    SystemClock_Config();

    if (SysTick_Config(SystemCoreClock / 1000))
    { // 1ms
        /* Capture error */ 
        while (1);
    }
    
    Drv_Usart_Init(usart_buf[PORT_USART1], USART_BUF_MAX, USART_BUF_MAX, PORT_USART1, 9600, USART_Parity_No);

    Drv_SX127x_IOCtl_Init(SX127x_CHIP1);
    Drv_SX127x_Init(SX127x_CHIP1);

    RTC_Config();
#if 1
    TASK_RadioCheckCreate();
    TASK_LmicCreate();
    //lmicapi_start();
    TASK_RadioCheckStart();
#endif

    TASK_UartStart();
    
#ifndef RADIO_DEBUG
    //TASK_WtdStart();
#endif

    osal_run();
    while(1);
}
