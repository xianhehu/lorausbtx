#include "Drv_wtd.h"
#include "osal.h"

static uint8_t wtd_task_id=0;
//static uint32_t count=0;

//static int32_t rtc_ss=0;
static void wtd_task(void)
{
    WTD_Feed();
    //static uint32_t pre=0;
    //uint32_t cur=RTC_GetSubSecond();
    //rtc_ss=cur-pre;
    //pre=cur;
    osal_delay(wtd_task_id, 10);
    //count++;
    //if (count>500) {
        //POW_EnterLowPow(0, 0);
    //}
}

void TASK_WtdStart(void)
{
    WTD_Config(20);
    wtd_task_id=osal_createtask(wtd_task);
    osal_delay(wtd_task_id, 20);
}