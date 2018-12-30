/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
// #include "Debug_Timer.h"
// #include "Drv_Timer.h"
#include "Drv_Usart.h"
// #include "Drv_Adc.h"
#include "Drv_SX127x_IOCTL.h"


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	while (1)
	{
	}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{
	}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
	while (1)
	{
	}
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
	while (1)
	{
	}
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    osal_tick();
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/


/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
*******************************************************************************/
void TIM3_IRQHandler(void)
{
#ifdef DEBUG_TIMER_USED
#if DEBUG_TIMER_USED
	Debug_Timer_IRQHandler();
	TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
#endif	//DEBUG_TIMER_USED
#endif	//DEBUG_TIMER_USED

#ifdef DRV_TIMER_EN
#if DRV_TIMER_EN
	TIM_IRQHandler();
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
#endif	//DRV_TIMER_EN
#endif	//DRV_TIMER_EN
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
*******************************************************************************/
void USART1_IRQHandler(void)
{
#ifdef DRV_USART_USED
#if DRV_USART_USED1
	USART_IRQHandler(PORT_USART1);
#endif  //DRV_USART_USED1
#endif	//DRV_USART_USED
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART1 global interrupt request.
*******************************************************************************/
void USART2_IRQHandler(void)
{
#ifdef DRV_USART_USED
#if DRV_USART_USED2
	USART_IRQHandler(PORT_USART2);
#endif  //DRV_USART_USED2
#endif	//DRV_USART_USED
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{
#ifdef DRV_ADC_USED
#if DRV_ADC_USED
// 	ADC12_IRQHandler();
#endif  //DRV_ADC_USED
#endif  //DRV_ADC_USED
}

/*******************************************************************************
  * @brief  This function handles DMA1 Channel 1 interrupt request.
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
	/* Test on DMA1 Channel1 Transfer Complete interrupt */
#ifdef DRV_ADC_USED
#if DRV_ADC_USED
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		DMA_ADC12_IRQHandler();
// #ifdef DRV_USART_USED
// #if DRV_USART_USED1
// 		Drv_Usart_WriteCntReset(PORT_USART1);
// #endif  //DRV_USART_USED1
// #endif	//DRV_USART_USED
		/* Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits */
		DMA_ClearITPendingBit(DMA1_IT_GL1);
	}
#endif	//DRV_ADC_USED
#endif	//DRV_ADC_USED
}

/*******************************************************************************
  * @brief  This function handles DMA1 Channel 2 interrupt request.
*******************************************************************************/
void DMA1_Channel2_3_IRQHandler(void)
{
  /* Test on DMA1 Channel2 Transfer Complete interrupt */
#ifdef DRV_USART_USED
#if DRV_USART_USED1
//   if(DMA_GetITStatus(DMA1_IT_TC2))
//   {
// 		Drv_Usart_WriteCntReset(PORT_USART1);
// 
//     /* Clear DMA1 Channel2 Half Transfer, Transfer Complete and Global interrupt pending bits */
//     DMA_ClearITPendingBit(DMA1_IT_GL2);
//   }
#endif  //DRV_USART_USED1
#endif	//DRV_USART_USED
}

/*******************************************************************************
* Function Name  : EXTI0_1_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
*******************************************************************************/
void EXTI0_1_IRQHandler(void)
{
#ifdef __SX127x_IOCTL_H
	if(EXTI->PR & EXTI_Line1)
		SX127x_IOCtl_IRQHandler(SX127x_IOCtl_PIN_SPI1_DIO0);
#endif  //__SX127x_IOCTL_H
    EXTI_ClearITPendingBit(EXTI_Line0 | EXTI_Line1);
}

/*******************************************************************************
* Function Name  : EXTI0_1_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
*******************************************************************************/
void EXTI4_15_IRQHandler(void)
{
// #ifdef __SX127x_IOCTL_H
// 	if(EXTI->PR & EXTI_Line1)
// 		SX127x_IOCtl_IRQHandler(SX127x_IOCtl_PIN_SPI1_DIO0);
// #endif  //__SX127x_IOCTL_H
#if 0
    extern void KEY_IRQHandler( void );
    KEY_IRQHandler(  );
#endif
    EXTI_ClearITPendingBit(EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line7);
}




/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
