#include "os.h"

static uint32_t cnt1     = 0;
static uint32_t cnt2     = 0;
static os_tcb_t tcb1     = {0};
static os_tcb_t tcb2     = {0};
static os_tcb_t tcb3     = {0};
static uint32_t stk1[20] = {0};
static uint32_t stk2[20] = {0};
static uint32_t stk3[20] = {0};

void idle(void *arg)
{
    while(1) {
        os_tick();

        Shed();
    }
}

void task1(void *arg)
{
    while(1) {
        cnt1++;
        
        os_delay(10);
    }
}

void task2(void *arg)
{
    while(1) {
        cnt2++;

        os_delay(11);
    }
}

void main(void)
{
    os_createtsk(&tcb1, task1, stk1, sizeof(stk1)/sizeof(stk1[0]), 1);
    os_createtsk(&tcb2, task2, stk2, sizeof(stk1)/sizeof(stk1[0]), 2);
    os_createtsk(&tcb3, idle , stk3, sizeof(stk1)/sizeof(stk1[0]), 3);

    os_run();

    while(1);
}
