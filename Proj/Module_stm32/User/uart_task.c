#include "uart_task.h"
#include "osal.h"
#include "at.h"
#include "Drv_Usart.h"
#include <string.h>

static uint8_t uart_task_id = 0;
static char at_buf[200] = {0};

static void taskdelay(uint32_t ms)
{
    osal_delay(uart_task_id, ms);
}

static void uart_task(void)
{
    static uint32_t len_prev = 0;
    uint32_t len = UART_RxBytes(PORT_USART1);
    
    if (len_prev > 0 && len == len_prev) {
        memset(at_buf, 0, sizeof(at_buf));
        Drv_Usart_Read((uint8_t *)at_buf, sizeof(at_buf), PORT_USART1);
        AT_CmdsDecode(at_buf, len);
        len_prev = 0;
    }
    else if (len > 0) {
        len_prev = len;
    }
    taskdelay(2);
}

void TASK_UartStart(void)
{
    uart_task_id = osal_createtask(uart_task);
}