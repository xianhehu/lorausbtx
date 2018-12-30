#include "osal.h"

#define MAX_TASKS      10
#define MAX_TIMERS     10

static task_t *task_list[MAX_TASKS] = {0};
static timer_t *timer_list[MAX_TIMERS] = {0};
static uint8_t task_count = 0;
static uint8_t timer_count = 0;
static uint32_t ticks=0;
static uint32_t sleep_ticks=0;

uint8_t osal_createusertask(task_t *task, task_func_t func)
{
    task->id = task_count;
    task->delay = 0;
    task->func = func;
    task_list[task_count++] = task;
    return task->id;
}


void osal_run(void)
{
    while(1) {
        for (int i=0; i<task_count; i++) {
            if (task_list[i]->delay == 0 && task_list[i]->func != NULL) {
                task_list[i]->func();
            }
        }
        
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
        PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
    }
}

void osal_delay(uint8_t task_id, uint32_t ticks)
{
    task_list[task_id]->delay += ticks;
}

void osal_sleep(uint32_t ticks)
{
    sleep_ticks=ticks;
    while(sleep_ticks>0) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
        PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
    }
}

void osal_tick(void)
{
    if (sleep_ticks>0) {
        sleep_ticks--;
    }
    
    for (int i=0; i<task_count; i++) {
        if (task_list[i]->delay > 0) {
            task_list[i]->delay -= 1;
        }
    }
    for (int i=0; i<timer_count; i++) {
        if (timer_list[i]->en == 0) {
            continue;
        }
        if (timer_list[i]->cnt >= timer_list[i]->period) {
            timer_list[i]->func();
            if (timer_list[i]->mode == 0) {
                timer_list[i]->en = 0;
            }
            else {
                timer_list[i]->cnt = 0;
            }
        }
        else {
            timer_list[i]->cnt++;
        }
    }
    ticks++;
}

uint32_t osal_getTime(void)
{
    return ticks;
}

/*timer : pointer to a timer
**func : timer callback function
**period: timer period or timeout
**mode: timer mode, 0:single, 1:periodic
*/
void osal_timerCreate(timer_t *timer, task_func_t func, uint32_t period, uint8_t mode)
{
    /* check if timer exist */
    for (int i=0; i<timer_count; i++) {
        if (timer == timer_list[i]) {
            return;
        }
    }
    timer->cnt = 0;
    timer->mode = mode;
    timer->period = period;
    timer->func = func;
    timer_list[timer_count++] = timer;
}

/*timer : pointer to a timer
*/
void osal_timerStart(timer_t *timer)
{
    timer->en = 1;
    timer->cnt = 0;
}

/*timer : pointer to a timer
*/
void osal_timerStop(timer_t *timer)
{
    timer->en = 0;
    timer->cnt = 0;
}

