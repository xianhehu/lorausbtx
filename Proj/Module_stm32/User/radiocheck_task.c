#include "osal.h"
#include "Drv_SX127x.h"
#include "lmiclowlevelapi.h"
#include "radiocheck_task.h"

static uint8_t task_done = 1;

void radiocheck_tsk(void)
{
    uint8_t len = 0;
    if (task_done) {
        return;
    }
    if (SX127x_IOCtl_IRQPend()){
        int8_t state = Drv_SX127x_Radio_DioIntHandle(SX127x_CHIP1, &len);
        RADIO_DioIsrCB(state, len);
    }
    else if (Drv_SX127x_Radio_SymTimeout(SX127x_CHIP1)) {
        Drv_SX127x_Radio_DioIntHandle(SX127x_CHIP1, &len);
        RADIO_DioIsrCB(DIO_ISR_STATE_RXTIMEOUT, 0);
    }
}

void TASK_RadioCheckCreate(void)
{
    osal_createtask(radiocheck_tsk);
}

void TASK_RadioCheckStart(void)
{
    task_done = 0;
}

void TASK_RadioCheckStop(void)
{
    task_done = 1;
}