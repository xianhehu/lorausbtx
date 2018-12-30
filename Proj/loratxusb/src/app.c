#include "os.h"
#include "app.h"
#include "hw_config.h"
#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_pwr.h"

static os_tcb_t tcb_app = {0};
static os_tcb_t tcb_at = {0};
static os_tcb_t tcb_gw = {0};

static uint32_t stk_app[64] = {0};
static uint32_t stk_at[64] = {0};
static uint32_t stk_gw[64] = {0};

static uint32_t cnt1 = 0;
static uint32_t cnt2 = 0;

extern void* malloc(int);
extern void  free(void *buf);

void *Malloc(uint16_t len)
{
    void *buf = NULL;
    
    while((buf = (void *)malloc(len)) == NULL) {
        os_delay(2);
    }
    
    return buf;
}

void MFree(void *buf)
{
    free(buf);
}

void ASSERT(bool state)
{
    while(!state);
}

void SysTick_Configuration(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_Config(72000-1);
}

static uint8_t reg = 0;

void TaskGW(void *arg)
{
    SX127X_Init();
    
    GW_Start();
    
    while(true) {
        GW_Task();
        
        os_delay(1);
    }
}

void TaskAT(void *arg)
{
    AT_Run();
}

void TaskAPP(void *arg)
{
    Set_System();

    USB_Interrupts_Config();

    Set_USBClock();

    GW_Init();
    AT_Init();

    USB_Init();

    SysTick_Configuration();

    os_createtsk(&tcb_at, TaskAT, 0, stk_at, 
                 sizeof(stk_at)/sizeof(stk_at[0]), 2);
    os_createtsk(&tcb_gw, TaskGW, 0, stk_gw, 
                 sizeof(stk_gw)/sizeof(stk_gw[0]), 1);

    while(1) { /* idle */
    }
}

void AppStart(void)
{
    os_createtsk(&tcb_app, TaskAPP, 0, stk_app, 
                 sizeof(stk_app)/sizeof(stk_app[0]), 64);
    
    os_run();
}
