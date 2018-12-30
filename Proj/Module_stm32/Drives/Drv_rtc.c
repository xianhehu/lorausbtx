#include "Drv_rtc.h"
#include "stm32f0xx_rtc.h"

static RTC_InitTypeDef   RTC_InitStructure;

void RTC_Config(void)
{
    RTC_TimeTypeDef RTC_TimeStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    EXTI_InitTypeDef EXTI_InitStructure;
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    RTC_WriteProtectionCmd(DISABLE);
    RTC_EnterInitMode();
    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);
#if 0
    /* LSI used as RTC source clock */
    /* The RTC Clock may varies due to LSI frequency dispersion. */   
    /* Enable the LSI OSC */ 
    RCC_LSICmd(DISABLE);
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#else
    RCC_LSEConfig(RCC_LSE_OFF);
    RCC_LSEConfig(RCC_LSE_ON);
    
    /* Wait till LSI is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }
    
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#endif

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
#if 0
    /* Calendar Configuration */
    RTC_InitStructure.RTC_AsynchPrediv = 99;
    RTC_InitStructure.RTC_SynchPrediv  = 399; /* (40KHz / 1) - 1 = 399*/
#else
    RTC_InitStructure.RTC_AsynchPrediv = 63;
    RTC_InitStructure.RTC_SynchPrediv  = 511; /* 64*512=32768 */
#endif
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);  

    /* EXTI configuration *****************************************************/
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Wakeup Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Set the time to 00h 00mn 00s AM */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x00;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0x00;

    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
}

uint8_t rtcstart=0;

void RTC_Start(uint32_t msecs)
{
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_AlarmTypeDef RTC_AlarmStructure;
    
    RTC_AlarmStructInit(&RTC_AlarmStructure);
    
    /* Set the time to 00h 00mn 00s AM */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x00;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0x00;  

    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
#if 0
    msecs+=999-(RTC_GetSubSecond()*2.5);
#else
    msecs+=999-(RTC_GetSubSecond()*1000/512);
#endif
    uint32_t secs=msecs/1000;
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    /* Set the alarm X+5s */
    RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = secs/3600;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = (secs%3600)/60;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = secs%60;
    RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
#if 0
    RTC_AlarmSubSecondConfig(RTC_Alarm_A, 
                             (uint32_t)((999-((msecs-1)%1000))*0.4), 
                             RTC_AlarmSubSecondMask_SS14_11);
#else
    RTC_AlarmSubSecondConfig(RTC_Alarm_A, 
                             (uint32_t)((999-((msecs-1)%1000))*0.512), 
                             RTC_AlarmSubSecondMask_SS14_11);
#endif

    RTC_ITConfig(RTC_IT_ALRA, ENABLE);

    /* Enable the alarm */
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
    
    rtcstart=1;
}

void RTC_IRQHandler(void)
{
    rtcstart=0;
    /* Set the time to 00h 00mn 00s AM */
    /*RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x00;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0;  
  
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);*/
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    EXTI_ClearITPendingBit(EXTI_Line17);
}