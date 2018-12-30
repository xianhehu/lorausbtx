#include "Drv_power.h"
#include "Drv_Usart.h"
#include "Drv_rtc.h"

#include "stm32f0xx_gpio.h"

void SystemClock_Config(void);

typedef struct {
    uint32_t mode;
    uint32_t type;
    uint32_t out;
} gpio_state_t;

static gpio_state_t state_gpioa;
static gpio_state_t state_gpiob;
static gpio_state_t state_gpioc;
static gpio_state_t state_gpiod;
static gpio_state_t state_gpiof;

static void POW_SetWakeUp(uint8_t mode)
{
    if (mode==0) {
        GPIO_InitTypeDef GPIO_InitStructure;
        EXTI_InitTypeDef EXTI_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        
        /* Enable GPIOA clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

        /* Configure PA0 pin as input floating */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* Enable SYSCFG clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
        /* Connect EXTI0 Line to PA0 pin */
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

        /* Configure EXTI0 line */
        EXTI_InitStructure.EXTI_Line = EXTI_Line0;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);

        /* Enable and set EXTI0 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        return;
    }
    
    if (mode != 0) {
        PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);
    }
}

void POW_IOLowPow(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
    
    state_gpioa.mode=GPIOA->MODER;
    state_gpiob.mode=GPIOB->MODER;
    state_gpioc.mode=GPIOC->MODER;
    state_gpiod.mode=GPIOD->MODER;
    state_gpiof.mode=GPIOF->MODER;
    
    state_gpioa.type=GPIOA->OTYPER;
    state_gpiob.type=GPIOB->OTYPER;
    state_gpioc.type=GPIOC->OTYPER;
    state_gpiod.type=GPIOD->OTYPER;
    state_gpiof.type=GPIOF->OTYPER;
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void POW_IOResume(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
    
    GPIOA->MODER=state_gpioa.mode;
    GPIOB->MODER=state_gpiob.mode;
    GPIOC->MODER=state_gpioc.mode;
    GPIOD->MODER=state_gpiod.mode;
    GPIOF->MODER=state_gpiof.mode;
    
    GPIOA->OTYPER=state_gpioa.type;
    GPIOB->OTYPER=state_gpiob.type;
    GPIOC->OTYPER=state_gpioc.type;
    GPIOD->OTYPER=state_gpiod.type;
    GPIOF->OTYPER=state_gpiof.type;
}

void POW_CloseAllPeripheral(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
}

void POW_OpenAllPeripheral(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/*
* mode: 0 stop mode, 1 standby mode
* time: time in ms
*/
void POW_EnterLowPow(uint8_t mode, uint16_t time)
{
    POW_IOLowPow();
    
    if (time>0) {
        RTC_Start(time);
    }
    
    //POW_CloseAllPeripheral();
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
    
    if (mode==0) {
        //POW_SetWakeUp(mode);
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        SystemClock_Config();
        POW_IOResume();
        return;
    }
    
    if (time==0) {
        //POW_SetWakeUp(mode);
    }

    PWR_ClearFlag(PWR_FLAG_WU);
    PWR_EnterSTANDBYMode();
}