#ifndef __OSAL_H__
#define __OSAL_H__

#include "DataType.h"

typedef void (*task_func_t)(void);

typedef struct {
    uint8_t id;
    task_func_t func;
    uint32_t delay;
} task_t;

typedef struct {
    uint8_t mode;
    uint8_t en;
    uint32_t cnt;
    uint32_t period;
    task_func_t func;
} timer_t;

#define TASK_DESC_DECLARE
static task_t task;

#define osal_createtask(func)  osal_createusertask(&task, func)

void osal_run(void);
void osal_delay(uint8_t task_id, uint32_t ticks);
void osal_sleep(uint32_t ticks);
uint8_t osal_createusertask(task_t *task, task_func_t func);
uint32_t osal_getTime(void);
void osal_timerCreate(timer_t *timer, task_func_t func, uint32_t period, uint8_t mode);
void osal_timerStart(timer_t *timer);
void osal_timerStop(timer_t *timer);
void osal_tick(void);
#endif