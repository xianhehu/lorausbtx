#ifndef __DRV_WTD_H__
#define __DRV_WTD_H__

#include "stm32f0xx_wwdg.h"
#include "stm32f0xx_rcc.h"

void WTD_Config(uint8_t ms);
void WTD_Feed();
#endif