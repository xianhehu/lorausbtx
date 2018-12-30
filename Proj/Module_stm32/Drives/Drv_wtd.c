#include "Drv_wtd.h"

static uint8_t wtd_period=0;

/*
*func: configure watchdog period
*ms:   watchdog period(1~43ms)
*/
void WTD_Config(uint8_t ms)
{
    /* Enable WWDG clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    /* WWDG clock counter = (PCLK1 (48MHz)/4096)/8 = 1464Hz (~683 us)  */
    WWDG_SetPrescaler(WWDG_Prescaler_8);

    wtd_period=ms*1000/683+80;
    
    /* Set Window value to 80; WWDG counter should be refreshed only when the counter
    is below 80 (and greater than 64) otherwise a reset will be generated */
    WWDG_SetWindowValue(wtd_period);

    /* Enable WWDG and set counter value to 127, WWDG timeout = ~683 us * 64 = 43.7 ms 
     In this case the refresh window is: ~683 * (127-80)= 32.1ms < refresh window < ~683 * 64 = 43.7ms
     */
    
    WWDG_Enable(wtd_period);
}

void WTD_Feed()
{
    WWDG_SetCounter(wtd_period);
}