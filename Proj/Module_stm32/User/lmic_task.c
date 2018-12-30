#include "lmic_task.h"
#include "lmicapi.h"
#include "osal.h"
#include "lmic.h"

static uint8_t lmic_task_id=0;
static uint8_t task_done = 1;
static uint8_t lmic_task_test = 0;
uint8_t netstatus = 0;

void TASK_Lmic(void)
{
    if (task_done){
        lmic_delay(10);
        return;
    }

    switch(lmic_task_test) {
    case 1:
        txtest();
        lmic_delay(2000);
        break;
    case 2:
        rxtest();
        break;
    default:
        lmic_task();
        lmic_delay(1);
        break;
    }
}

void TASK_LmicCreate(void)
{
    lmicapi_readparam();
    lmicapi_init();
    
    /* Check if the system has resumed from WWDG reset */
    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET) { /* restart network after watchdog reset */
        /* Clear reset flags */
        RCC_ClearFlag();
        if (flash_param.valid==1) {
            if (flash_param.active==0 && flash_param.netstat !=0) {
                lmicapi_start();
            }
        }
    }

    lmic_task_id=osal_createtask(lmic_task);
}

void lmic_delay(uint32_t ticks)
{
    osal_delay(lmic_task_id, ticks);
}

void lmic_start(void)
{
    CPU_DisableInt();
    task_done = 0;
    lmic_task_test = 0;
    CPU_EnableInt();
    flash_param.netstat = 1;
    lmicapi_saveparam();
}

void lmic_stop(void)
{
    CPU_DisableInt();
    netstatus = NET_STATUS_BREAK;
    task_done = 1;
    CPU_EnableInt();
    flash_param.netstat = 0;
    lmicapi_saveparam();
}

void lmic_txstart(void)
{
    CPU_DisableInt();
    task_done = 0;
    lmic_task_test=1;
    CPU_EnableInt();
}

void lmic_txstop(void)
{
    CPU_DisableInt();
    task_done = 1;
    CPU_EnableInt();
}

void lmic_rxstart(void)
{
    CPU_DisableInt();
    task_done = 0;
    lmic_task_test=2;
    CPU_EnableInt();
}

void lmic_rxstop(void)
{
    CPU_DisableInt();
    task_done = 1;
    CPU_EnableInt();
}