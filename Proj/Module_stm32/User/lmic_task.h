#ifndef __LMIC_TASK_H__
#define __LMIC_TASK_H__
#include "DataType.h"

void TASK_LmicCreate(void);
void TASK_LmicStart(void);
void lmic_delay(uint32_t ticks);

#endif